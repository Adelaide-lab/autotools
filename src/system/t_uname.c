#define _GNU_SOURCE
#include <sys/utsname.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{

    struct utsname uts;

    if (uname(&uts) == -1)
        errExit("uname fail");

    fprintf(stdout, "Node name:       %s\n", uts.nodename);
    fprintf(stdout, "System name:     %s\n", uts.sysname);
    fprintf(stdout, "Release:         %s\n", uts.release);
    fprintf(stdout, "Version:         %s\n", uts.version);
    fprintf(stdout, "Machine:         %s\n", uts.machine);
#ifdef _GNU_SOURCE
    fprintf(stdout, "Domain name:     %s\n", uts.domainname);
#endif
    exit(EXIT_SUCCESS);
}