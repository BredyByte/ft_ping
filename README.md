# ft_ping

> This repository contains 'ft_ping' project that is part of the École 42 curriculum.

## Overview
ft_ping is a 42 École outer core project centered on networking and system-level programming. The objective is to create a program in C that mimics the behavior of the ping command, allowing users to test the reachability of a host in a network and measure **round-trip time (RTT)** for messages sent to a destination.

This project dives into Layer 3 (Network Layer) of the OSI model, providing insights into core networking concepts such as: **ICMP (Internet Control Message Protocol)**, IP addresses, RTT, **TTL (Time To Live)**, and packet structure. By implementing ft_ping, students gain a deeper understanding of network diagnostics, packet generation, and transmission, as well as how to interpret and handle ICMP responses effectively.

## Packet Structure
Here, I will provide all the headers related to the ICMP packet for better visualization.

### Ethernet Header
```sql
+---------------------------------------------------------------+
|		Destination MAC Address (6 bytes)		|
+---------------------------------------------------------------+
|		Source MAC Address (6 bytes)			|
+---------------------------------------------------------------+
|		EtherType / Length (2 bytes)			|
+---------------------------------------------------------------+
|		Data/Payload (46-1500 bytes)			|
+---------------------------------------------------------------+
```

### IP Header
```sql
+--------------------+--------------------------+-------------------------------+
| Version (4 bits)   | Header Length (4 bits)   |  Type of Service (8 bits)	|
+-------------------------------------------------------------------------------+
|				Total Length(16 bits)				|
+-------------------------------------------------------------------------------+
|				Identification (16 bits)			|
+-------------------+-----------------------------------------------------------+
|		Flags (3 bits)		|	Fragment Offset (13 bits)	|
+---------------------------------------+---------------------------------------+
|		Time to Live (8 bits)	|	Protocol (8 bits)		|
+-------------------------------------------------------------------------------+
|				Header Checksum (16 bits)			|
+-------------------------------------------------------------------------------+
|                   		Source IP Address (32 bits)			|
+-------------------------------------------------------------------------------+
|				Destination IP Address (32 bits)		|
+-------------------------------------------------------------------------------+
|				Data/Payload					|
+-------------------------------------------------------------------------------+
```

### ICMP Header
```sql
+-------------------------------------------------------+
|	Type (1 byte)	|	Code (1 byte)		|
+-------------------------------------------------------+
|		Checksum (2 bytes)			|
+-------------------------------------------------------+
|		Identifier (2 bytes, optional)		|
+-------------------------------------------------------+
| 		Sequence Number (2 bytes, optional)	|
+-------------------------------------------------------+
|		Payload (Timestamp and Data)		|
+-------------------------------------------------------+
|		Timestamp (16 bytes)			|
+-------------------------------------------------------+
|		Data (40 bytes)				|
+-------------------------------------------------------+
```

### IP Header defined in netinet/ip.h
```c
struct iphdr
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int	ihl:4;      	// Internet Header Length
    unsigned int	version:4;	// IP Version: Typically 4 for IPv4.

#elif __BYTE_ORDER == __BIG_ENDIAN
    unsigned int	version:4;
    unsigned int	ihl:4;

#else
# error "Please fix <bits/endian.h>"

#endif
    uint8_t		tos;        	// Type of Service
    uint16_t		tot_len;    	// Total size of the IP packet (header + payload) in bytes.
    uint16_t		id;         	// Unique identifier for fragmented packets.
    uint16_t		frag_off;   	// Flags and offset for handling packet fragmentation.
    uint8_t		ttl;        	// Time To Live: Maximum number of hops the packet can travel before being discarded.
    uint8_t		protocol;   	// Protocol: Protocol used in the data portion (e.g., TCP = 6, UDP = 17).
    uint16_t		check;      	// Header Checksum: Error-checking field for the IP header.
    uint32_t		saddr;      	// Source Address
    uint32_t		daddr;      	// Destination Address
    /* The options start here. */
};
```

### ICMP Header defined in netinet/ip_icmp.h
```c
struct icmphdr
{
  uint8_t		type;		// ICMP message type (e.g., Echo Request = 8, Echo Reply = 0).
  uint8_t		code;		// Provides additional information about the message type.
  uint16_t		checksum;	// Error-checking field for the ICMP header and payload.
  union
    {
      struct
      {
        uint16_t	id;		// Used to match requests and replies (e.g., in ping).
        uint16_t	sequence;	// Helps in ordering Echo Request and Reply messages.
      } echo;

      uint32_t		gateway;	// Used in ICMP Redirect messages to specify the new gateway.

     struct
     {
       uint16_t	__glibc_reserved;
       uint16_t	mtu;
     } frag;

    } un;				// Holds fields specific to the ICMP message type.
};
```

