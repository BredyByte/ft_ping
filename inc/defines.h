#ifndef DEFINES_H
# define DEFINES_H

# include <stdbool.h>
# include <stdint.h>
# include <arpa/inet.h>
# include <stdbool.h>

# define INET4_LEN              4
# define PATTERN_SIZE           41
# define RTT_RESERVE_BLOCK_SIZE 20              // Block size for memory allocation (for rtt_values array)

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
    bool	            q_flag;       	        // --quiet (-q)
    int		            count;         	        // --count (-c)
    char	            pattern[PATTERN_SIZE];	// --pattern (-p)
}   t_opts;

typedef struct s_ping_stats
{
    int                 packets_transmitted;
    int                 packets_received;
    double              *rtt_values;
    int                 rtt_count;              // Current len of rtt_values array
    int                 rtt_capacity;           // Current capacity of rtt_values array
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
    pid_t               icmp_id;                // In inetutils, ping PID is used for imcp pack. id
    t_opts              f_args;
    t_ping_stats        stats;
}   t_ping;

extern t_ping           g_data;
extern volatile bool    g_continue_ping;
#endif
