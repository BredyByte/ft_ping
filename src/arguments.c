#include "globals.h"
#include "utils.h"
#include <bits/getopt_core.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static struct option long_options[] = {
    {"verbose", no_argument, NULL, 'v'},      	// --verbose -> 'v'
    {"flood", no_argument, NULL, 'f'},        	// --flood -> 'f'
    {"quiet", no_argument, NULL, 'q'},        	// --quiet -> 'q'
    {"help", no_argument, NULL, '?'},         	// --help -> '?'
    {"version", no_argument, NULL, 'V'},      	// --version -> 'V'
    {"count", required_argument, NULL, 'c'},  	// --count=NUMBER -> 'c'
    {"interval", required_argument, NULL, 'i'}, // --interval=NUMBER -> 'i'
    {"timeout", required_argument, NULL, 'w'},  // --timeout=N -> 'w'
    {"linger", required_argument, NULL, 'W'},   // --linger=N -> 'W'
    {"pattern", required_argument, NULL, 'p'},  // --pattern=PATTERN -> 'p'
    {"ttl", required_argument, NULL, 0},        // --ttl=N -> -
    {"usage", no_argument, NULL, 0},        	// --usage -> -
    {0, 0, 0, 0}
};

void get_def_vals_struct(void) {
	global_data.f_args.v_flag = false;
	global_data.f_args.f_flag = false;
	global_data.f_args.q_flag = false;
	global_data.f_args.count = -1;
	global_data.f_args.interval = -1;
	global_data.f_args.timeout = -1;
	global_data.f_args.linger = -1;
	global_data.f_args.pattern[0] = '\0';
	global_data.f_args.ttl = -1;
}

int parse_arguments(int argc, char **argv) {
	(void) argc;
	(void) argv;

	int opt;
	int long_index = 0;

	get_def_vals_struct();

	while ((opt = getopt_long(argc, argv, "vfq?Vc:i:w:W:p:", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'v':
                global_data.f_args.v_flag = true;
                break;
            case 'f':
                global_data.f_args.f_flag = true;
                break;
            case 'q':
                global_data.f_args.q_flag = true;
                break;
            case '?':
                print_help();
				exit(0);
            case 'V':
				print_version();
				exit(0);
            case 'c':
                global_data.f_args.count = atoi(optarg);
                break;
            case 'i':
                global_data.f_args.interval = atoi(optarg);
                break;
            case 'w':
                global_data.f_args.timeout = atoi(optarg);
                break;
            case 'W':
                global_data.f_args.linger = atoi(optarg);
                break;
            case 'p':
                strncpy(global_data.f_args.pattern, optarg, sizeof(global_data.f_args.pattern) - 1);
                global_data.f_args.pattern[sizeof(global_data.f_args.pattern) - 1] = '\0';
				if (valid_hex() != 0)
					return -1;
                break;
			case 0:
                if (strcmp("ttl", long_options[long_index].name) == 0) {
                    global_data.f_args.ttl = atoi(optarg);
                } else if (strcmp("usage", long_options[long_index].name) == 0) {
					print_usage();
					exit(0);
				}
                break;
            default:
                fprintf(stderr, "Unknown option: -%c\n", opt);
                return -1;
        }
    }

	if (optind < argc) {
		strncpy(global_data.dest_ip_str, argv[optind], INET_ADDRSTRLEN - 1);
		global_data.dest_ip_str[INET_ADDRSTRLEN - 1] = '\0';
	} else {
		fprintf(stderr, "ft_ping: missing host operand\n");
		return -1;
	}

	return 0;
}

int check_args(int argc, char **argv) {
	memset(&global_data, 0, sizeof(t_data));
	(void) argc;
	(void) argv;

	if (parse_arguments(argc, argv) != 0)
		return -1;

	return 0;
}

