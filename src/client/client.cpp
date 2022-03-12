// C
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// C++
#include <iostream>

#include "../shared/shared.h"

using namespace std;

int main(int argc, char *argv[])
{
	char buffer[BUFFER_SIZE];
	if (argc < 2)
	{
		fprintf(stderr, "usage %s hostname\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = NONE;			/* For wildcard IP address */
	hints.ai_protocol = DEFAULT_PROTOCOL;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	struct addrinfo *address_candidates;
	int exit_code = getaddrinfo(argv[1], PORT_STR, &hints, &address_candidates);
	if (exit_code != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(exit_code));
		exit(EXIT_FAILURE);
	}

	struct addrinfo *server_address;
	int socket_descriptor;
	for (server_address = address_candidates; server_address != NULL; server_address = server_address->ai_next)
	{
		socket_descriptor = socket(
			server_address->ai_family,
			server_address->ai_socktype,
			server_address->ai_protocol);
		if (socket_descriptor == -1)
			continue;

		if (connect(socket_descriptor, server_address->ai_addr, server_address->ai_addrlen) != -1)
			break; /* Success */

		close(socket_descriptor);
	}
	freeaddrinfo(address_candidates);

	if (server_address == NULL)
	{ /* No address succeeded */
		fprintf(stderr, "Could not bind\n");
		exit(EXIT_FAILURE);
	}

	printf("Enter the message: ");
	bzero(buffer, BUFFER_SIZE);
	fgets(buffer, BUFFER_SIZE, stdin);

	int number_of_bytes = write(socket_descriptor, buffer, strlen(buffer));
	if (number_of_bytes != strlen(buffer))
	{
		fprintf(stderr, "partial/failed write\n");
		exit(EXIT_FAILURE);
	}

	number_of_bytes = read(socket_descriptor, buffer, BUFFER_SIZE);
	if (number_of_bytes == -1)
	{
		perror("read");
		exit(EXIT_FAILURE);
	}

	printf("Received %d bytes: %s\n", number_of_bytes, buffer);

	close(socket_descriptor);
	return 0;
}