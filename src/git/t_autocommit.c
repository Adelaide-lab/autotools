#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <libgen.h>
#include <sys/select.h>
#include <signal.h>
#include <setjmp.h>
#include <errno.h>
#include "tlpi_hdr.h"

#define MAX_TOKEN_LENGTH 16
#define MAX_LABEL_LENGTH 32
#define MAX_BRANCH_LENGTH 128
#define SHORT_STRING_LENGTH 256
#define MAX_COMMAND_LENGTH 1024
#define MAX_STRING_LENGTH 1024

#define INPUT_MSG_TIMEOUT 4

enum
{
    UX,
    AM,
    LEA,
    STACK,
    MAX
};
typedef struct
{
    int msg_type;
    char *msg_name;
} commit_msg_t;

commit_msg_t commit_msg_lables[] = {
    COMMIT_MSG_INFO(UX),
    COMMIT_MSG_INFO(AM),
    COMMIT_MSG_INFO(LEA),
    COMMIT_MSG_INFO(STACK),
};

static unsigned short int msg_cnt[4];

static char tokens[MAX_TOKEN_LENGTH];
static char label[MAX_LABEL_LENGTH];
static char command[MAX_COMMAND_LENGTH];
static char commit_branch[MAX_BRANCH_LENGTH];
static char short_string[SHORT_STRING_LENGTH];
static char common_string[MAX_STRING_LENGTH];

static struct option long_opts[] = {
    {"debug", required_argument, NULL, 'd'},
    {"message", required_argument, NULL, 'm'},
    {"push", required_argument, NULL, 'p'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}};

static void t_autocommit_usgae(const char *filename)
{
    printf("Usage: %s [option] [parameter]\n", filename);
    printf("[option] is one of the followings:\n");
    printf("  -m, --message add commit message\n");
    printf("  -d, --debug add debug link id\n");
    printf("  -p, --push push to remote [yes/no]\n"
           "                     default no\n");
    printf("  -h, --help print this information\n");
    exit(EXIT_FAILURE);
}

static bool t_git_current_branch_revise(void)
{
    FILE *fp;
    int ret;

    fp = popen("git rev-parse --abbrev-ref HEAD", "r");
    if (fp == NULL)
    {
        perror("Get current branch failed");
        return false;
    }

    while (fgets(commit_branch, MAX_BRANCH_LENGTH, fp) != NULL)
    {
        if (strstr(commit_branch, "fatal"))
        {
            return false;
        }
    }

    pclose(fp);
    return true;
}

static bool t_git_add_changed_file(char *file)
{
    char *env;
    int ret;

    for (int i = 0; i < ARRAY_SIZE(commit_msg_lables); i++)
    {
        env = getenv(commit_msg_lables[i].msg_name);
        if (env != NULL)
        {
            char *token = strtok(env, ":");
            while (token != NULL)
            {
                if (strstr(file, token))
                {
                    msg_cnt[i]++;
                    sprintf(command, "git add %s", file);
                    ret = system(command);
                    return true;
                }
                token = strtok(NULL, ":");
            }
        }
    }

    return false;
}

static bool t_git_get_file_modification()
{
    FILE *fp;
    bool ret;

    fp = popen("git status -s", "r");
    if (fp == NULL)
    {
        perror("Get current branch failed");
        return false;
    }

    while (fgets(common_string, MAX_BRANCH_LENGTH, fp) != NULL)
    {
        sscanf(common_string, "%s %s", tokens, short_string);
        if (strchr(tokens, 'M') || strchr(tokens, 'D') || strchr(tokens, 'A'))
        {
            ret = t_git_add_changed_file(short_string);
        }
    }

    pclose(fp);
    return ret;
}

volatile int msg_info_timeout_flag;
sigjmp_buf timeout_jump;

void t_git_get_msg_info_timeout(int signum)
{
    msg_info_timeout_flag = 1;
    siglongjmp(timeout_jump, 1);
}

static char *t_git_get_commit_message_info(unsigned int time_out)
{
    struct sigaction sa = {
        .sa_handler = t_git_get_msg_info_timeout};
    sigset_t set;

    if (sigaction(SIGALRM, &sa, NULL) == -1)
    {
        errMsg("sigaction error");
        goto sigaction_error;
    }

    sigemptyset(&set);
    sigaddset(&set, SIGALRM);

    if (sigprocmask(SIG_UNBLOCK, &set, NULL) == -1)
    {
        errMsg("sigprocmask SIG_UNBLOCK error");
        goto sigaction_error;
    }

    alarm(time_out);

    if (sigsetjmp(timeout_jump, 1) == 0)
    {
        if (fgets(common_string, MAX_STRING_LENGTH, stdin) != NULL)
        {
            alarm(0);
            common_string[strcspn(common_string, "\n")] = 0;
            if (sigprocmask(SIG_BLOCK, &set, NULL) == -1)
            {
                errMsg("sigprocmask SIG_BLOCK error");
                goto sigaction_error;
            }
            return strdup(common_string);
        }
        else if (errno == EINTR && msg_info_timeout_flag)
        {
            msg_info_timeout_flag = 0;
        }
    }

    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1)
    {
        errMsg("sigprocmask SIG_BLOCK error");
    }

sigaction_error:
    return NULL;
}

static bool t_git_commit_modified_message(const char *commit_msg, const char *commit_bug_id, bool push)
{
    int ret;
    char tmp_tokens[MAX_TOKEN_LENGTH];
    char *commit_msg_time;
    time_t t;

    memset(label, '\0', MAX_LABEL_LENGTH);

    /*Get current calendar time*/
    t = time(NULL);
    commit_msg_time = ctime(&t);

    for (int i = 0; i < ARRAY_SIZE(commit_msg_lables); i++)
    {
        if (msg_cnt[i])
        {
            sprintf(tmp_tokens, "[%s]", commit_msg_lables[i].msg_name);
            strncat(label, tmp_tokens, MAX_BRANCH_LENGTH - strlen(tmp_tokens) - 1);
        }
    }

    snprintf(common_string, MAX_STRING_LENGTH, "%s %s\n\n[Commit-time]%s[Bug Id] %s\n[Branch] %s[Build] Ok \n[Test Result] w4-feedback\n", label, commit_msg ? commit_msg : "None",
             commit_msg_time, commit_bug_id ? commit_bug_id : "None", commit_branch);
    snprintf(command, MAX_COMMAND_LENGTH, "git commit -m \"%s\"", common__string);
    ret = system(command);
    if (push)
    {
        snprintf(command, MAX_COMMAND_LENGTH, "git push origin HEAD:refs/for/%s", commit_branch);
        ret = system(command);
    }

    return ret ? false : true;
}

int main(int argc, char *argv[])
{
    char *commit_msg = NULL;
    char *commit_bug_id = NULL;
    int opt, ret;
    bool push = false;

    if (!t_git_current_branch_revise())
    {
        errExit("Current dir not a git repository");
    }

    if (!t_git_get_file_modification())
    {
        errExit("No file modificated");
    }

    while ((opt = getopt_long(argc, argv, "m:d:hp:", long_opts, NULL)) != -1)
    {
        switch (opt)
        {
        case 'm':
            commit_msg = strdup(optarg);
            break;
        case 'd':
            commit_bug_id = strdup(optarg);
            break;
        case 'p':
            if (!strcmp(optarg, "yes"))
            {
                push = true;
                break;
            }
            else if (!strcmp(optarg, "no"))
            {
                push = false;
                break;
            }
        case 'h':
        case '?':
            t_autocommit_usgae(argv[0]);
            return -1;
        }
    }

    if (!commit_msg)
    {
        printf("Input your commit message:\n");
        commit_msg = t_git_get_commit_message_info(INPUT_MSG_TIMEOUT);
    }

    if (!commit_bug_id)
    {
        printf("Input your commit bug id:\n");
        commit_bug_id = t_git_get_commit_message_info(INPUT_MSG_TIMEOUT);
    }

    t_git_commit_modified_message(commit_msg, commit_bug_id, push);

    if (commit_msg)
    {
        free(commit_msg);
    }

    if (commit_bug_id)
    {
        free(commit_bug_id);
    }

    return 0;
}