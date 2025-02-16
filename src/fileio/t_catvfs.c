#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include "tlpi_hdr.h"

const char *get_filesystem_type(const char *file_path)
{
    struct statvfs fs_info;
    if (statvfs(file_path, &fs_info) == -1)
    {
        perror("statfs");
        return NULL;
    }

    FILE *mounts_file = fopen("/proc/mounts", "r");
    if (mounts_file == NULL)
    {
        perror("fopen");
        return NULL;
    }

    char line[1024];
    while (fgets(line, sizeof(line), mounts_file) != NULL)
    {
        char device[256], mount_point[256], fs_type[256];
        if (sscanf(line, "%255s %255s %255s", device, mount_point, fs_type) == 3)
        {
            struct statvfs mount_fs_info;
            if (statvfs(mount_point, &mount_fs_info) == 0)
            {
                if (fs_info.f_fsid == mount_fs_info.f_fsid)
                {
                    fclose(mounts_file);
                    return strdup(fs_type);
                }
            }
        }
    }

    fclose(mounts_file);
    return NULL;
}

int main(int argc, char *argv[])
{

    struct statvfs vfs_info;
    const char *path;

    if (argc < 2)
    {
        path = ".";
    }
    else
    {
        path = argv[1];
    }

    if (statvfs(path, &vfs_info) == -1)
    {
        perror("statvfs error\n");
        return -1;
    }

    fprintf(stdout, "File system information for %s:\n", path);
    fprintf(stdout, "File system name:%s\n", get_filesystem_type(path));
    fprintf(stdout, "Total space:%lu bytes\n", (unsigned long)vfs_info.f_blocks * vfs_info.f_frsize);
    fprintf(stdout, "Free space:%lu bytes\n", (unsigned long)vfs_info.f_bfree * vfs_info.f_frsize);
    fprintf(stdout, "Available space:%lu bytes\n", (unsigned long)vfs_info.f_bavail * vfs_info.f_frsize);
    fprintf(stdout, "Total inodes:%lu\n", (unsigned long)vfs_info.f_files);
    fprintf(stdout, "Free inodes:%lu\n", (unsigned long)vfs_info.f_ffree);
    fprintf(stdout, "Filesystem block size:%lu bytes\n", (unsigned long)vfs_info.f_frsize);
    fprintf(stdout, "Fragment size:%lu bytes\n", (unsigned long)vfs_info.f_bsize);
}