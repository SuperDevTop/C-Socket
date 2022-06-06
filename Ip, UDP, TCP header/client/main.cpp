

#include <iostream>
#include <winsock2.h>
using namespace std;

//#pragma comment(lib,"ws2_32.lib") 
#pragma warning(disable:4996) 

#define SERVER "127.0.0.1"  // or "localhost" - ip address of UDP server
#define BUFLEN 512  // max length of answer
#define PORT 8888  // the port on which to listen for incoming data

//

// IPv4 header

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
    unsigned char  ip_checksum;      // IP checksum
    unsigned char  ip_tos;           // IP type of service

} IPV4_HDR, *PIPV4_HDR;


// Define the UDP header

typedef struct udp_hdr
{
    unsigned char udp_length;       // Udp packet length
    unsigned char udp_checksum;     // Udp checksum (optional)
    unsigned short src_portno;       // Source port no.
    unsigned short dst_portno;       // Dest. port no.
} UDP_HDR, *PUDP_HDR;
//

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

	IPV4_HDR *v4hdr = NULL;
	UDP_HDR  *udphdr = NULL;
    TCP_HDR *tcphdr = NULL;
	USHORT   sourceport = 5000, destport=8888;
	int      payload = 512;  // size of UDP data

	SOCKADDR_STORAGE dest;
	char buf[1024];
	
	v4hdr = (IPV4_HDR *)buf;
	// v4hdr->ip_verlen = (4 << 4) │ (sizeof(IPV4_HDR) / sizeof(ULONG));
    v4hdr->ip_version = 4;
    v4hdr->ip_headerlength = 32;
	v4hdr->ip_tos    = 0;
	v4hdr->ip_totallength = htons(sizeof(IPV4_HDR) + sizeof(UDP_HDR) + sizeof(TCP_HDR));
	v4hdr->ip_id     = 0;
	v4hdr->ip_offset = 0;
	v4hdr->ip_ttl    = 8;    // Time-to-live is eight
	v4hdr->ip_protocol = IPPROTO_UDP;
	v4hdr->ip_checksum = 0;
	v4hdr->ip_srcaddr  = inet_addr("127.0.0.1");
	v4hdr->ip_destaddr = inet_addr("127.0.0.1");
// Calculate checksum for IPv4 header

//   The checksum() function computes the 16-bit one's

//   complement on the specified buffer.

	// v4hdr->ip_checksum = checksum(v4hdr, sizeof(IPV4_HDR));

// Initialize the UDP header

//	printf("%d, %d, %d, %d", sizeof(IPV4_HDR), sizeof(int), sizeof(char), sizeof(short));
	udphdr = (UDP_HDR *)&buf[sizeof(IPV4_HDR)];
	udphdr->udp_length = sizeof(UDP_HDR);
	udphdr->udp_checksum = 0;
	udphdr->src_portno = htons(sourceport);
	udphdr->dst_portno = htons(destport);

    tcphdr = (TCP_HDR *)&buf[sizeof(IPV4_HDR) + sizeof(UDP_HDR)];
    tcphdr->src_portno = htons(sourceport);
    tcphdr->dest_portno = htons(destport);
    tcphdr->sequence_number = 10;
    tcphdr->acknowledgement = 10;
    tcphdr->header_length = 4;
    
//    printf("%d,%d,%d", v4hdr, udphdr, tcphdr);

// Initialize the UDP payload to something

//	data = &buf[sizeof(IPV4_HDR) + sizeof(UDP_HDR)];
//	memset(data, '^', payload);
//	udphdr->udp_checksum = Ipv4PseudoHeaderChecksum(v4hdr, udphdr, data, sizeof(IPV4_HDR) + sizeof(UDP_HDR) + payload);
 
    system("title Client");

    // initialise winsock
    WSADATA ws;
    printf("Initialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
    {
        printf("Failed. Error Code: %d", WSAGetLastError());
        return 1;
    }
    printf("Initialised.\n");

    // create socket
    sockaddr_in server;
    int client_socket;
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) // <<< UDP socket
    {
        printf("socket() failed with error code: %d", WSAGetLastError());
        return 2;
    }

    // setup address structure
    memset((char*)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.S_un.S_addr = inet_addr(SERVER);
    
    static int count = 0;

    // start communication
    while (true)
    {
    	count++;
        char message[BUFLEN];
        printf("Enter message: ");
        cin.getline(message, BUFLEN);

        // send the message
//        if (sendto(client_socket, message, strlen(message), 0, (sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR)
//        {
//            printf("sendto() failed with error code: %d", WSAGetLastError());
//            return 3;
//        }
        
        //Send packet
        if(count == 1)
        {
	        if (sendto(client_socket, buf, 33, 0, (sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR)
	        {
	            printf("sendto() failed with error code: %d", WSAGetLastError());
	            return 3;
	        }        	
		}
		else{
			if (sendto(client_socket, message, strlen(message), 0, (sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR)
	        {
	            printf("sendto() failed with error code: %d", WSAGetLastError());
	            return 3;
	        }   
		}


        // receive a reply and print it
        // clear the answer by filling null, it might have previously received data
        char answer[BUFLEN] = {};

        // try to receive some data, this is a blocking call
        int slen = sizeof(sockaddr_in);
        int answer_length;
        
        if (answer_length = recvfrom(client_socket, answer, BUFLEN, 0, (sockaddr*)&server, &slen) == SOCKET_ERROR)
        {
            printf("recvfrom() failed with error code: %d", WSAGetLastError());
            exit(0);
        }

        cout << answer << "\n";
    }

    closesocket(client_socket);
    WSACleanup();
}