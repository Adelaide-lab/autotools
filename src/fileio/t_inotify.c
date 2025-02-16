#include <sys/inotify.h>
#include <limits.h>
// #include <sys/limits.h>
#include "tlpi_hdr.h"

static void displayInotifyEvent(struct inotify_event *i)
{
    printf("    wd =%2d; \n", i->wd);
    if (i->cookie > 0)
        printf("cookie = %4d; ", i->cookie);
    printf("mask =\n");
    if (i->mask & IN_ACCESS)
        printf("    IN_ACCESS   \n");
    if (i->mask & IN_ATTRIB)
        printf("    IN_ATTRIB   \n");
    if (i->mask & IN_CLOSE_NOWRITE)
        printf("    IN_CLOSE_NOWRITE    \n");
    if (i->mask & IN_CLOSE_WRITE)
        printf("    IN_CLOSE_WRITE  \n");
    if (i->mask & IN_CREATE)
        printf("    IN_CREATE   \n");
    if (i->mask & IN_DELETE)
        printf("    IN_DELETE   \n");
    if (i->mask & IN_DELETE_SELF)
        printf("    IN_DELETE_SELF  \n");
    if (i->mask & IN_IGNORED)
        printf("    IN_IGNORED  \n");
    if (i->mask & IN_ISDIR)
        printf("    IN_ISDIR    \n");
    if (i->mask & IN_MODIFY)
        printf("    IN_MODIFY   \n");
    if (i->mask & IN_MOVE_SELF)
        printf("    IN_MOVE_SELF   \n");
    if (i->mask & IN_MOVED_FROM)
        printf("    IN_MOVED_FROM   \n");
    if (i->mask & IN_MOVED_TO)
        printf("    IN_MOVED_TO   \n");
    if (i->mask & IN_OPEN)
        printf("    IN_OPEN   \n");
    if (i->mask & IN_Q_OVERFLOW)
        printf("    IN_Q_OVERFLOW   \n");
    if (i->mask & IN_UNMOUNT)
        printf("    IN_UNMOUNT   \n");

    if (i->len > 0)
        printf("        name = %s\n", i->name);
}

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int main(int argc, char *argv[])
{
    int inotifyFd, wd, j;
    char buf[BUF_LEN];
    ssize_t numRead;
    char *p;
    struct inotify_event *event;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s pathname.... \n", argv[0]);

    inotifyFd = inotify_init();
    if (inotifyFd == -1)
        errExit("inotify_init");

    printf("%s init pid:%d\n", argv[0], getpid());

    for (j = 1; j < argc; j++)
    {
        wd = inotify_add_watch(inotifyFd, argv[j], IN_ALL_EVENTS);
        if (wd == -1)
            errExit("inotify_add_watch");

        printf("Watching %s using wd %d\n", argv[j], wd);
    }

    while (1)
    {
        numRead = read(inotifyFd, buf, BUF_LEN);
        if (numRead == 0)
            fatal("read() from inotify id returned 0");

        if (numRead == 1)
            errExit("read");

        // printf("Read %ld bytes from inotify fd\n", numRead);

        for (p = buf; p < buf + numRead;)
        {
            event = (struct inotify_event *)p;
            displayInotifyEvent(event);

            p += sizeof(struct inotify_event) + event->len;
        }
    }

    exit(EXIT_SUCCESS);
}
