#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

#include "shared.h"

int main(int argc, char *argv[])
{
	int socket_descriptor, number_of_bytes;
	socklen_t client_address_struct_size;
	struct sockaddr_in server_address, client_address;
	char buffer[BUFFER_SIZE];

	socket_descriptor = socket(AF_INET, SOCK_DGRAM, DEFAULT_PROTOCOL);
	if (socket_descriptor == ERROR_VALUE)
		printf("ERROR opening socket");

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;
	bzero(&(server_address.sin_zero), sizeof(server_address.sin_zero));

	if (bind(socket_descriptor, (struct sockaddr *)&server_address, sizeof(struct sockaddr)) < 0)
		printf("ERROR on binding");

	client_address_struct_size = sizeof(struct sockaddr_in);

	while (TRUE)
	{
		/* receive from socket */
		number_of_bytes = recvfrom(
			socket_descriptor,
			buffer,
			BUFFER_SIZE,
			NONE,
			(struct sockaddr *)&client_address,
			&client_address_struct_size);
		if (number_of_bytes < 0)
			printf("ERROR on recvfrom");
		printf("Received a datagram: %s\n", buffer);

		/* send to socket */
		number_of_bytes = sendto(
			socket_descriptor,
			"Got your message\n",
			17, // Size of message
			NONE,
			(struct sockaddr *)&client_address, sizeof(struct sockaddr));
		if (number_of_bytes < 0)
			printf("ERROR on sendto");
	}

	close(socket_descriptor);
	return 0;
}