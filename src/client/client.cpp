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
#include <pthread.h>

// C++
#include <iostream>

#include "../libs/jsoncpp/json/json.h"
#include "../shared/shared.h"

using namespace std;

void write_from_buffer(int socket_descriptor, const char *buffer)
{
	int number_of_bytes = write(socket_descriptor, buffer, strlen(buffer));
	if (number_of_bytes != strlen(buffer))
	{
		fprintf(stderr, "partial/failed write\n");
		exit(EXIT_FAILURE);
	}
}

void read_to_buffer(int socket_descriptor, void *buffer)
{
	int number_of_bytes = read(socket_descriptor, buffer, BUFFER_SIZE);
	if (number_of_bytes == -1)
	{
		perror("read");
		exit(EXIT_FAILURE);
	}
}

struct ReceiverSystem
{
	int socket_descriptor;
};

void *fn_receiver(void *arg)
{
	auto receiver_system = static_cast<ReceiverSystem *>(arg);
	int socket_descriptor = receiver_system->socket_descriptor;
	char buffer[BUFFER_SIZE];
	Json::Reader reader;
	Json::Value messageValue;
	for (;;)
	{
		read_to_buffer(socket_descriptor, buffer);

		bool parseSuccess = reader.parse(buffer, messageValue, false);
		if (!parseSuccess)
		{
			cout << "ERROR parsing message on fn_receiver: " << messageValue << endl;
			exit(EXIT_FAILURE);
		}
		int server_msg_id = messageValue["id"].asInt();
		ServerMsgType server_msg_type = static_cast<ServerMsgType>(messageValue["type"].asInt());
		switch (server_msg_type)
		{
		case ServerMsgType::ServerMessage:
		{
			cout << "Received message: " << endl;
			string username = messageValue["username"].asString();
			cout << "From: @" << username << endl;
			string body = messageValue["body"].asString();
			cout << "Message: " << body << endl
				 << endl;
			break;
		}
		default:
		{
			cout << "Invalid server message type received on fn_receiver: " << server_msg_type << endl;
			exit(EXIT_FAILURE);
			break;
		}
		}
	}
}

int main(int argc, char *argv[])
{
	char buffer[BUFFER_SIZE];
	if (argc < 4)
	{
		fprintf(stderr, "usage %s username address port\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	string username(argv[1]);
	if (!is_valid_username(username))
	{
		cout << username << " is not a valid username." << endl;
		cout << "Username must be between one and twenty characters long." << endl;
		fprintf(stderr, "usage %s username address port\n", argv[0]);
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
	int exit_code = getaddrinfo(argv[2], argv[3], &hints, &address_candidates);
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

	MsgIdManager msg_id_manager;

	// buffer[strlen(buffer) - 1] = 0; // Remove new line and ensure termination.

	ClientMsgType msg_type = ClientMsgType::Login;
	int id = msg_id_manager.nextId();
	ClientMsgPayload payload;
	strcpy(payload.username, username.c_str());

	ClientMsg login_request{id, msg_type, payload};
	string json_encoded = login_request.serialize();

	write_from_buffer(socket_descriptor, json_encoded.c_str());

	read_to_buffer(socket_descriptor, buffer);
	Json::Reader reader;
	Json::Value messageValue;
	bool parseSuccess = reader.parse(buffer, messageValue, false);
	if (!parseSuccess)
	{
		cout << "ERROR parsing message while connecting: " << messageValue << endl;
		exit(EXIT_FAILURE);
	}
	int server_msg_id = messageValue["id"].asInt();
	ServerMsgType server_msg_type = static_cast<ServerMsgType>(messageValue["type"].asInt());
	switch (server_msg_type)
	{
	case ServerMsgType::LoginFail:
	{
		printf("Login failure.\n");
		exit(EXIT_FAILURE);
		break;
	}
	case ServerMsgType::LoginSuccess:
	{
		printf("Login successful.\n");
		break;
	}
	default:
	{
		cout << "Invalid server message type received while connecting: " << server_msg_type << endl;
		exit(EXIT_FAILURE);
		break;
	}
	}

	pthread_t receiver;
	ReceiverSystem receiver_system = {socket_descriptor};
	pthread_create(&receiver, NULL, fn_receiver, &receiver_system);

	while (TRUE)
	{
		cout << endl
			 << "Type one of the following commands:" << endl;
		cout << "FOLLOW @username" << endl;
		cout << "SEND message" << endl;
		cout << "QUIT" << endl
			 << endl;

		bzero(buffer, BUFFER_SIZE);
		fgets(buffer, BUFFER_SIZE, stdin);

		if (strlen(buffer) < 4)
		{
			cout << "Malformed command: " << buffer << endl;
			continue;
		}

		string buffer_string(buffer);
		buffer_string.pop_back(); // Remove new line.
		string command = buffer_string.substr(0, 4);

		if (!command.compare("QUIT"))
		{
			exit(EXIT_SUCCESS);
		}

		if (buffer_string.size() < 6)
		{
			cout << "Malformed command: " << buffer_string << endl;
			continue;
		}

		if (!command.compare("SEND"))
		{
			if (buffer_string[4] != ' ')
			{
				cout << "Malformed command: " << buffer_string << endl;
				continue;
			}
			string message = buffer_string.substr(5);
			if (!is_valid_message(message))
			{
				cout << "Invalid message: " << message << endl;
				continue;
			}

			ClientMsgType msg_type = ClientMsgType::ClientMessage;
			int id = msg_id_manager.nextId();
			ClientMsgPayload payload;
			strcpy(payload.message, message.c_str());

			ClientMsg message_packet{id, msg_type, payload};
			string json_encoded = message_packet.serialize();

			write_from_buffer(socket_descriptor, json_encoded.c_str());
			cout << "Sent message: " << message << endl;
			continue;
		}

		if (buffer_string.size() < 9)
		{
			cout << "Malformed command: " << buffer_string << endl;
			continue;
		}

		command = buffer_string.substr(0, 8);
		if (!command.compare("FOLLOW @"))
		{
			string follow_username = buffer_string.substr(8);
			if (!is_valid_username(follow_username))
			{
				cout << "Invalid username: " << follow_username << endl;
				continue;
			}

			ClientMsgType msg_type = ClientMsgType::Follow;
			int id = msg_id_manager.nextId();
			ClientMsgPayload payload;
			strcpy(payload.username, follow_username.c_str());

			ClientMsg message_packet{id, msg_type, payload};
			string json_encoded = message_packet.serialize();

			write_from_buffer(socket_descriptor, json_encoded.c_str());
			cout << "Follow requested: " << follow_username << endl;
			continue;
		}

		cout << "Malformed command: " << buffer_string << endl;
		continue;

		// write_from_buffer(socket_descriptor, buffer);

		// read_to_buffer(socket_descriptor, buffer);

		// printf("Received %d bytes: %s\n", number_of_bytes, buffer);
	}

	close(socket_descriptor);
	return 0;
}
