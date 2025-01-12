# include "defines.h"
# include <arpa/inet.h>
# include <stdbool.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <errno.h>
# include <netdb.h>

static void	hostname_resolution(const char *host)
{
	struct addrinfo hints, *result;
	int errcode;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	errcode = getaddrinfo(host, NULL, &hints, &result);
	if (errcode != 0)
	{
		fprintf(stderr, "ft_ping: unknown host\n");
		exit(EXIT_FAILURE);
	}

	memcpy(&g_data.dest_ip, result->ai_addr, sizeof(struct sockaddr_in));

  	freeaddrinfo(result);
}

void	ip_resolve_and_validate(char *hostip)
{
	struct sockaddr_in sa;
    int result = inet_pton(AF_INET, hostip, &(sa.sin_addr));
	int len;

	if (result == 1 )
	{
		g_data.dest_ip.sin_family = AF_INET;
        g_data.dest_ip.sin_addr = sa.sin_addr;
	}
	else
	{
		hostname_resolution(hostip);
	}

	len = strlen(hostip);
	g_data.dest_host = malloc((len + 1 ) * sizeof(char));
	if (g_data.dest_host == NULL)
	{
		fprintf(stderr, "Error on allocation memory: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	strncpy(g_data.dest_host, hostip, len);
	g_data.dest_host[len] = '\0';
}
