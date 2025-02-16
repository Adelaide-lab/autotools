#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

int main()
{
    int fd, wd;
    char buffer[EVENT_BUF_LEN];

    fd = inotify_init();
    if (fd < 0)
    {
        perror("inotify_init");
        return 1;
    }

    wd = inotify_add_watch(fd, ".", IN_CREATE | IN_DELETE | IN_MODIFY);
    if (wd < 0)
    {
        perror("inotify_add_watch");
        close(fd);
        return 1;
    }

    printf("Inotify 监控进程已启动，PID 为 %d\n", getpid());

    while (1)
    {
        int length = read(fd, buffer, EVENT_BUF_LEN);
        if (length < 0)
        {
            perror("read");
            break;
        }

        int i = 0;
        while (i < length)
        {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            if (event->mask & IN_CREATE)
            {
                if (event->mask & IN_ISDIR)
                {
                    printf("新目录 %s 被创建。\n", event->name);
                }
                else
                {
                    printf("新文件 %s 被创建。\n", event->name);
                }
            }
            else if (event->mask & IN_DELETE)
            {
                if (event->mask & IN_ISDIR)
                {
                    printf("目录 %s 被删除。\n", event->name);
                }
                else
                {
                    printf("文件 %s 被删除。\n", event->name);
                }
            }
            else if (event->mask & IN_MODIFY)
            {
                if (event->mask & IN_ISDIR)
                {
                    printf("目录 %s 被修改。\n", event->name);
                }
                else
                {
                    printf("文件 %s 被修改。。\n", event->name);
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);

    return 0;
}