# include "defines.h"
# include "utils.h"
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
# include <errno.h>             // For handle recvfrom errors
# include <limits.h>
# include <pthread.h>

static unsigned short   checksum(void *b, int len)
{
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
    size_t          pattern_len = strlen(g_data.f_args.pattern);
    unsigned char   temp_buffer[40];
    size_t          temp_len = 0;
    unsigned int    byte;

    if (pattern_len == 0)
    {
        memset(buffer, 0x42, size);
        return;
    }

    for (size_t i = 0; i < pattern_len; i += 2)
    {
        if (sscanf(&g_data.f_args.pattern[i], "%2x", &byte) != 1)
        {
            fprintf(stderr, "ft_ping: error parsing pattern near '%s'\n", &g_data.f_args.pattern[i]);
            memset(buffer, 0x42, size);
            return;
        }
        temp_buffer[temp_len++] = (unsigned char)byte;

        if (i + 1 >= pattern_len)
            break;
    }

    for (size_t i = 0; i < size; i++)
        buffer[i] = temp_buffer[i % temp_len];
}

static uint16_t generate_packet_id(void)
{
    static uint16_t packet_id = 0;

    if (g_data.sequence == 0)
        packet_id = (uint16_t)rand();
    else
        packet_id++;

    return packet_id;
}

static void iphdr_dynamicdata_prep(void *packet, struct iphdr *iph)
{
    iph->id = htons(generate_packet_id());
    iph->check = 0;
    iph->check = checksum((unsigned short *)packet, IP_HDR_SIZE);
}

static void iphdr_staticdata_prep(struct iphdr *iph)
{
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = htons(IP_HDR_SIZE + ICMP_HDR_SIZE + ICMP_PAYLOAD_SIZE);
    iph->id = htons(0);
    iph->frag_off = 0x40;
    iph->ttl = 64;
    iph->protocol = IPPROTO_ICMP;
    iph->saddr = g_data.source_ip.sin_addr.s_addr;
    iph->daddr = g_data.dest_ip.sin_addr.s_addr;
    iph->check = 0;
}

static void icmphdr_dynamicdata_prep(void *packet, struct icmphdr *icmph)
{
    unsigned char   *ptr = (unsigned char *)packet;

    fill_icmp_timestamp(ptr + IP_HDR_SIZE + ICMP_HDR_SIZE);

    icmph->un.echo.sequence = htons(g_data.sequence++);
    icmph->checksum = 0;
    icmph->checksum = checksum((unsigned short *)icmph, ICMP_HDR_SIZE + ICMP_PAYLOAD_SIZE);
}

static void icmphdr_staticdata_prep(void *packet, struct icmphdr *icmph)
{
    unsigned char   *ptr = (unsigned char *)packet;

    icmph->type = ICMP_ECHO;
    icmph->code = 0;
    icmph->un.echo.id = htons(g_data.icmp_id);
    icmph->un.echo.sequence = htons(0);
    icmph->checksum = 0;

    fill_icmp_data(ptr + IP_HDR_SIZE + ICMP_HDR_SIZE + ICMP_TIMESTAMP_SIZE, ICMP_PAYLOAD_SIZE);
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

static void store_rtt(double rtt)
{
    double              *ptr_new;

    g_data.stats.packets_received++;

    if (g_data.stats.rtt_count == 0)
    {
        g_data.stats.rtt_values = (double *)malloc(RTT_RESERVE_BLOCK_SIZE * sizeof(double));
        if (g_data.stats.rtt_values == NULL)
            exit_failure("Error on allocation memory");

        g_data.stats.rtt_capacity = RTT_RESERVE_BLOCK_SIZE;
    }

    // Check if more memory is needed
    if (g_data.stats.rtt_count >= g_data.stats.rtt_capacity)
    {
        ptr_new = (double *)realloc(g_data.stats.rtt_values,
                            (g_data.stats.rtt_capacity + RTT_RESERVE_BLOCK_SIZE) * sizeof(double));
        if (ptr_new == NULL)
            exit_failure("Error on allocation memory");

        g_data.stats.rtt_values = ptr_new;
        g_data.stats.rtt_capacity += RTT_RESERVE_BLOCK_SIZE;
    }

    g_data.stats.rtt_values[g_data.stats.rtt_count] = rtt;
    g_data.stats.rtt_count++;
}

static void *recv_icmp_thread(void *sock_ptr)
{
    int                 sock = *((int *)sock_ptr);
    char                buffer[4096];
    struct sockaddr_in  sender;
    socklen_t           sender_len = sizeof(sender);
    ssize_t             bytes_received;

    struct iphdr        *iph;
    int                 iph_len;
    struct icmphdr      *icmph;
    struct timeval      send_time, recv_time;
    double              rtt;

    while (1)
    {
        bytes_received = recvfrom(sock, buffer, sizeof(buffer), 0,
                            (struct sockaddr *)&sender, &sender_len);

        if (bytes_received < 0)
        {
            perror("recvfrom error");
            continue;
        }

        if (gettimeofday(&recv_time, NULL) == -1) {
            perror("Failed to get time of day");
            continue;
        }

        iph = (struct iphdr *)buffer;
        iph_len = iph->ihl * 4;
        icmph = (struct icmphdr *)(buffer + iph_len);

        if (icmph->type == ICMP_ECHOREPLY)
        {
            memcpy(&send_time, buffer + iph_len + sizeof(struct icmphdr), sizeof(struct timeval));

            rtt = (recv_time.tv_sec - send_time.tv_sec) * 1000.0;
            rtt += (recv_time.tv_usec - send_time.tv_usec) / 1000.0;

            store_rtt(rtt);

            if (!g_data.f_args.q_flag)
                printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
                    inet_ntoa(sender.sin_addr),
                    ntohs(icmph->un.echo.sequence),
                    iph->ttl,
                    rtt);
        }
        else
        {
            fprintf(stderr, "Received non-echo reply ICMP packet (type=%d, code=%d)\n",
                    icmph->type, icmph->code);
        }
    }

    return NULL;
}

static void send_icmp_request(char *packet, uint16_t iph_totallen, struct sockaddr_in dest)
{
    if (sendto(g_data.sock, packet, ntohs(iph_totallen), 0,
        (struct sockaddr *)&dest, sizeof(dest)) < 0)
    {
        perror("sendto");
        exit_failure(NULL);
    }

    g_data.stats.packets_transmitted++;
}

static void display_ping_intro(void)
{
    printf("PING %s (%s): 56 data bytes", g_data.dest_host, inet_ntoa(g_data.dest_ip.sin_addr));
    if (g_data.f_args.v_flag)
        printf(", id 0x%x = %d", g_data.icmp_id, g_data.icmp_id);
    printf("\n");
}

void    init_ping(void)
{
    char                packet[4096];
	struct iphdr        *iph = (struct iphdr *)packet;
    struct icmphdr      *icmph = (struct icmphdr *)(packet + sizeof(struct iphdr));
    struct sockaddr_in  dest;
    int                 packet_count;
    pthread_t           recv_thread;

	memset(packet, 0, sizeof(packet));
    sock_create(&g_data.sock);
    iphdr_staticdata_prep(iph);

    // Config dest address
    dest.sin_port = 0;
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = iph->daddr;

    icmphdr_staticdata_prep(packet, icmph);
    display_ping_intro();

    // Create a thread to receive and print responses from hosts
    if (pthread_create(&recv_thread, NULL, recv_icmp_thread, &g_data.sock) != 0)
    {
        perror("Failed to create receive thread");
        exit_failure(NULL);
    }

    packet_count = g_data.f_args.count > 0 ? g_data.f_args.count : INT_MAX;

    while (g_continue_ping && packet_count > 0)
    {
        // Refill the data that changes when sending a new packet
        iphdr_dynamicdata_prep(packet, iph);
        icmphdr_dynamicdata_prep(packet, icmph);

        // Send ping to destination
        send_icmp_request(packet, iph->tot_len, dest);

        if (packet_count == 1) {
            sleep_microseconds(200000);
            break;
        }

        // Delay before sending each pack.
        sleep_microseconds(1000000);
        packet_count--;
    }

    pthread_cancel(recv_thread);
    pthread_join(recv_thread, NULL);
}
