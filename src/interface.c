#define _GNU_SOURCE
#include "globals.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <linux/if_link.h>
#include <netdb.h>

void check_available_interface(void) {
	struct ifaddrs *ifaddr;
	int s;
	char host[NI_MAXHOST];
	memset(&host, 0, NI_MAXHOST);

	if (getifaddrs(&ifaddr) == -1) {
		perror("getaddrinfo");
		exit(EXIT_FAILURE);
	}

	/* Walk through linked list, maintaining head pointer so we
		can free list later. */

	for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {

	/* Flag explication: AF_INET - for IPv4, IFF_UP - for
		check if is active and ready to work interface,
		IFF_LOOPBACK - exclude localhost. */

		if (ifa->ifa_addr != NULL
            && ifa->ifa_addr->sa_family == AF_INET
            && (ifa->ifa_flags & IFF_UP)
            && !(ifa->ifa_flags & IFF_LOOPBACK)
			&& (ifa->ifa_flags & IFF_BROADCAST)) {

			s = getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),
					host, NI_MAXHOST,
					NULL, 0, NI_NUMERICHOST);
			if (s != 0)
				continue;
		}
	}

	freeifaddrs(ifaddr);

	if (host[0] == 0) {
		fprintf(stderr, "Error: No suitable network interface found.\n");
		exit(EXIT_FAILURE);
	}
}

