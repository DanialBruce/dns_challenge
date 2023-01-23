#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <cstdio>

#define PORT 9000
#define MAXLINE 1024

using std::cout;
using std::endl;
using std::perror;


bool is_number(const std::string& s)
{
    return !s.empty() && find_if(s.begin(), 
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}


int main(int argc, char *argv[])
{
	int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in dns_servaddr, servaddr, cliaddr;

	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	inet_pton(AF_INET, "8.8.8.8", &(dns_servaddr.sin_addr));

	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

	// Filling dns_server information
	dns_servaddr.sin_family = AF_INET; // IPv4
	dns_servaddr.sin_addr.s_addr = INADDR_ANY;
	dns_servaddr.sin_port = htons(PORT);

	// Bind the socket with the server address
	if (bind(sockfd, (const struct sockaddr *)&servaddr,
			 sizeof(servaddr)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	socklen_t len;
	int n;

	len = sizeof(cliaddr); // len is value/result

	// Listen for client, until recieve.
	n = recvfrom(sockfd, (char *)buffer, MAXLINE,
				 MSG_WAITALL, (struct sockaddr *)&cliaddr,
				 &len);
	buffer[n] = '\0';
	printf("Client : %s\n", buffer);

	// Send DNS request to DNS Server
	sendto(sockfd, (const char *)buffer, n,
		   MSG_CONFIRM, (const struct sockaddr *)&dns_servaddr,
		   len);

	// Wait for DNS server response
	n = recvfrom(sockfd, (char *)buffer, MAXLINE,
				 MSG_WAITALL, (struct sockaddr *)&dns_servaddr,
				 &len);

	// Rturn the DNS server response back to client.
	sendto(sockfd, (const char *)buffer, n,
		   MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
		   len);


	return 0;
}