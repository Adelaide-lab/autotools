#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <libgen.h>
#include "tlpi_hdr.h"

#define MAX_COMMAND_LENGTH 256
#define MAX_BRANCH_LENGTH 128
#define MAX_STRING_LENGTH 1024

static struct option long_opts[] = {
    {"add", required_argument, NULL, 'a'},
    {"remove", required_argument, NULL, 'r'},
    {"help", no_argument, NULL, 'h'},
    {"list", no_argument, NULL, 'l'},
    {"exist", required_argument, NULL, 'e'},
    {"check", required_argument, NULL, 'c'},
    {NULL, 0, NULL, 0}};

static void t_worktree_usgae(const char *filename)
{
    printf("Usage: %s [option] branch_name\n", filename);
    printf("[option] is one of the followings:\n");
    printf("  -a, --add  add branch in current worktree\n");
    printf("  -r, --remove remove branch from current worktree\n");
    printf("  -l, --list list all worktree branch and directory\n");
    printf("  -e, --exit check if branch in already in worktree\n");
    printf("  -c, --check check if branch in remote\n");
    printf("  -h, --help print this information\n");
    exit(EXIT_FAILURE);
}

static bool t_worktree_check(const char *branch_name, bool print)
{
    FILE *fp;
    int ret;
    unsigned long hash;
    bool exist = false;
    char branch[MAX_BRANCH_LENGTH];

    fp = popen("git branch --remote", "r");
    if (fp == NULL)
    {
        errExit("Execute list failed");
        goto execute_fail;
    }

    while (fgets(branch, MAX_BRANCH_LENGTH, fp) != NULL)
    {
        if (strstr(branch, branch_name))
        {
            exist = true;
            break;
        }
    }

    if (print)
    {
        fprintf(stdout, "%s is %s remote repository\n", branch_name, exist ? "in" : "not in");
    }

malloc_fail:
    pclose(fp);
execute_fail:
    return exist;
}

static bool t_worktree_handle(const char *branch_name, bool delete)
{

    if (!t_worktree_check(branch_name, false))
    {
        errExit("%s in not in remote", branch_name);
        return false;
    }

    FILE *fp;
    int ret, hash;
    bool find = false;
    fprintf(stdout, "branch name:%s\n", branch_name);

    struct stat file_stat;
    char command[MAX_COMMAND_LENGTH];
    char branch[MAX_BRANCH_LENGTH];
    char dir_name[MAX_BRANCH_LENGTH];

    snprintf(command, MAX_COMMAND_LENGTH, "git worktree list");

    fp = popen(command, "r");
    if (fp == NULL)
    {
        errMsg("Execute %s Failed", command);
        goto exe_error;
    }

    char *str = (char *)malloc(MAX_STRING_LENGTH);

    while (fgets(str, MAX_STRING_LENGTH, fp) != NULL)
    {
        ret = sscanf(str, "%s %x %s", dir_name, &hash, branch);
        if (strstr(branch, branch_name))
        {
            find = true;
            break;
        }
    }

    if (stat(dir_name, &file_stat) == -1)
    {
        errMsg("No workstatic bool t_worktree_handle(const char* branch_name, bool delete) {

    if (!t_worktree_check(branch_name, false)) {
            errExit("%s in not in remote", branch_name);
            return false;
    }

    FILE *fp;
    int ret, hash;
    bool find = false;
    fprintf(stdout, "branch name:%s\n", branch_name);

    struct stat file_stat;
    char command[MAX_COMMAND_LENGTH];
    char branch[MAX_BRANCH_LENGTH];
    char dir_name[MAX_BRANCH_LENGTH];

    snprintf(command, MAX_COMMAND_LENGTH, "git worktree list");

    fp = popen(command, "r");
    if (fp == NULL) {
            errMsg("Execute %s Failed", command);
            goto exe_error;
    }

    char *str = (char *)malloc(MAX_STRING_LENGTH);

    while(fgets(str, MAX_STRING_LENGTH, fp) != NULL ) {
            ret = sscanf(str, "%s %x %s", dir_name, &hash, branch);
            if (strstr(branch, branch_name))
            {
                find = true;
                break;
            }
    }

    if(stat(dir_name, &file_stat) == -1) {
            errMsg("No worktree list");
            goto str_err;
    }
        
    char *dname = strdup(dir_name);
    char *brname = strdup(branch_name);

    if (delete) {
            if (!find)
            {
                errMsg("%s was not found in %s\n", branch_name, dname);
                goto dup_err;
            }
            snprintf(command, MAX_COMMAND_LENGTH, "git worktree remove --force %s", dname);
            ret = system(command);
    } else {
            if (find)
            {
                errMsg("%s already in %s\n", branch_name, dname);
                goto dup_err;
            }
            char *dir = dirname(dname);
            char *br = basename(brname);
            snprintf(dir_name, MAX_BRANCH_LENGTH, "%s/%s", dir, br);
            ret = mkdir(dir_name, 0777);
            snprintf(command, MAX_COMMAND_LENGTH, "git worktree add %s %s", dir_name, branch_name);
            ret = system(command);
            snprintf(command, MAX_COMMAND_LENGTH, "cp /home/shuhaoyang/Desktop/work-tree/git_command.sh %s", dir_name);
            ret = system(command);
    }

    if (ret) {
            errMsg("COMMAND EXECUTE ERROR");
            goto dup_err;
    } else {
            fprintf(stdout, "%s %s success\n", delete ? "Delete" : "Add", branch_name);
    }

    return true;

dup_err:
    free(dname);
    free(brname);
str_err:
    free(str);
exe_error:
    pclose(fp);
    return false;
    }

    static void t_worktree_list(void)
    {
        FILE *fp;
        int ret;
        unsigned long hash;
        char branch[MAX_BRANCH_LENGTH];
        char dirname[MAX_BRANCH_LENGTH];

        fp = popen("git worktree list", "r");
        if (fp == NULL)
        {
            errExit("Execute list failed");
            return;
        }
        char *string = (char *)malloc(MAX_STRING_LENGTH);
        fprintf(stdout, "Current worktree list:\n");
        while (fgets(string, MAX_STRING_LENGTH, fp) != NULL)
        {
            ret = sscanf(string, "%s %lx %s", dirname, &hash, branch);
            /*
             * git worktree list format is like
             * directory commit_hash branch
             * so return value should be 3
             */
            if (ret == 3)
            {
                char *basenm = basename(dirname);
                fprintf(stdout, "dir:%-30s node:%-15lx branch:%s\n", basenm, hash, branch);
            }
        }

        free(string);
        pclose(fp);
    }

    static bool t_worktree_exist(const char *branch_name)
    {

        if (!t_worktree_check(branch_name, false))
        {
            errExit("%s in not in remote", branch_name);
            return false;
        }

        FILE *fp;
        int ret;
        unsigned long hash;
        bool exist = false;
        char branch[MAX_BRANCH_LENGTH];
        char dir_name[MAX_BRANCH_LENGTH];

        fp = popen("git worktree list", "r");
        if (fp == NULL)
        {
            errExit("Execute list failed");
            goto execute_fail;
        }
        char *string = (char *)malloc(MAX_STRING_LENGTH);
        if (string == NULL)
        {
            errExit("Malloc string failed");
            goto malloc_fail;
        }
        while (fgets(string, MAX_STRING_LENGTH, fp) != NULL)
        {
            ret = sscanf(string, "%s %lx %s", dir_name, &hash, branch);
            if (ret == 3 && strstr(branch, branch_name))
            {
                exist = true;
                break;
            }
        }

        fprintf(stdout, "%s is %s current worktree list\n", branch_name, exist ? "in" : "not in");

        free(string);
    malloc_fail:
        pclose(fp);
    execute_fail:
        return exist;
    }

    int main(int argc, char *argv[])
    {
        int opt, ret;
        while ((opt = getopt_long(argc, argv, "a:r:hle:c:", long_opts, NULL)) != -1)
        {
            switch (opt)
            {
            case 'a':
                t_worktree_handle(optarg, false);
                break;
            case 'r':
                t_worktree_handle(optarg, true);
                break;
            case 'l':
                t_worktree_list();
                break;
            case 'e':
                t_worktree_exist(optarg);
                break;
            case 'c':
                t_worktree_check(optarg, true);
                return 0;
            case 'h':
            case '?':
                t_worktree_usgae(argv[0]);
                break;
            }
        }

        return 0;
    }