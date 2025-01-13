# include "defines.h"
# include <stdio.h>
# include <ctype.h>
# include <stdlib.h>
# include <unistd.h>

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
	else
		fprintf(stderr, "Falat error");

	free_allocations();

	exit(EXIT_FAILURE);
}

int	is_valid_hex(void)
{
	char	*pattern_ptr = g_data.f_args.pattern;

	while (*pattern_ptr != '\0')
	{
		if (isxdigit(*pattern_ptr) == 0)
		{
			fprintf(stderr, "ping: error in pattern near %s", pattern_ptr);
			return -1;
		}
		pattern_ptr++;
	}

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
	printf("  -i, --interval=NUMBER\t\twait NUMBER seconds between sending each packet\n");
	printf("      --ttl=N\t\t\tspecify N as time-to-live\n");
	printf("  -v, --verbose\t\t\tverbose output\n");
	printf("  -w, --timeout=N\t\tstop after N seconds\n");
	printf("  -W, --linger=N\t\tnumber of seconds to wait for response\n");
	printf("\n");
	printf(" Options valid for --echo requests:\n");
	printf("\n");
	printf("  -f, --flood\t\t\tflood ping (root only)\n");
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
	printf("Usage: ft_ping\t[-vfq?V] [-c NUMBER] [-i NUMBER] [-w N] [-W N] [-p PATTERN]\n");
	printf("\t\t");
	printf("[--count=NUMBER] [--interval=NUMBER] [--ttl=N] [--verbose]\n");
	printf("\t\t");
	printf("[--timeout=N] [--linger=N] [--flood] [--pattern=PATTERN]\n");
	printf("\t\t");
	printf("[--quiet] [--help] [--usage] [--version]\n");
	printf("\t\t");
	printf("HOST ...");
}

void	print_version(void)
{
	printf("ft_ping (42 École project) 1.0\n");
	printf("Copyright (C) 2024 Free Software Foundation, Inc.\n");
	printf("This is free software: you are free to change and redistribute it.\n");
	printf("There is NO WARRANTY, to the extent permitted by law.\n\n");
	printf("Written by Davyd Bredykhin.");
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
	printf("	Flood:\t\t%s\n", g_data.f_args.f_flag ? "True" : "False");
	printf("	Quiet:\t\t%s\n", g_data.f_args.q_flag ? "True" : "False");
	printf("	Count:\t\t%d\n", g_data.f_args.count);
	printf("	Interval:\t%d\n", g_data.f_args.interval);
	printf("	Timeout:\t%d\n", g_data.f_args.timeout);
	printf("	Linger:\t\t%d\n", g_data.f_args.linger);
	printf("	Pattern:\t%s", g_data.f_args.pattern[0] == '\0' ? "Empty" : g_data.f_args.pattern);
	printf("\n");
	printf("	ttl:\t\t%d\n", g_data.f_args.ttl);
}
