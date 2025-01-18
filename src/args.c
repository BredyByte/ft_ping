# include "ip_resolve.h"
# include "defines.h"
# include "utils.h"
# include <bits/getopt_core.h>
# include <getopt.h>
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <ctype.h>
# include <limits.h>
# include <unistd.h>

static struct option long_options[] =
{
    {"verbose", no_argument, NULL, 'v'},      	// --verbose -> 'v'
    {"quiet", no_argument, NULL, 'q'},        	// --quiet -> 'q'
    {"help", no_argument, NULL, 0},         	// --help -> -
    {"version", no_argument, NULL, 'V'},      	// --version -> 'V'
    {"count", required_argument, NULL, 'c'},  	// --count=NUMBER -> 'c'
    {"pattern", required_argument, NULL, 'p'},  // --pattern=PATTERN -> 'p'
    {"usage", no_argument, NULL, 0},        	// --usage -> -
    {0, 0, 0, 0}
};

static void defs_global_strust(void)
{
    memset(&g_data, 0, sizeof(t_ping));
    memset(&g_data.dest_host, 0, sizeof(uint8_t));
    g_data.dest_host = NULL;
    g_data.sequence = 0;
    g_data.icmp_id = getpid();
    g_data.sock = 0;

    g_data.f_args.v_flag = false;
	g_data.f_args.q_flag = false;
	g_data.f_args.count = -1;
	g_data.f_args.pattern[0] = '\0';

    g_data.stats.packets_transmitted = 0;
    g_data.stats.packets_received = 0;
    g_data.stats.rtt_values = NULL;
    g_data.stats.rtt_count = 0;
    g_data.stats.rtt_min = 0;
    g_data.stats.rtt_max = 0;
    g_data.stats.rtt_avg = 0;
    g_data.stats.rtt_stddev = 0;
}

static void handle_quest_mark(int argc, char **argv)
{
    size_t  len;
    char    *current_arg;

    if (argc < 2)
        return;

    for (int i = 1; i < argc; i++)
    {
        current_arg = argv[i];
        if (current_arg == NULL)
            return;

        len = strlen(current_arg);
        if (len != 2)
            continue;

        for (size_t j = 0; j < len - 1; j++)
        {
            if (current_arg[j] == '-' && current_arg[j + 1] == '?')
            {
                print_help();
                exit(EXIT_SUCCESS);
            }
        }
    }

}

static int  is_valid_int(const char *str)
{
    int     res = 0;
	int     sign = 1;
    char    *ptr = (char *)str;

   while (isspace(*ptr))
		ptr++;

	if (*ptr == '-')
    {
		sign = -1;
        ptr++;
    }

	while (*ptr != '\0')
    {
        if (*ptr >= '0' && *ptr <= '9')
            res = res * 10 + (*ptr++ - '0');
        else
        {
            fprintf(stderr, "ft_ping: invalid value (`%s' near `%s')\n", str, ptr);
            exit(EXIT_FAILURE);
        }
    }
	return (res * sign);
}

static void args_options(int argc, char **argv)
{
	int opt;
	int long_index = 0;

	while ((opt = getopt_long(argc, argv, "vfqVc:i:w:W:p:", long_options, &long_index)) != -1)
    {
        switch (opt)
        {
            case 'v':
                g_data.f_args.v_flag = true;
                break;
            case 'q':
                g_data.f_args.q_flag = true;
                break;
            case '?':
                fprintf(stderr, "Try \'./ft_ping --help\' or \'./ft_ping --usage\' for more information.\n");
                exit(EXIT_FAILURE);
            case 'V':
				print_version();
				exit(EXIT_SUCCESS);
            case 'c':
                g_data.f_args.count = is_valid_int(optarg);
                if (g_data.f_args.count <= 0 )
                    g_data.f_args.count = -1;
                break;
            case 'p':
                strncpy(g_data.f_args.pattern, optarg, sizeof(g_data.f_args.pattern) - 1);
                g_data.f_args.pattern[sizeof(g_data.f_args.pattern) - 1] = '\0';
				if (hex_serializer() != 0)
					exit(EXIT_FAILURE);
                break;
			case 0:
                if (strcmp("usage", long_options[long_index].name) == 0)
                {
					print_usage();
					exit(EXIT_SUCCESS);
				}
                else if (strcmp ("help", long_options[long_index].name) == 0)
                {
                    print_help();
                    exit(EXIT_SUCCESS);
                }
                break;
            default:
                fprintf(stderr, "ft_ping: unknown option encountered.\n");
                exit(EXIT_FAILURE);
        }
    }

	if (optind >= argc)
    {
        fprintf(stderr, "ft_ping: missing host operand\n");
        fprintf(stderr, "Try \'./ft_ping --help\' or \' ./ft_ping --usage\' for more information.\n");
        exit(EXIT_FAILURE);
    }
}

void    args(int argc, char **argv)
{
    handle_quest_mark(argc, argv);
	defs_global_strust();
	args_options(argc, argv);
    ip_resolve_and_validate(argv[optind]);
}

