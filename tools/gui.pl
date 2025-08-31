#!/usr/bin/env perl
# $Id$

my $in = $ARGV[0];
my %menus = ();
my @menus_order = ();

my @vars = ();
my $lastmenu = "";

sub make_id {
	my $s = uc($_[0]);

	$s =~ s/[^a-zA-Z0-9_ ]//g;
	$s =~ s/ /_/g;

	return "ID_" . $s;
}

sub banner {
	print OUT ("/**\n");
	print OUT (" * This file is auto-generated using ./tool/gui.pl\n");
	print OUT (" * DO NOT EDIT MANUALLY\n");
	print OUT (" */\n");
	print OUT ("\n");
}

sub args {
	my @r = ();
	my @l = split(/ /, $_[0]);

	my $dq = 0;
	my $str = "";

	foreach my $s (@l){
		my $count = $s =~ s/"//g;
		$count = $count + 0;

		if($dq && ($count % 2 == 1)){
			$dq = 0;
			push(@r, "$str $s");
		}elsif($count % 2 == 1){
			$dq = 1;
			$str = $s;
		}elsif($dq){
			$str = "$str $s";
		}else{
			push(@r, $s);
		}
	}

	return @r;
}

open(IN, "<", "$in") or die;
while(my $l = <IN>){
	$l =~ s/\r?\n$//g;
	$l =~ s/^[ \t]+//;
	if($l =~ /^[ \t]*$/){
	}elsif(!($l =~ /^#/)){
		my @al = args($l);
		if($al[0] eq "Set"){
			$al[1] =~ /^([^\=]+)\=(.+)$/;
			$vars{$1} = $2;
		}elsif($al[0] eq "Popup"){
			$menus{$al[1]} = ();
			push(@menus_order, $al[1]);
			$lastmenu = $al[1];
		}elsif($al[0] eq "HelpPopup"){
			$menus{"#$al[1]"} = ();
			push(@menus_order, "#$al[1]");
			$lastmenu = "#$al[1]";
		}elsif($al[0] eq "MenuItem"){
			push(@{$menus{$lastmenu}}, $al[1]);
		}elsif($al[0] eq "MenuSeparator"){
			push(@{$menus{$lastmenu}}, "%SEPARATOR");
		}
	}
}
close(IN);

printf("Generating $vars{EmbedFile}\n");

open(OUT, ">", "$vars{EmbedFile}");
banner();
print OUT ("#include <fishymail.h>\n");
print OUT ("");
print OUT ("const char* ui_text =");

open(IN, "<", "$in");
while(my $l = <IN>){
	$l =~ s/\r?\n//g;
	$l =~ s/\\/\\\\/g;
	$l =~ s/"/\\"/g;
	print OUT (" \\\n\t\"$l\\n\"");
}
print OUT (";\n");
close(IN);

close(OUT);

my $incr = 100;

printf("Generating $vars{ResourceHeader}\n");

open(OUT, ">", "$vars{ResourceHeader}");
banner();
print OUT ("#ifndef __GUIRES_H__\n");
print OUT ("#define __GUIRES_H__\n");
print OUT ("\n");
foreach my $s (@menus_order){
	foreach my $i (@{$menus{$s}}){
		my $id = make_id("menu_${s}_${i}");
		print OUT ("#define $id $incr\n");
		$incr = $incr + 1;
	}
}
print OUT ("\n");
print OUT ("#endif\n");
close(OUT);

print ("Running yacc/lex\n");
system("yacc -d src/ui.y") == 0 or die;
system("lex --nounistd src/ui.l") == 0 or die;

rename("y.tab.c", "src/ui.tab.c");
rename("y.tab.h", "include/ui.tab.h");
rename("lex.yy.c", "src/ui.yy.c");
