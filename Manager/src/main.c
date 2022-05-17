// Server side C/C++ program to demonstrate Socket programming
// See: https://www.geeksforgeeks.org/socket-programming-cc/

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "manager.h"

int main(int argc, char const *argv[])
{
    int client_socket, server_socket, valread;
    char buffer[BUFFER_SIZE] = {0};
    char *hello = "Hello from server\n";
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1;

    printf("Starting manager server\n");
    // Creating socket file descriptor
    // AF_INET is IPv4, SOCK_STREAM is tcp connection
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // set socket options including connection to the port 8080
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, //| SO_REUSEPORT,     //GNP: SO_REUSEADDR not available on macos!
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE); // GNP: the setsockopt gives an error message. If we continue, the communication works???
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // attach socket to server
    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, MAX_CONNECTIONS) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Manager online and listening for connections\n");
    if ((client_socket = accept(server_socket, (struct sockaddr *)&address,
                                (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    printf("New client connection from address: %s\n", inet_ntoa(address.sin_addr));
    
    while (1)
    {
        sleep(1);
        valread = read(client_socket, buffer, BUFFER_SIZE);
        printf("Server received: %s\n", buffer);
        send(client_socket, hello, strlen(hello), strlen(hello));
    }
    printf("Server: Hello message sent\n");

    return 0;
}
