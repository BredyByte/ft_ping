# define _GNU_SOURCE
# include "defines.h"
# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <ifaddrs.h>
# include <sys/ioctl.h>
# include <net/if.h>
# include <sys/socket.h>
# include <linux/if_link.h>
# include <netdb.h>
# include <sys/ioctl.h>
# include <unistd.h>

static int  is_internet_connected(const char *interface_name)
{
    int sock, s, connect_status;
    struct sockaddr_in server;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation failed.");
        return -1;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE,
        interface_name, strlen(interface_name)) < 0)
    {
        perror("Failed to bind socket to interface");
        close(sock);
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(53); // DNS-port

    s = inet_pton(AF_INET, "8.8.8.8", &server.sin_addr);
    if (s <= 0)
    {
        if (s == 0)
            fprintf(stderr, "Not in presentation format");
        else
            perror("inet_pton");
        close(sock);
        return -1;
    }

    connect_status = connect(sock, (struct sockaddr*)&server, sizeof(server));

    close(sock);
    return connect_status;
}

void    get_available_interface(void)
{
    struct ifaddrs *ifaddr;
    struct sockaddr_in *addr;
    int s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        memset(&host, 0, NI_MAXHOST);

        /*
        ** AF_INET - for IPv4, IFF_UP - active and ready to work interface,
        ** IFF_LOOPBACK - exclude localhost.
        */
        if (ifa->ifa_addr != NULL
            && ifa->ifa_addr->sa_family == AF_INET
            && (ifa->ifa_flags & IFF_UP)
            && !(ifa->ifa_flags & IFF_LOOPBACK)
            && (ifa->ifa_flags & IFF_BROADCAST)) {

            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                            host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0)
                continue;  // Skip if error in retrieving address

            // Sheck if interface is connected to the Internet
            if (is_internet_connected(ifa->ifa_name) == 0)
            {
                addr = (struct sockaddr_in *)ifa->ifa_addr;
	            memcpy(global_data.source_ip, &addr->sin_addr, sizeof(global_data.dest_ip));

                break;  // Stop after finding the first valid interface
            }
        }
    }

    freeifaddrs(ifaddr);

    if (host[0] == 0)
    {
        fprintf(stderr, "Error: No suitable network interface found.\n");
        exit(EXIT_FAILURE);
    }
}
