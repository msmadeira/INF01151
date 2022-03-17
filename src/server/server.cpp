// C
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

#include "../shared/shared.h"

using namespace std;

int main(int argc, char *argv[])
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;	/* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = AI_PASSIVE;	/* For wildcard IP address */
	hints.ai_protocol = 0;			/* Any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	struct addrinfo *address_candidates;
	int exit_code = getaddrinfo(NULL, PORT_STR, &hints, &address_candidates);
	if (exit_code != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(exit_code));
		exit(EXIT_FAILURE);
	}

	struct addrinfo *server_address;
	int socket_descriptor;
	for (server_address = address_candidates; server_address != NULL; server_address = server_address->ai_next)
	{
		socket_descriptor = socket(server_address->ai_family, server_address->ai_socktype, server_address->ai_protocol);
		if (socket_descriptor == -1)
			continue;

		if (bind(socket_descriptor, server_address->ai_addr, server_address->ai_addrlen) == 0)
			break; /* Success */

		close(socket_descriptor);
	}

	if (server_address == NULL)
	{ /* No address succeeded */
		fprintf(stderr, "Could not bind\n");
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(address_candidates); /* No longer needed */

	char buffer[BUFFER_SIZE];
	char host[NI_MAXHOST], service[NI_MAXSERV];
	struct sockaddr_in client_address;
	socklen_t client_address_struct_size = sizeof(struct sockaddr_storage);
	for (;;)
	{
		/* receive from socket */
		int number_of_bytes = recvfrom(
			socket_descriptor,
			buffer,
			BUFFER_SIZE,
			NONE,
			(struct sockaddr *)&client_address,
			&client_address_struct_size);
		if (number_of_bytes < 0)
			printf("ERROR on recvfrom");
		printf("Received a datagram: %s\n", buffer);

		exit_code = getnameinfo(
			(struct sockaddr *)&client_address,
			client_address_struct_size,
			host,
			NI_MAXHOST,
			service,
			NI_MAXSERV,
			NI_NUMERICSERV);
		if (exit_code == 0)
			printf("Received %d bytes from %s:%s\n", number_of_bytes, host, service);
		else
			fprintf(stderr, "getnameinfo: %s\n", gai_strerror(exit_code));

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