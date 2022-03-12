#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "shared.h"

int main(int argc, char *argv[])
{
	int socket_descriptor, number_of_bytes;
	unsigned int size_of_message = sizeof(struct sockaddr_in);
	struct sockaddr_in server_address, from;
	struct hostent *server;

	char buffer[BUFFER_SIZE];
	if (argc < 2)
	{
		fprintf(stderr, "usage %s hostname\n", argv[0]);
		exit(0);
	}

	server = gethostbyname(argv[1]);
	if (server == NULL)
	{
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}

	socket_descriptor = socket(AF_INET, SOCK_DGRAM, DEFAULT_PROTOCOL);
	if (socket_descriptor == ERROR_VALUE)
		printf("ERROR opening socket");

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr = *((struct in_addr *)server->h_addr);
	bzero(&(server_address.sin_zero), sizeof(server_address.sin_zero));

	printf("Enter the message: ");
	bzero(buffer, BUFFER_SIZE);
	fgets(buffer, BUFFER_SIZE, stdin);

	number_of_bytes = sendto(
		socket_descriptor,
		buffer,
		strlen(buffer),
		NONE,
		(const struct sockaddr *)&server_address,
		size_of_message);
	if (number_of_bytes < 0)
		printf("ERROR sendto");

	number_of_bytes = recvfrom(
		socket_descriptor, buffer,
		BUFFER_SIZE,
		NONE,
		(struct sockaddr *)&from,
		&size_of_message);
	if (number_of_bytes < 0)
		printf("ERROR recvfrom");

	printf("Got an ack: %s\n", buffer);

	close(socket_descriptor);
	return 0;
}