#!/usr/bin/env perl
# $Id$

my $cmdline = "owcc";

foreach my $arg (@ARGV) {
    if ($arg =~ /^-l(.+)$/) {
        $cmdline = $cmdline . " $1.lib";
    }
    elsif (not($arg =~ /\.res$/)) {
        $cmdline = $cmdline . " $arg";
    }
}

exec("$cmdline");
