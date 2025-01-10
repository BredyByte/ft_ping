#include "globals.h"
#include "icmp.h"
#include <netinet/ip_icmp.h>	// Def. struct for ICMP packet header
#include <netinet/ip.h>			// Def. struct for IP packet header
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;

    // Add up 16-bit words one by one
    for (; len > 1; len -= 2) {
        sum += *buf++;
	}
	/*
	Ex: We have this data 0x1234  0x5678  0x9ABC
	First itter:
		*buf = 0x1234
		sum += 0x1234 -> sum = 0x1234

	Second itter:#include <netinet/in.h>

		sum += 0x9ABC -> sum = 0x68AC + 0x9ABC = 0x10468
	*/

    // If there's a leftover byte, add it as a 16-bit word (padding with 0)
    if (len == 1)
        sum += *(unsigned char *)buf;

    // Fold the sum to fit into 16 bits by adding the carry bits to the lower 16 bits
    sum = (sum >> 16) + (sum & 0xFFFF); // Add higher 16 bits to lower 16 bits
	/*
	Ex:
	sum >> 16 - Extracting the higher 16 bits of the sum
		if sum = 0x123456, so sum >> 16 = 0x12.
		in  binary format:	sum = 0x123456 =	0001 0010 0011 0100 0101 0110
		result:				sum >> 16 = 		0000 0000 0001 0010

	sum & 0xFFFF - Extracting the lower 16 bits of the sum
	if sum = 0x123456, so sum & 0xFFFF = 0x3456
	in binary format:	sum =		0001 0010 0011 0100 0101 0110
	mask:				0xFFFF =	1111 1111 1111 1111
	res =							0000 0000 0011 0100 0101 0110

	0x12 + 0x3456= 0x3468.
	*/

    sum += (sum >> 16);                 // Add carry if any remains

    // Invert the bits to finalize the checksum
	/*
	Ex:
	if	sum = 0x123456 =	0001 0010 0011 0100 0101 0110
		res = 				1110 1101 1100 1011 1010 1001
	*/
    return ~sum;
}

void start_ping(void) {
	int sock;
    char packet[4096];
	struct iphdr *iph = (struct iphdr *)packet;
    struct icmphdr *icmph = (struct icmphdr *)(packet + sizeof(struct iphdr));
    struct sockaddr_in dest;
	int opt = 1;

	if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        perror("socket");
		free(global_data.dest_host);
        exit(EXIT_FAILURE);
    }


	// Flag for telling the system that i will manage the IP header manually.
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }


	// Fill IP header
	memset(packet, 0, sizeof(packet));
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = htons(sizeof(struct iphdr) + sizeof(struct icmphdr));
    iph->id = htons(54321);
    iph->frag_off = 0x40;
    iph->ttl = 64;
    iph->protocol = IPPROTO_ICMP;
    iph->saddr = inet_addr("10.12.250.207");
    iph->daddr = inet_addr("8.8.8.8");

    // checksum for ip header (not necessarily, the system can calculate it itself.)
    iph->check = checksum((unsigned short *)packet, sizeof(struct iphdr));

	// Fill ICMP header
	icmph->type = ICMP_ECHO;
    icmph->code = 0;
    icmph->un.echo.id = htons(1234);
    icmph->un.echo.sequence = htons(1);
    icmph->checksum = 0;
    icmph->checksum = checksum((unsigned short *)icmph, sizeof(struct icmphdr));

	// config dest addres
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = iph->daddr;

	 if (sendto(sock, packet, ntohs(iph->tot_len), 0, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
        perror("sendto");
        close(sock);
		free(global_data.dest_host);
		exit(EXIT_FAILURE);
    }

	printf("ICMP Echo Request sent to 8.8.8.8\n");

	close(sock);
}
