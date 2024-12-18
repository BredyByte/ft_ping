#include "ping.h"

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

	Second itter:
		*buf = 0x5678
		sum += 0x5678 -> sum = 0x1234 + 0x5678 = 0x68AC

	Third itter:
		*buf = 0x9ABC
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


void simple_ping(void) {
	int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        perror("Socket creation failed");
        exit (1);
    }

	struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr("8.8.8.8");

	char packet[64];
    memset(packet, 0, sizeof(packet));

	struct icmphdr *icmph = (struct icmphdr *)packet;
    icmph->type = ICMP_ECHO;
    icmph->code = 0;
    icmph->un.echo.id = htons(1234);
    icmph->un.echo.sequence = htons(1);
    icmph->checksum = 0;

	strcpy(packet + sizeof(struct icmphdr), "Hello ICMP!");

	icmph->checksum = checksum(packet, sizeof(struct icmphdr) + strlen("Hello ICMP!"));

	if (sendto(sock, packet, sizeof(struct icmphdr) + strlen("Hello ICMP!"), 0,
		(struct sockaddr *)&dest, sizeof(dest)) < 0) {
        perror("Sendto failed");
    } else {
        printf("ICMP packet sent to 8.8.8.8\n");
    }

    close(sock);
}
