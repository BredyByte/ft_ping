#include "ping.h"

void print_help() {
    printf("Usage: ft_ping [OPTION...] HOST ...\n");
	printf("Send ICMP ECHO_REQUEST packets to network hosts.\n");
	printf("\n");
	printf(" Options valid for all request types:\n");
	printf("\n");
	printf("      --ttl=N\t\t\tspecify N as time-to-live\n");
	printf("  -v, --verbose\t\t\tverbose output\n");
	printf("  -w, --timeout=N\t\tstop after N seconds\n");
	printf("  -W, --linger=N\t\tnumber of seconds to wait for response\n");
	printf("\n");
	printf(" Options valid for --echo requests:\n");
	printf("\n");
	printf("  -f, --flood\t\t\tflood ping (root only)\n");
	printf("  -l, --preload=NUMBER\t\tsend NUMBER packets as fast as possible before \n\t\t\t\tfalling into normal mode of behavior (root only)\n");
	printf("  -p, --pattern=PATTERN\t\tfill ICMP packet with given pattern (hex)\n");
	printf("  -?, --help\t\t\tgive this help list\n");
	printf("      --usage\t\t\tgive a short usage message\n");
	printf("  -V, --version\t\t\tprint program version\n");
	printf("Mandatory or optional arguments to long options are also mandatory or optional\nfor any corresponding short options.\n");
	printf("\n");
	printf("Options marked with (root only) are available only to superuser.\n");
	printf("\n");
	printf("Report bugs to <bug-is-yourlife@foo.org>.");
}
