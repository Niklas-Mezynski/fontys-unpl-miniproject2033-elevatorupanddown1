// Server side C/C++ program to demonstrate Socket programming
// See: https://www.geeksforgeeks.org/socket-programming-cc/

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080
#define MAX_CONNECTIONS 8

int main(int argc, char const *argv[])
{
	int server_fd, client_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};
	char *hello = "Hello from server\n";

	// Creating socket file descriptor
	// AF_INET is IPv4, SOCK_STREAM is tcp connection
	printf("Starting manager\n");

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// set socket options including connection to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, //| SO_REUSEPORT,     //GNP: SO_REUSEADDR not available on macos!
				   &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE); // GNP: the setsockopt gives an error message. If we continue, the communication works???
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// attach socket to server
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	// 3 is here the max number of client connections
	if (listen(server_fd, MAX_CONNECTIONS) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	printf("Manager online and listening for connections\n");
	if ((client_socket = accept(server_fd, (struct sockaddr *)&address,
								(socklen_t *)&addrlen)) < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	// for (int i = 0; i < 4; i++)
	// {
	// 	sleep(2);
	// 	valread = read(client_socket, buffer, 1024);
	// 	printf("Server received: %s\n", buffer);
	// 	send(client_socket, hello, strlen(hello), strlen(hello));
	// }
	while (1)
	{
		sleep(2);
		valread = read(client_socket, buffer, 1024);
		printf("Server received: %s\n", buffer);
		send(client_socket, hello, strlen(hello), strlen(hello));
	}
	printf("Server: Hello message sent\n");

	return 0;
}
