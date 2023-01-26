// Server program
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>

// server program for udp connection
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define CLIENT_PORT 9000
#define DNS_PORT 53
#define MAXLINE 1000

// Driver code
int main()
{
	char buffer[512];
	char message[512];
	int listenfd, dns_sockfd;
	socklen_t len;
	struct sockaddr_in servaddr, cliaddr, dns_servaddr;
	bzero(&servaddr, sizeof(servaddr));

	// Create a UDP Socket
	listenfd = socket(AF_INET, SOCK_DGRAM, 0);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(CLIENT_PORT);
	servaddr.sin_family = AF_INET;

	dns_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	dns_servaddr.sin_addr.s_addr = inet_addr("8.8.8.8");
	dns_servaddr.sin_port = htons(DNS_PORT);
	dns_servaddr.sin_family = AF_INET;

	dns_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	// bind server address to socket descriptor
	bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	if (connect(dns_sockfd, (struct sockaddr *)&dns_servaddr, sizeof(dns_servaddr)) < 0)
	{
		printf("\nError : Connect Failed \n");
		exit(0);
	}

	while (true)
	{
		// receive the datagram
		len = sizeof(cliaddr);
		int n = recvfrom(listenfd, buffer, sizeof(buffer),
						 0, (struct sockaddr *)&cliaddr, &len); // receive message from server
		buffer[n] = '\0';
		puts(buffer);

		// send the response
		// sendto(listenfd, message, MAXLINE, 0,
		//(struct sockaddr *)&cliaddr, sizeof(cliaddr));
		printf("DNS Query recieved, sending it to DNS server . . . \n");
		send(dns_sockfd, buffer, n, 0);

		int r = recv(dns_sockfd, buffer, 512, 0);

		sendto(listenfd, buffer, r + 10, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));

		printf("operation done\n");
	}

	close(listenfd);
	close(dns_sockfd);
}