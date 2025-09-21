                                   FishyMail

   FishyMail is a lightweight, and simple email client written in C89.

   This release supports following platforms:
     * NetBSD 10.0 or newer
     * Windows 95/NT 4 or newer
     * Debian GNU/Linux 12.0 or newer

   with following GUI:
     * Motif (default) or GTK2 for Unix and Unix-likes
     * Pure Win32 API for Windows

   and with following features:
     * SASL authentication, using Cyrus SASL 2.x
     * SSL/TLS using OpenSSL

   Source code is available at:
     * [1]Subversion
     * [2]GitHub
     __________________________________________________________________

Building

   Building with makefiles in ostype directory should be enough for normal
   users. But, you can also modify config.mk to change default options.
     __________________________________________________________________

Makefile variables

     P_CC    C compiler used to compile FishyMail
   P_CFLAGS  Platform dependent flags for compiling
   P_LDFLAGS Platform dependent flags for linking
    P_LIBS   Platform dependent libraries
   C_CFLAGS  Config dependent flags for compiling
   C_LDFLAGS Config dependent flags for linking
    C_LIBS   Config dependent libraries
   U_CFLAGS  User customizable flags for compiling
   U_LDFLAGS User customizable flags for linking
    U_LIBS   User customizable libraries
         If you're using Unix or Unix-likes
    UNIX_UI  motif (default) or gtk2
     __________________________________________________________________

Known "issues"

  Platforms with musl libc

   Because thread-safe functions for libresolv are missing in musl libc,
   you have to compile with:

     U_CFLAGS=-DUSE_UNSAFE_DNS

   or use ostype/musl.mk.

  GTK2 Backend

   Cannot shrink the window.
     __________________________________________________________________

Contact

     * Nishi <nishi@nishi.boats>

References

   1. http://svn2.nishi.boats/svn/fishymail
   2. https://github.com/nishiowo/fishymail
