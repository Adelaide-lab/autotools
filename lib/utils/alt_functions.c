/* ALT_functions.c

   Some library functions on Linux are not available on other UNIX
   implementations. Below are some implementations (in some cases quite
   minimal) of those functions used by our programs.

   Each of these functions has a name of the form ALT_xxx() where xxx() is the
   function being replaced. (#defines are used elsewhere to convert the
   standard names into these alternate forms.)
*/
#include <stdio.h>
#include <fcntl.h>
#include "alt_functions.h"

/* A very minimal implementation of strsignal()... */

#define BUF_SIZE 100
char *
ALT_strsignal(int sig)
{
    static char buf[BUF_SIZE]; /* Not thread-safe */

    snprintf(buf, BUF_SIZE, "SIG-%d", sig);
    return buf;
}
/* A very minimal implementation of hstrerror()... */

char *
ALT_hstrerror(int err)
{
    static char buf[BUF_SIZE]; /* Not thread-safe */

    snprintf(buf, BUF_SIZE, "hstrerror-%d", err);
    return buf;
}
/* posix_openpt() is simple to implement */

int ALT_posix_openpt(int flags)
{
    return open("/dev/ptmx", flags);
}