#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include <stdint.h>
#include <arpa/inet.h>

#define INET4_LEN 4
#define PATTERN_SIZE 113

typedef struct s_args {
    bool	v_flag;       	        		// --verbose (-v)
    bool	f_flag;       	        		// --flood (-f)
    bool	q_flag;       	        		// --quiet (-q)
    int		count;         	        		// --count (-c)
    int		interval;      	        		// --interval (-i)
    int		timeout;      	        		// --timeout (-w)
    int		linger;        	        		// --linger (-W)
    char	pattern[PATTERN_SIZE];			// --pattern (-p)
    int		ttl;           	        		// --ttl
} t_args;

typedef struct s_data {
	char	dest_ip_str[INET_ADDRSTRLEN];	// IPv4 argument in string
	uint8_t dest_ip_addr[INET4_LEN];		// IPv4-addr in binary format
	t_args  f_args;
} t_data;

extern t_data global_data;

#endif
