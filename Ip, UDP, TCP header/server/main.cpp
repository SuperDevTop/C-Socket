
#include <iostream>
#include <winsock2.h>
using namespace std;

#pragma comment(lib,"ws2_32.lib") // Winsock Library
#pragma warning(disable:4996) 

#define BUFLEN 512
#define PORT 8888

typedef struct ip_hdr
{
    unsigned char  ip_version;        // 4-bit IPv4 version 4-bit header length (in 32-bit words)
    unsigned char  ip_headerlength;        // 4-bit IPv4 version 4-bit header length (in 32-bit words)
    unsigned char  ip_ttl;           // Time to live
    unsigned char  ip_protocol;      // Protocol(TCP,UDP etc)
    unsigned int   ip_srcaddr;       // Source address
    unsigned int   ip_destaddr;      // Dest. address
    unsigned short ip_totallength;   // Total length
    unsigned short ip_id;            // Unique identifier
    unsigned short ip_offset;        // Fragment offset field
    unsigned char ip_checksum;      // IP checksum
    unsigned char  ip_tos;           // IP type of service
//    unsigned char[] ip_message;      // Message to be sent

} IPV4_HDR, *PIPV4_HDR, FAR * LPIPV4_HDR;

typedef struct udp_hdr
{
    unsigned char udp_length;       // Udp packet length
    unsigned char udp_checksum;     // Udp checksum (optional)
    unsigned short src_portno;       // Source port no.
    unsigned short dst_portno;       // Dest. port no.
} UDP_HDR, *PUDP_HDR;

//Define the TCP header
typedef struct tcp_hdr
{
	unsigned char sequence_number; //Sequence number
	unsigned char acknowledgement;  //Acknowledge number
	unsigned char header_length;  //Header Length
	unsigned short src_portno;   //Source port no.
	unsigned short dest_portno;   //Dest. port no.
}TCP_HDR, *PTCP_HDR;

int main()
{
    system("title Server");

    sockaddr_in server, client;
    IPV4_HDR* pIP;
    TCP_HDR *pTCP;
    UDP_HDR *pUDP;

    // initialise winsock
    WSADATA wsa;
    printf("Initialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code: %d", WSAGetLastError());
        exit(0);
    }
    printf("Initialised.\n");

    // create a socket
    SOCKET server_socket;
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket: %d", WSAGetLastError());
    }
    printf("Socket created.\n");

    // prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // bind
    if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code: %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    puts("Bind done.");
	static int count = 0;
	
    while (true)
    {
        printf("Waiting for data...");
        fflush(stdout);
        char message[BUFLEN] = {};
        char buf[1024];
		count++;
        // try to receive some data, this is a blocking call
        int message_len;
        int slen = sizeof(sockaddr_in);
        
		if(count == 1){	
	        if (message_len = recvfrom(server_socket, buf, BUFLEN, 0, (sockaddr*)&client, &slen) == SOCKET_ERROR)
	        {
	            printf("recvfrom() failed with error code: %d", WSAGetLastError());
	            exit(0);
	        }

        // print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		pIP = (IPV4_HDR *)buf;
		pUDP = (UDP_HDR *)&buf[sizeof(IPV4_HDR)];
		pTCP = (TCP_HDR *)&buf[sizeof(IPV4_HDR) + sizeof(UDP_HDR)];
		
		struct in_addr paddr_src;	
		paddr_src.S_un.S_addr = pIP->ip_srcaddr;
		char *srcIP = inet_ntoa(paddr_src);

        printf("\n\t IP Version: %d\n \
		IP Header Length: %d\n \
		IP TTL: %d\n \
		IP Protocol: %s\n \
		IP Source Address: %s\n \
		IP Destination Address: %s\n\n \
		UDP Source Port: %s\n \
		UDP Destination Port: %d\n \
		UDP Length: %d\n \
		UDP Checksum: %d\n\n \
		TCP Source Port: %s\n \
		TCP Destination Port: %d\n \
		TCP Sequence Number: %d\n \
		TCP Acknowledgement: %d\n \
		TCP Header Length: %d\n",
		pIP->ip_version, pIP->ip_headerlength, pIP->ip_ttl, "IPPRITI_UDP", srcIP, "127.0.0.1",
		"8888", ntohs(client.sin_port), pUDP->udp_length, pUDP->udp_checksum,
		"8888", ntohs(client.sin_port), pTCP->sequence_number, pTCP->acknowledgement, pTCP->header_length);
	}
	 else{
	    if (message_len = recvfrom(server_socket, message, BUFLEN, 0, (sockaddr*)&client, &slen) == SOCKET_ERROR)
	    {
	        printf("recvfrom() failed with error code: %d", WSAGetLastError());
	        exit(0);
	    }
	    
	    printf("Received packet from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
	    printf("Data: %s\n", message);
	}

        cin.getline(message, BUFLEN);

        // reply the client with 2the same data
        if (sendto(server_socket, message, strlen(message), 0, (sockaddr*)&client, sizeof(sockaddr_in)) == SOCKET_ERROR)
        {
            printf("sendto() failed with error code: %d", WSAGetLastError());
            return 3;
        }
    }

    closesocket(server_socket);
    WSACleanup();
}