#include "ping.h"



t_data global_data;

/*
Ethernet header - 14 bytes:
	- Destination MAC 6 bytes
	- Source MAC 6 bytes
	- EtherType 2 bytes
IP header - 20 bytes:
	- Version 4 bytes
	- IHL (Internet Header Length) 4 bytes
	- Type of Service 1 byte
	- Total Length 2 bytes
	- Identification 2 bytes
	- Flags 3 bits
	- Fragment Offset 13 bits
	- Time to Live (TTL) 1 byte
	- Protocol 1 byte
	- Header Checksum 2 bytes
	- Source IP Address 4 bytes
	- Destination IP Address 4 bytes
ICMP header - 8 bytes:
	- Type 1 byte
	- Code 1 byte
	- Checksum 2 bytes
	- Identifier 2 bytes
	- Sequence Number 2 bytes
	- Gateway 4 bytes
	- MTU 2 bytes
payload - 1458 bytes
MTU - Maximum Transmission Unit - 1500 bytes
*/


/*
Tips:
	- ICMP packets are raw packets, so you'll need root permissions to send them.
		if (getuid() != 0) {
			fprintf(stderr,"Root privileges are required to run ft_ping.\n");
			return 1;
		}
	- The checksum is critical for ensuring data integrity.
	- The structure includes support for various ICMP message types, such as Echo, Destination Unreachable, and Time Exceeded.
	- Think about Ctrl+C
*/

/*
ICMP structure from <netinet/ip_icmp.h>

A union in C is a special construct that allows multiple variables of different types to share the same memory location.
	- All fields inside a union share the same memory.
	- The size of a union is determined by its largest member
	- It is used to save memory when only one of the fields will be used at a time.

structure:
	- type: Specifies the type of ICMP message:
		0 Echo Reply
		3 Destination Unreachable
		8 Echo Request (using in ping)
		5 Redirection

	- code: Provides additional context for the type:
		for type = 3
			0: Network Unreachable
			1: Host Unreachable
			2: Protocol Unreachable
			4: Fragmentation Needed
		for type = 5
			0: Redirect for a specific network
			1: Redirect for a specific host
			2: Redirect for a specific type of service and network
			3: Redirect for a specific type of service and host

	- checksum: Ensures data integrity for the ICMP packet.
		Calculated over the entire ICMP packet, including the header and data.

	- union un: is used to hold different types of data depending on the ICMP message type (type).
		Each ICMP message type defines which specific field in the union is used.
		Here is some cases:
			- type = 8 (Echo request) or type = 0 (Echo reply) the echo field is used:
				- id:
					Uniquely identifies the process or device sending the ICMP packet.
					(usually derived from the process ID of the application initiating the request).

					Ensures that replies are correctly matched with requests.
				- sequence:
					Tracks the order of ICMP packets.
					Helps detect packet loss and calculate round-trip time RTT.

				ex:
					Request: id = 100, sequence = 1, 2, 3, 4, 5
					Reply:   id = 100, sequence = 1, 2, 4, 5 (sequence 3 is missing)
			- type = 5 (Redirect) The gateway field is used.
				- gateway: The gateway field stores the IP address of a new gateway (router) that the sender of the ICMP Redirect message suggests the target should use.
					To inform a host about a more efficient route to a destination. This is part of the network optimization process to reduce latency and avoid inefficient routing paths.
					Example Scenario:
						1. Host A sends the packet to Router 1.
						2. Router 1 forwards the packet to Router 2 but also sends an ICMP Redirect message to Host A.
						3. Host A updates its routing table to use Router 2 for packets destined for Host B.
			- type = 3 and code = 4 (Fragmentation Needed) The frag field is used. It contains the MTU value for the next hop.


	struct icmphdr {
		uint8_t	type;
		uint8_t code;
		uint16_t checksum;

		union {
			struct {
				uint16_t id;
				uint16_t sequence;
			} echo;				// For Echo Request/Reply type 0 reply , 8 - request

			uint32_t gateway;	// For ICMP Redirect messages type 5 redirection

			struct {
				uint16_t __glibc_reserved;
				uint16_t mtu;	// Maximum Transmission Unit for fragmentation messages
			} frag;				// For messages about fragmentation issues
		} un;
	};
*/

/*
IP structure from <netinet/ip.h>
struct iphdr {
	unsigned int ihl:4; 		// Header length (in 32-bit words)	(def. 4)
	unsigned int version:4;		// Version (4 for IPv4)				(def. 5)
	uint8_t tos;				// Type of service					(def. 0)
	uint16_t tot_len;			// Total length						(IP 20 bytes + ICMP 8 byte + payload 56 bytes)
	uint16_t id;				// Identification					(Incremental)
	uint16_t frag_off;			// Fragment offset field			(def. 0 or 0x4000 if no fragmentation)
	uint8_t ttl;				// Time to live						(def. 64 for linux and macOS)
	uint8_t protocol;			// Protocol							(def. 1 for ICMP)
	uint16_t check;				// Header checksum					(Data dependent)
	uint32_t saddr;				// Source address					(My ip addr)
	uint32_t daddr;				// Destination address				(Final dest. ip addr like '8.8.8.8' Google DNS)
};
*/

/*
Implement SHA-256 for data integrity ???:
	- Use a cryptographic hash function to create a hash of the ICMP packet.
	- Include the hash in a custom field of the ICMP payload.
	- At the receiver end, recalculate the hash and compare it to the received hash.
*/


int lookup_host(const char *host) {
	struct addrinfo hints, *tmpres, *result;
	int errcode;
	char addrstr[100];
	char local_canonname[100];
	void *ptr;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;

	errcode = getaddrinfo(host, NULL, &hints, &result);
	if (errcode != 0) {
		perror ("getaddrinfo");
		return -1;
	}

	tmpres = result;

	printf ("Host: %s\n", host);
	while (tmpres) {
		ptr = &((struct sockaddr_in *)tmpres->ai_addr)->sin_addr;
		inet_ntop(tmpres->ai_family, ptr, addrstr, 100); // Convert from binary to text

		if (tmpres->ai_canonname != NULL) {
			strncpy(local_canonname, tmpres->ai_canonname, sizeof(local_canonname) - 1);
			local_canonname[sizeof(local_canonname) - 1] = '\0';  // Null-terminate
		} else {
			strcpy(local_canonname, "Void");
		}

		printf("IPv%d address: %s (%s)\n", 4, addrstr, local_canonname);

		tmpres = tmpres->ai_next;
	}

  	freeaddrinfo(result);

  	return 0;
}

void test_getaddrinfo(void) {
	char inbuf[256];
	int len;

	while(1) {
		bzero(inbuf, 256);
		printf("Type domain name: ");
		fgets(inbuf, 256, stdin);

		printf("\n");

		len = strlen(inbuf);
		inbuf[len-1] = '\0';

		if(strlen(inbuf) > 0) {
			lookup_host(inbuf);
		} else {
			exit(EXIT_SUCCESS);
		}

		printf("\n");
	}

}


// typedef struct s_args {
//     bool	v_flag;       	// --verbose (-v)
//     bool	f_flag;       	// --flood (-f)
//     bool	q_flag;       	// --quiet (-q)
//     bool	help_flag;    	// --help (-?)
//     bool	version_flag; 	// --version (-V)
//     int		count;         	// --count (-c)
//     int		interval;      	// --interval (-i)
//     int		timeout;       	// --timeout (-w)
//     int		linger;        	// --linger (-W)
//     char	pattern[256];	// --pattern (-p)
//     int		ttl;           	// --ttl
// } t_args;

static struct option long_options[] = {
    {"verbose", no_argument, NULL, 'v'},      	// --verbose -> 'v'
    {"flood", no_argument, NULL, 'f'},        	// --flood -> 'f'
    {"quiet", no_argument, NULL, 'q'},        	// --quiet -> 'q'
    {"help", no_argument, NULL, '?'},         	// --help -> '?'
    {"version", no_argument, NULL, 'V'},      	// --version -> 'V'
    {"count", required_argument, NULL, 'c'},  	// --count=NUMBER -> 'c'
    {"interval", required_argument, NULL, 'i'}, // --interval=NUMBER -> 'i'
    {"timeout", required_argument, NULL, 'w'},  // --timeout=N -> 'w'
    {"linger", required_argument, NULL, 'W'},   // --linger=N -> 'W'
    {"pattern", required_argument, NULL, 'p'},  // --pattern=PATTERN -> 'p'
    {"ttl", required_argument, NULL, 0},        // --ttl=N -> -
    {"usage", no_argument, NULL, 0},        	// --usage -> -
    {0, 0, 0, 0}
};

int valid_hex() {
	char *pattern_ptr = global_data.f_args.pattern;

	while (*pattern_ptr != '\0') {
		if (isxdigit(*pattern_ptr) == 0) {
			fprintf(stderr, "ping: error in pattern near %s", pattern_ptr);
			return -1;
		}
		pattern_ptr++;
	}

	return 0;
}

int parse_arguments(int argc, char **argv) {
	(void) argc;
	(void) argv;

	int opt;
	int long_index = 0;

	global_data.f_args.v_flag = false;
	global_data.f_args.f_flag = false;
	global_data.f_args.q_flag = false;
	global_data.f_args.count = -1;
	global_data.f_args.interval = -1;
	global_data.f_args.timeout = -1;
	global_data.f_args.linger = -1;
	global_data.f_args.pattern[0] = '\0';
	global_data.f_args.ttl = -1;

	while ((opt = getopt_long(argc, argv, "vfq?Vc:i:w:W:p:", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'v':
                global_data.f_args.v_flag = true;
                break;
            case 'f':
                global_data.f_args.f_flag = true;
                break;
            case 'q':
                global_data.f_args.q_flag = true;
                break;
            case '?':
                print_help();
				exit(0);
            case 'V':
				print_version();
				exit(0);
            case 'c':
                global_data.f_args.count = atoi(optarg);
                break;
            case 'i':
                global_data.f_args.interval = atoi(optarg);
                break;
            case 'w':
                global_data.f_args.timeout = atoi(optarg);
                break;
            case 'W':
                global_data.f_args.linger = atoi(optarg);
                break;
            case 'p':
                strncpy(global_data.f_args.pattern, optarg, sizeof(global_data.f_args.pattern) - 1);
                global_data.f_args.pattern[sizeof(global_data.f_args.pattern) - 1] = '\0';
				if (valid_hex() != 0) {
					return -1;
				}
                break;
			case 0: // For args without short version
                if (strcmp("ttl", long_options[long_index].name) == 0) {
                    global_data.f_args.ttl = atoi(optarg);
                } else if (strcmp("usage", long_options[long_index].name) == 0) {
					print_usage();
					exit(0);
				}
                break;
            default:
                fprintf(stderr, "Unknown option: -%c\n", opt);
                return -1;
        }
    }

	return 0;
}

int check_args(int argc, char **argv) {
	memset(&global_data, 0, sizeof(t_data));
	(void) argc;
	(void) argv;

	if (parse_arguments(argc, argv) != 0) {
		return -1;
	}

	return 0;
}

int check_available_interface(void) {
	return 0;
}

int main(int argc, char **argv) {
	// if (getuid() != 0) {
	// 	fprintf(stderr,"Root privileges are required to run ft_ping.\n");
	// 	return 1;
	// }

	if (check_args(argc, argv) != 0) {
		return 1;
	}

	if (check_available_interface() != 0) {
		fprintf(stderr, "Error: Could not find a free network interface.\n");
        return 1;
	}

	print_args();

	return 0;
}

/*
todo:
	- resolve arguments
		* check addr ipv4
		* if !ipv4 check hostaname resolution
		*

*/

/*
doubts:
	- You will take as reference the ping implementation from inetutils-2.0 (ping -V).		✅	(sudo apt-get install inetutils-ping)
	- You are authorised to use the libc functions to complete this project.				✅	(all standart libs)
	- You have to manage the -v -? options													✅	(-v verbose, -? or -h help menu)
	- The -v option here will also allow us to see the results in case of a					✅
		problem or an error linked to the packets, which logically shouldn’t
		force the program to stop (the modification of the TTL value can help
		to force an error).
	- You will have to manage FQDN (Fully Qualified Domain Name)ex. www.example.com			✅ (do DNS resolution just on requesting, but on received response no)
		without doing the DNS resolution in the packet return
	- Except for the RTT line and the reverse DNS resolution, the result must have an		✅
		indentation identical to the implementation from inetutils-2.0

	ex:
	command: ping -c 1 google.com
	return value:	PING google.com (142.250.184.14) 56(84) bytes of data.
					64 bytes from mad41s10-in-f14.1e100.net (142.250.184.14): icmp_seq=1 ttl=114 time=10.8 ms

	so, it's not necessary to do DNS resolution on response and rtt doesn't have to be exactly the same
	A delay of +/- 30ms is tolerated on the reception of a packet.
*/

/*
All possible argument frags:
	* -v			/ --verbode
	* -f			/ --flood
	* -q			/ --quiet
	* -?			/ --help
	* -				/ --usage
	* -V			/ --version
	* -c NUMBER		/ --count=NUMBER
	* -i NUMBER 	/ --interval=NUMBER
	* -w N			/ --timeout=N
	* -W N			/ --linger=N
	* -p PATTERN	/ --pattern=PATTERN
	* -				/ --ttl=N
*/


/*
Test cases:
	-q -v: prints like -v (PING 8.8.8.8 (8.8.8.8): 56 data bytes, id 0x503a = 20538) but doesn't print more info
	-c 1 --count 2: the second one overwrites the value of packet limit
	-V --usage --help -?: always wins the first one, prints its message and stop execution
	-c 1 --count 10 -f: flood will be ignored, as -c puts limit on packets be send
	-f: works in quiet mode, prints just the first message and on ctrl+c the workflow overview

*/

/*
Ping behaviour:
*/


