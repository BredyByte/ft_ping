# include "defines.h"
# include "utils.h"
# include "ping_stats.h"
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
    unsigned short  *buf = b;
    unsigned int    sum = 0;

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
		exit_failure(NULL);
    }

	// Flag for telling the system that i will manage the IP header manually.
    if (setsockopt(*sock, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit_failure(NULL);
    }
}

void store_rtt(double rtt)
{
    double              *ptr_new;

    g_data.stats.packets_received++;

    if (g_data.stats.rtt_count == 0)
    {
        g_data.stats.rtt_values = (double *)malloc(RTT_RESERVE_BLOCK_SIZE * sizeof(double));
        if (g_data.stats.rtt_values == NULL)
            exit_failure("Memory allocation failed");

        g_data.stats.rtt_capacity = RTT_RESERVE_BLOCK_SIZE;
    }

    // Check if more memory is needed
    if (g_data.stats.rtt_count >= g_data.stats.rtt_capacity)
    {
        ptr_new = (double *)realloc(g_data.stats.rtt_values,
                            (g_data.stats.rtt_capacity + RTT_RESERVE_BLOCK_SIZE) * sizeof(double));
        if (ptr_new == NULL)
            exit_failure("Memory allocation failed");

        g_data.stats.rtt_values = ptr_new;
        g_data.stats.rtt_capacity += RTT_RESERVE_BLOCK_SIZE;
    }

    g_data.stats.rtt_values[g_data.stats.rtt_count] = rtt;
    g_data.stats.rtt_count++;
}

static void recv_icmp_response(int sock)
{
    char                buffer[4096];
    struct sockaddr_in  sender;
    socklen_t           sender_len = sizeof(sender);
    ssize_t             bytes_received;

    struct iphdr        *iph;
    int                 iph_len;
    struct icmphdr      *icmph;
    struct timeval      send_time, recv_time;
    double              rtt;

    bytes_received = recvfrom(sock, buffer, sizeof(buffer), 0,
                    (struct sockaddr *)&sender, &sender_len);

    if (bytes_received < 0)
    {
        perror("recvfrom");
        return;
    }

    gettimeofday(&recv_time, NULL);

    iph = (struct iphdr *)buffer;
    iph_len = iph->ihl * 4;

    icmph = (struct icmphdr *)(buffer + iph_len);

    if (icmph->type == ICMP_ECHOREPLY)
    {

        memcpy(&send_time, buffer + iph_len + sizeof(struct icmphdr), sizeof(struct timeval));

        rtt = (recv_time.tv_sec - send_time.tv_sec) * 1000.0;
        rtt += (recv_time.tv_usec - send_time.tv_usec) / 1000.0;

        store_rtt(rtt);

        printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
               inet_ntoa(sender.sin_addr),
               ntohs(icmph->un.echo.sequence),
               iph->ttl,
               rtt);
    }
    else
    {
        printf("Received non-echo reply ICMP packet (type=%d, code=%d)\n",
               icmph->type, icmph->code);
    }
}

void send_icmp_request(char *packet, uint16_t iph_totallen, struct sockaddr_in dest)
{
    if (sendto(g_data.sock, packet, ntohs(iph_totallen), 0,
        (struct sockaddr *)&dest, sizeof(dest)) < 0)
    {
        perror("sendto");
        exit_failure(NULL);
    }

    g_data.stats.packets_transmitted++;
}

void    init_ping(void)
{
    char                packet[4096];
	struct iphdr        *iph = (struct iphdr *)packet;
    struct icmphdr      *icmph = (struct icmphdr *)(packet + sizeof(struct iphdr));
    struct sockaddr_in  dest;

	memset(packet, 0, sizeof(packet));

    sock_create(&g_data.sock);

    prep_iphdr(packet, iph);

    // config dest addres
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = iph->daddr;

    prep_icmphdr(packet, icmph);

    printf("PING %s (%s): 56 data bytes\n", g_data.dest_host, inet_ntoa(g_data.dest_ip.sin_addr));

    // Send ping to destination
    send_icmp_request(packet, iph->tot_len, dest);

    // Waiting for response
    recv_icmp_response(g_data.sock);

    print_stats();
}
