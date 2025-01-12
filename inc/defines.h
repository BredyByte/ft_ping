#ifndef DEFINES_H
# define DEFINES_H

# include <stdbool.h>
# include <stdint.h>
# include <arpa/inet.h>

# define INET4_LEN              4
# define PATTERN_SIZE           113

# define FT_PING_TTL            64
# define FT_PING_ID		        42
# define FT_PING_DELAY	        1

# define IP_HDR_SIZE			20
# define ICMP_HDR_SIZE			8
# define ICMP_TIMESTAMP_SIZE    16
# define ICMP_DATA_SIZE         40
# define ICMP_PAYLOAD_SIZE		(ICMP_TIMESTAMP_SIZE + ICMP_DATA_SIZE)
# define PACKET_SIZE			(IP_HDR_SIZE + ICMP_HDR_SIZE + ICMP_PAYLOAD_SIZE)

typedef struct s_opts
{
    bool	            v_flag;       	        // --verbose (-v)
    bool	            f_flag;       	        // --flood (-f)
    bool	            q_flag;       	        // --quiet (-q)
    int		            count;         	        // --count (-c)
    int		            interval;      	        // --interval (-i)
    int		            timeout;      	        // --timeout (-w)
    int		            linger;        	        // --linger (-W)
    char	            pattern[PATTERN_SIZE];	// --pattern (-p)
    int		            ttl;           	        // --ttl
}   t_opts;

typedef struct s_ping_stats
{
    int                 packets_transmitted;
    int                 packets_received;
    double              *rtt_values;
    int                 rtt_count;
    double              rtt_min;
    double              rtt_max;
    double              rtt_avg;
    double              rtt_stddev;
}   t_ping_stats;

typedef struct s_ping
{
	char                *dest_host;
	struct sockaddr_in  dest_ip;
	struct sockaddr_in  source_ip;
    int                 sock;
    uint16_t            sequence;
    t_opts              f_args;
    t_ping_stats        stats;
}   t_ping;

extern t_ping g_data;

#endif
