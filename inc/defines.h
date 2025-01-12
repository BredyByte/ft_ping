#ifndef DEFINES_H
# define DEFINES_H

# include <stdbool.h>
# include <stdint.h>
# include <arpa/inet.h>

# define INET4_LEN      4
# define PATTERN_SIZE   113

# define FT_PING_TTL    64
# define FT_PING_ID		42
# define FT_PING_DELAY	1

# define IP_HDR_SIZE			20
# define ICMP_HDR_SIZE			8
# define ICMP_TIMESTAMP_SIZE    16
# define IMCP_DATA_SIZE         40
# define ICMP_PAYLOAD_SIZE		(ICMP_TIMESTAMP_SIZE + IMCP_DATA_SIZE)
# define PACKET_SIZE			(IP_HDR_SIZE + ICMP_HDR_SIZE + ICMP_PAYLOAD_SIZE)

typedef struct s_args {
    bool	v_flag;       	        // --verbose (-v)
    bool	f_flag;       	        // --flood (-f)
    bool	q_flag;       	        // --quiet (-q)
    int		count;         	        // --count (-c)
    int		interval;      	        // --interval (-i)
    int		timeout;      	        // --timeout (-w)
    int		linger;        	        // --linger (-W)
    char	pattern[PATTERN_SIZE];	// --pattern (-p)
    int		ttl;           	        // --ttl
} t_args;

typedef struct s_data {
	char	*dest_host;             // hostname in string format
	uint8_t dest_ip[INET4_LEN];     // dest IPv4 addr in binary format
	uint8_t source_ip[INET4_LEN];   // source IPv4 addr in binary format
	t_args  f_args;
} t_data;

extern t_data global_data;

#endif
