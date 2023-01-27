// Server program
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>

#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <string>


#define CLIENT_PORT 9000	//Port from which we listen for client(s)
#define DEFUALT_DNS_PORT 53
#define DEFUALT_DNS_ADDR "8.8.8.8"

bool port_is_valid(const std::string &str, int *con)
{

    if ((std::all_of(str.begin(), str.end(), ::isdigit))) // C++11)
	{
		if ((*con = std::stoi(str)) > 0)
		{	
			return true;
		}
		else{
			return false;
		}
		
	}
	return false;
	
}
bool isValidIpAddress(char *ipAddress, sockaddr_in *sa)
{
    int result = inet_pton(AF_INET, ipAddress, &(sa->sin_addr));
	sa->sin_family = AF_INET;
    return result != 0;
}
// Driver code


int main(int argc, char* argv[])
{
	char buffer[512];
	int listenfd, dns_sockfd, dns_port;
	socklen_t len;
	struct sockaddr_in servaddr, cliaddr, dns_servaddr;
	struct timeval tv;

	tv.tv_sec = 3;
	tv.tv_usec = 5000;
	//bzero(&servaddr, sizeof(servaddr));
	memset(&servaddr, 0,sizeof(servaddr));
	memset(&dns_servaddr, 0,sizeof(dns_servaddr));

	if (argc > 3 || argc < 3)
	{
		printf("\n\n\nUSAGE: %s [DNS_ADDRESS] [PORT]\n\n", argv[0]);
		return 1;
	}
	// CONTINUE FROM HERE
	if (isValidIpAddress(argv[1], &dns_servaddr) == false)
	{
		printf("\n\n\nError, Invalid IP address format, USAGE: %s [DNS_ADDRESS] [PORT]\n\n", argv[0]);
		return 1;
	}
	

	if ((port_is_valid(argv[2], &dns_port)) == false)
	{
		printf("\n\n\nError, Invalid port number, USAGE: %s [DNS_ADDRESS] [PORT]\n\n", argv[0]);
		return 1;
	}

	dns_servaddr.sin_port = htons(dns_port);
	printf("DNS IP address is %s\n", inet_ntoa(dns_servaddr.sin_addr));
	printf("DNS port is %d\n\n\n", dns_port);


	// Create a UDP Socket, UDP listener
	listenfd = socket(AF_INET, SOCK_DGRAM, 0);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(CLIENT_PORT);
	servaddr.sin_family = AF_INET;

	// Create a UDP socket for Forwarding DNS requests
	dns_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	//init. dns addrs for use
	/*dns_servaddr.sin_addr.s_addr = inet_addr("8.8.8.8");
	//dns_servaddr.sin_port = htons(DNS_PORT);
	//dns_servaddr.sin_family = AF_INET;*/
	if (setsockopt(dns_sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
    	perror("Error");
	}
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
						 0, (struct sockaddr *)&cliaddr, &len); // receive message from client
		buffer[n] = '\0';
		printf("%d Bytes recieved\n\n", n);
		
		printf("DNS Query recieved, sending it to DNS server . . . \n");
		// send to DNS server
		send(dns_sockfd, buffer, n, 0);

		//Get back the DNS server response!
		int r = recv(dns_sockfd, buffer, 512, 0);

		// Send it back to the client.
		sendto(listenfd, buffer, r + 10, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));

		printf("operation done\n");
	}

	close(listenfd);
	close(dns_sockfd);
}