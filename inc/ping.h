#ifndef FT_PING_H
# define FT_PING_H

#include <netinet/ip_icmp.h>	// Def. struct for ICMP packet header
#include <netinet/ip.h>			// Def. struct for IP packet header
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define INET4_LEN 4

typedef struct s_data {
	char	dest_ip_str[INET_ADDRSTRLEN];	// IPv4 argument in string
	uint8_t dest_ip_addr[INET4_LEN];		// IPv4-addr in binary format
	bool	v_flag;							// argument for verbose mode
	bool	h_flag;							// argument for usage message
} t_data;

extern t_data global_data;

// Output function
void print_help(void);


void simple_ping(void);

#endif
