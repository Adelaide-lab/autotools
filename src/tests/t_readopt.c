#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

// 定义长选项
static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"version", no_argument, 0, 'v'},
    {"input", required_argument, 0, 'i'},
    {"output", required_argument, 0, 'o'},
    {"verbose", no_argument, 0, 'V'},
    {"debug", no_argument, 0, 'd'},
    {0, 0, 0, 0}};

static void usage(const char *progname)
{
    printf("Usage: %s [--help] [--version] [--input <file>] [--output <file>]\n", progname);
    printf("       [--verbose] [--debug]\n");
}

int main(int argc, char *argv[])
{
    int c;
    int option_index = 0;
    char *input_file = NULL;
    char *output_file = NULL;
    int verbose = 0;
    int debug = 0;

    while ((c = getopt_long(argc, argv, "hvi:o:Vd", long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 'h':
            usage(argv[0]);
            return 0;
        case 'v':
            printf("Version 1.0\n");
            return 0;
        case 'i':
            input_file = optarg;
            printf("Input file: %s\n", input_file);
            break;
        case 'o':
            output_file = optarg;
            printf("Output file: %s\n", output_file);
            break;
        case 'V':
            verbose = 1;
            printf("Verbose mode enabled\n");
            break;
        case 'd':
            debug = 1;
            printf("Debug mode enabled\n");
            break;
        case '?':
            usage(argv[0]);
            return 1;
        default:
            printf("Invalid option\n");
            usage(argv[0]);
            return 1;
        }
    }

    // 处理剩余的非选项参数
    for (; optind < argc; optind++)
    {
        printf("Non-option argument: %s\n", argv[optind]);
    }

    // 根据选项进行后续处理
    if (input_file && output_file)
    {
        if (verbose)
        {
            printf("Processing input file '%s' to output file '%s' in verbose mode\n", input_file, output_file);
        }
        else
        {
            printf("Processing input file '%s' to output file '%s'\n", input_file, output_file);
        }
        if (debug)
        {
            printf("Debugging enabled\n");
        }
    }

    return 0;
}