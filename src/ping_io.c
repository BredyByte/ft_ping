# include "defines.h"
# include <netinet/ip_icmp.h>	// Def. struct for ICMP packet header
# include <netinet/ip.h>		// Def. struct for IP packet header
# include <sys/socket.h>
# include <sys/types.h>
# include <stdio.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <time.h>
# include <err.h>

static unsigned short   checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;

    // Add up 16-bit words one by one
    for (; len > 1; len -= 2) {
        sum += *buf++;
	}

    // If there's a leftover byte, add it as a 16-bit word (padding with 0)
    if (len == 1)
        sum += *(unsigned char *)buf;

    // Fold the sum to fit into 16 bits by adding the carry bits to the lower 16 bits
    sum = (sum >> 16) + (sum & 0xFFFF); // Add higher 16 bits to lower 16 bits
    sum += (sum >> 16);                 // Add carry if any remains
    return ~sum;
}

static void fill_icmp_timestamp(void *buffer)
{
	if (gettimeofday(buffer, NULL) == -1)
		warn("failed getting time of day");
}

static void fill_icmp_data(unsigned char *buffer, size_t size)
{
    memset(buffer, 0x42, size);
}

static void prep_iphdr(void *packet, struct iphdr *iph)
{
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = htons(IP_HDR_SIZE + ICMP_HDR_SIZE + ICMP_PAYLOAD_SIZE);
    iph->id = htons(54321);
    iph->frag_off = 0x40;
    iph->ttl = 64;
    iph->protocol = IPPROTO_ICMP;
    iph->saddr = g_data.source_ip.sin_addr.s_addr;
    iph->daddr = g_data.dest_ip.sin_addr.s_addr;
    iph->check = checksum((unsigned short *)packet, IP_HDR_SIZE);
}

static void prep_icmphdr(void *packet, struct icmphdr *icmph)
{
    unsigned char *ptr = (unsigned char *)packet;

    icmph->type = ICMP_ECHO;
    icmph->code = 0;
    icmph->un.echo.id = htons(1234);
    icmph->un.echo.sequence = htons(0);
    icmph->checksum = 0;

    fill_icmp_data(ptr + IP_HDR_SIZE + ICMP_HDR_SIZE + ICMP_TIMESTAMP_SIZE, ICMP_PAYLOAD_SIZE);
    fill_icmp_timestamp(ptr + IP_HDR_SIZE + ICMP_HDR_SIZE);

    icmph->checksum = checksum((unsigned short *)icmph, ICMP_HDR_SIZE + ICMP_PAYLOAD_SIZE);
}

static void sock_create(int *sock)
{
    int opt = 1;

	if ((*sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
    {
        perror("socket");
		free(g_data.dest_host);
        exit(EXIT_FAILURE);
    }

	// Flag for telling the system that i will manage the IP header manually.
    if (setsockopt(*sock, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
}

void    init_ping(void)
{
    char packet[4096];
	struct iphdr *iph = (struct iphdr *)packet;
    struct icmphdr *icmph = (struct icmphdr *)(packet + sizeof(struct iphdr));
    struct sockaddr_in dest;

	memset(packet, 0, sizeof(packet));

    sock_create(&g_data.sock);

    prep_iphdr(packet, iph);

    prep_icmphdr(packet, icmph);

    // config dest addres
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = iph->daddr;

	if (sendto(g_data.sock, packet, ntohs(iph->tot_len), 0,
        (struct sockaddr *)&dest, sizeof(dest)) < 0)
    {
        perror("sendto");
        close(g_data.sock);
		free(g_data.dest_host);
		exit(EXIT_FAILURE);
    }

	printf("ICMP Echo Request sent to 8.8.8.8\n");

	close(g_data.sock);
}
