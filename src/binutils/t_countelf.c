#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>

#define STRING_SIZE 1024

static struct option long_opts[] = {
    {"Gtext", no_argument, NULL, 'T'},
    {"Ltext", no_argument, NULL, 't'},
    {"Gbss", no_argument, NULL, 'B'},
    {"Lbss", no_argument, NULL, 'b'},
    {"Grdonly", no_argument, NULL, 'R'},
    {"Lrdonly", no_argument, NULL, 'r'},
    {"Gdata", no_argument, NULL, 'D'},
    {"Ldata", no_argument, NULL, 'd'},
    {"Target", required_argument, NULL, 'i'},
    {"help", no_argument, NULL, '?'},
    {NULL, 0, NULL, 0}};

static void obj_usage(const char *progname)
{
    printf("Usage: %s [option] -i <object-file>\n", progname);
    printf("[option] is one of the following:\n");
    printf("  -t    List local.text into %-20s\n", "{file_name}_text.size");
    printf("  -T    List global.text into %-20s\n", "{file_name}_tex.size");
    printf("  -d    List local.data into %-20s\n", "{file_name}data.size");
    printf("  -D    List global.data into %-20s\n", "{file_name}data.size");
    printf("  -B    List global.bss into %-20s\n", "{file_name}_bss.size");
    printf("  -b    List local.bss into %-20s\n", "{file_name}_bss.size");
    printf("  -R    List global.rodata into %-20s\n", "{file_name}_rodata.size");
    printf("  -r    List local.rodata variables into %-20s\n", "{file_name}_rodata.size");
    printf("  -i    Specify the input file %-20s\n", "");
    printf("  -h    Show this help message %-20s\n", "");
}

static void obj_size_deport(const char *file_name, char opt, const char *path)
{
    char cmd[256] = "arm-none-eabi-nm -t d -S -s ";
    strncat(cmd, path, sizeof(cmd) - strlen(cmd) - 1);
    char op_file[256];
    strncpy(op_file, file_name, strlen(file_name) + 1);
    int n = sizeof(op_file) - strlen(op_file) - 1;
    switch (opt)
    {
    case 'T':
    case 't':
        strncat(op_file, "_text.size", n);
        break;
    case 'B':
    case 'b':
        strncat(op_file, "_bss.size", n);
        break;
    case 'R':
    case 'r':
        strncat(op_file, "_rodata.size", n);
        break;
    case 'D':
    case 'd':
        strncat(op_file, "_data.size", n);
        break;
    default:
        perror("Invalid input type\n");
        return;
    }
    FILE *fp_from = popen(cmd, "r");
    if (fp_from == NULL)
    {
        fprintf(stderr, "command not valid %s", cmd);
        return;
    }
    FILE *fp_to = fopen(op_file, "a+");
    if (fp_to == NULL)
    {
        fprintf(stderr, "open %s faled", op_file);
        pclose(fp_from);
        return;
    }

    char string[1025];
    int ret, size, location;
    int total = 0;
    char type;
    char *buf = (char *)malloc(STRING_SIZE);
    while (fgets(string, sizeof(string), fp_from) != NULL)
    {
        ret = sscanf(string, "%d %d %c %s", &location, &size, &type, buf);
        if (ret == 4)
        {
            total += size;
            sprintf(string, "Size:%-5d Type:%-2c Symbol:%s\r\n", size, type, buf);
            fprintf(stdout, "%s", string);
            if (type == opt)
            {
                ret = fwrite(string, 1, strlen(string), fp_to);
                if (ret != strlen(string))
                {
                    fprintf(stderr, "write data error");
                    break;
                }
            }
        }
    }

    sprintf(string, "================================================================"
                    "\r\nTotal size %d for type '%c' in %s\r\n"
                    "================================================================\r\n",
            total, opt, path);
    ret = fwrite(string, 1, strlen(string), fp_to);
    free(buf);
    fclose(fp_to);
    pclose(fp_from);
    return;
}

int main(int argc, char *argv[])
{
    int opt, ret;
    char *target_file = NULL;
    char opts[8];
    int ind = 0;

    ret = system("rm -rf *.size");
    if (ret)
    {
        perror("remove file error");
    }

    while ((opt = getopt_long(argc, argv, "i:htTbBdDrR", long_opts, NULL)) != -1)
    {
        switch (opt)
        {
        case 'i':
            target_file = optarg;
            if (!strstr(target_file, ".a") && !strstr(target_file, ".o") && !strstr(target_file, ".elf"))
            {
                perror("Input file is not any kind of object, dynamic lib, or elf");
                target_file = NULL;
                return -1;
            }
            break;
        case 'T':
        case 't':
        case 'R':
        case 'r':
        case 'D':
        case 'd':
        case 'B':
        case 'b':
            opts[ind++] = opt;
            break;
        case ':':
            fprintf(stderr, "No input object file\n");
            return -1;
        case 'h':
        case '?':
            obj_usage(argv[0]);
            return -1;

            for (; optind < argc; optind++)
            {
                fprintf(stderr, "Non-option argument: %s\n", argv[optind]);
            }
        }
    }

    if (!target_file)
    {
        perror("No valid file to process\n");
        return -1;
    }

    char *dup_name;
    dup_name = strdup(target_file);
    const char *file_name = strtok(dup_name, ".");

    for (int i = ind - 1; i >= 0; i--)
    {
        obj_size_deport(file_name, opts[i], target_file);
    }

    free(dup_name);
    return 0;
}