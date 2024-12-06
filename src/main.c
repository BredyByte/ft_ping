#include "ping.h"

/*
Tips:
	- ICMP packets are raw packets, so you'll need root permissions to send them.
		if (getuid() != 0) {
			fprintf(stderr,"Root privileges are required to run ft_ping.\n");
			return 1;
		}
	- The checksum is critical for ensuring data integrity.
	- The structure includes support for various ICMP message types, such as Echo, Destination Unreachable, and Time Exceeded.
*/

/*
ICMP structure from <netinet/ip_icmp.h>
	struct icmphdr {
		uint8_t	type;		// Specifies the type of ICMP message. (ex. 0 Echo Reply, 3 Destination Unreachable, 8 Echo Request (using in ping))

		uint8_t code;		// Provides additional context for the type.
							// (ex. for type = 3 (Dest. Unreachable) code indicate: 0 Network Unreachable, 1 Host Unreachable, 2 Protocol Unreachable)
		uint16_t checksum;	// Ensures data integrity for the ICMP packet. Calculated over the entire ICMP packet, including the header and data.
		union {
			struct {
				uint16_t id;	// Requests/Replies: echo.id and echo.sequence identify the message.
				uint16_t sequence;
			} echo;				// echo datagram

			uint32_t gateway;	// gateway address. For Redirects: gateway holds the new route.

			struct {
				uint16_t __glibc_reserved;
				uint16_t mtu; // For Fragmentation: frag.mtu specifies the Maximum Transmission Unit.
			} frag;			// path mtu discovery
		} un;
	};
*/

/*
Implement SHA-256 for data integrity ???:
	- Use a cryptographic hash function to create a hash of the ICMP packet.
	- Include the hash in a custom field of the ICMP payload.
	- At the receiver end, recalculate the hash and compare it to the received hash.
*/



unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;   // Treat the data as an array of 16-bit words
    unsigned int sum = 0;      // Initialize the checksum to 0
    unsigned short result;

    // Add up 16-bit words one by one
    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
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
    result = ~sum;
    return result;
}

int main() {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); // RAW-socket
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "8.8.8.8", &dest_addr.sin_addr); // IP-adddr Google DNS

    struct icmphdr icmp_hdr;
    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.code = 0;
    icmp_hdr.un.echo.id = htons(getpid());
    icmp_hdr.un.echo.sequence = htons(1);
    icmp_hdr.checksum = 0;

    // Calculate checksum
    icmp_hdr.checksum = checksum(&icmp_hdr, sizeof(icmp_hdr));

    // Dend ICMP-packet
    if (sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) <= 0) {
        perror("Send failed");
        close(sockfd);
		return 1;
    }

    printf("ICMP Echo Request sent to 8.8.8.8\n");

    close(sockfd);
    return 0;
}

/*
doubts:
	- You will take as reference the ping implementation from inetutils-2.0 (ping -V).
	- You are authorised to use the libc functions to complete this project.
	- You have to manage the -v -? options
	- The -v option here will also allow us to see the results in case of a
	problem or an error linked to the packets, which logically shouldn’t
	force the program to stop (the modification of the TTL value can help
	to force an error).
	- You will have to manage FQDN without doing the DNS resolution in the packet return
	- Except for the RTT line and the reverse DNS resolution, the result must have an
	indentation identical to the implementation from inetutils-2.0
*/
