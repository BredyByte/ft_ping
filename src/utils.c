# include "defines.h"
# include <stdio.h>
# include <ctype.h>
# include <stdlib.h>
# include <unistd.h>
# include <time.h>

void	sleep_microseconds(long microseconds)
{
    struct timespec	ts;
    ts.tv_sec = microseconds / 1000000;
    ts.tv_nsec = (microseconds % 1000000) * 1000;
    nanosleep(&ts, NULL);
}

void	free_allocations(void)
{
	if (g_data.dest_host)
		free(g_data.dest_host);

	if (g_data.stats.rtt_values)
		free(g_data.stats.rtt_values);

	close(g_data.sock);
}

void	exit_failure(const char *message)
{
	if (message)
		fprintf(stderr, "%s\n", message);

	free_allocations();

	exit(EXIT_FAILURE);
}

int	hex_serializer(void)
{
	char	*pattern_ptr = g_data.f_args.pattern;
	char	*write_ptr = g_data.f_args.pattern;

	while (*pattern_ptr != '\0')
	{
		if (*pattern_ptr == '0' && *(pattern_ptr + 1) == 'x')
        {
            *write_ptr++ = '0';
            *write_ptr++ = '0';
            pattern_ptr += 2;
        }
		else
		{
			if (isxdigit(*pattern_ptr) == 0)
			{
				fprintf(stderr, "ping: error in pattern near %s\n", pattern_ptr);
				return -1;
			}

			*write_ptr++ = *pattern_ptr++;
		}
	}

	*write_ptr = '\0';

	return 0;
}

void	print_help(void)
{
    printf("Usage: ft_ping [OPTION...] HOST ...\n");
	printf("Send ICMP ECHO_REQUEST packets to network hosts.\n");
	printf("\n");
	printf(" Options valid for all request types:\n");
	printf("\n");
	printf("  -c, --count=NUMBER\t\tstop after sending NUMBER packets\n");
	printf("  -v, --verbose\t\t\tverbose output\n");
	printf("\n");
	printf(" Options valid for --echo requests:\n");
	printf("\n");
	printf("  -p, --pattern=PATTERN\t\tfill ICMP packet with given pattern (hex)\n");
	printf("  -q, --quiet\t\t\tquiet output\n");
	printf("  -?, --help\t\t\tgive this help list\n");
	printf("      --usage\t\t\tgive a short usage message\n");
	printf("  -V, --version\t\t\tprint program version\n");
	printf("Mandatory or optional arguments to long options are also mandatory or optional\nfor any corresponding short options.\n");
	printf("\n");
	printf("Options marked with (root only) are available only to superuser.\n");
	printf("\n");
	printf("Report bugs to <bug-is-yourlife@foo.org>.\n");
}

void	print_usage(void)
{
	printf("Usage: ft_ping\t[-vq?V] [-c NUMBER] [-p PATTERN] [--count=NUMBER]\n");
	printf("\t\t");
	printf("[--verbose] [--pattern=PATTERN] [--quiet] [--help]\n");
	printf("\t\t");
	printf("[--usage] [--version]\n");
	printf("\t\t");
	printf("HOST ...\n");
}

void	print_version(void)
{
	printf("ft_ping (42 École project) 1.0\n");
	printf("Copyright (C) 2024 Free Software Foundation, Inc.\n");
	printf("This is free software: you are free to change and redistribute it.\n");
	printf("There is NO WARRANTY, to the extent permitted by law.\n\n");
	printf("Written by Davyd Bredykhin.\n");
}

void	print_args(void)
{
	char	dipinput[INET_ADDRSTRLEN];
	char	sipinput[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(g_data.dest_ip.sin_addr), dipinput, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &(g_data.source_ip.sin_addr), sipinput, INET_ADDRSTRLEN);

	printf("Program's parsed arguments:\n");
	printf("	Dest host\t%s\n", g_data.dest_host);
	printf("	Dest IP\t\t%s\n", dipinput);
	printf("	Source IP\t%s\n", sipinput);
	printf("\n");
	printf("	Verbose:\t%s\n", g_data.f_args.v_flag ? "True" : "False");
	printf("	Quiet:\t\t%s\n", g_data.f_args.q_flag ? "True" : "False");
	printf("	Count:\t\t%d\n", g_data.f_args.count);
	printf("	Pattern:\t%s\n", g_data.f_args.pattern[0] == '\0' ? "Empty" : g_data.f_args.pattern);
}
