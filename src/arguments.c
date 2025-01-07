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
    {"help", no_argument, NULL, 0},         	// --help -> -
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
    memset(&global_data, 0, sizeof(t_data));
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

void handle_question_mark_workaround(int argc, char **argv) {
    if (argc < 2)
        return;

    for (int i = 1; i < argc; i++) {
        char *current_arg = argv[i];
        if (current_arg == NULL)
            return;

        size_t len = strlen(current_arg);
        if (len != 2)
            continue;

        for (size_t j = 0; j < len - 1; j++) {
            if (current_arg[j] == '-' && current_arg[j + 1] == '?') {
                print_help();
                exit(0);
            }
        }
    }

}

void parse_arguments(int argc, char **argv) {
	int opt;
	int long_index = 0;

    handle_question_mark_workaround(argc, argv);

	get_def_vals_struct();

	while ((opt = getopt_long(argc, argv, "vfqVc:i:w:W:p:", long_options, &long_index)) != -1) {
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
                fprintf(stderr, "Try \'./ft_ping --help\' or \'./ft_ping --usage\' for more information.\n");
                exit(EXIT_FAILURE);
            case 'V':
				print_version();
				exit(EXIT_SUCCESS);
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
					exit(EXIT_FAILURE);
                break;
			case 0:
                if (strcmp("ttl", long_options[long_index].name) == 0) {
                    global_data.f_args.ttl = atoi(optarg);
                } else if (strcmp("usage", long_options[long_index].name) == 0) {
					print_usage();
					exit(EXIT_SUCCESS);
				} else if (strcmp ("help", long_options[long_index].name) == 0) {
                    print_help();
                    exit(EXIT_SUCCESS);
                }
                break;
            default:
                fprintf(stderr, "Unknown option encountered.\n");
                exit(EXIT_FAILURE);
        }
    }

	if (optind < argc) {
        printf("Host: %s\n", argv[optind]);
		strncpy(global_data.dest_ip_str, argv[optind], INET_ADDRSTRLEN - 1);
		global_data.dest_ip_str[INET_ADDRSTRLEN - 1] = '\0';
	} else {
		fprintf(stderr, "ft_ping: missing host operand\n");
        fprintf(stderr, "Try \'./ft_ping --help\' or \' ./ft_ping --usage\' for more information.\n");
        exit(EXIT_FAILURE);
	}
}

void check_args(int argc, char **argv) {
	parse_arguments(argc, argv);
}

