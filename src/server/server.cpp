// C
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <unordered_map>

// libs
#include "../libs/jsoncpp/json/json.h"

#include "../shared/shared.h"
#include "hashing.h"
#include "user_manager.h"

using namespace std;

class ServerManager
{
public:
	int socket_descriptor;
	MsgIdManager msg_id_manager;
	UserManager user_manager;

	ServerManager(int socket_descriptor) : socket_descriptor(socket_descriptor){};

	void send_to_client(const char *buffer, struct sockaddr *client_address)
	{
		int number_of_bytes = sendto(
			this->socket_descriptor,
			buffer,
			strlen(buffer),
			NONE,
			client_address,
			sizeof(struct sockaddr));
		if (number_of_bytes < 0)
			printf("ERROR on sendto");
	}

	void handle_follow(Json::Value messageValue, struct sockaddr_in client_address)
	{
		string username = messageValue["username"].asString();
		this->user_manager.add_follow(username, client_address);
	}

	void handle_message(Json::Value messageValue, struct sockaddr_in client_address)
	{
		string message = messageValue["message"].asString();
		if (!is_valid_message(message))
		{
			return;
		}

		auto response_type = ServerMsgType::ServerMessage;

		User *sender_user = user_manager.get_user_by_address(client_address);

		ServerMsgPayload payload;
		strcpy(payload.message.username, sender_user->username.c_str());
		strcpy(payload.message.body, message.c_str());

		cout << "Sending message." << endl
			 << "Sender: " << sender_user->username << endl
			 << "Message: " << message << endl;

		for (const auto receiver_user_id : sender_user->followed_by)
		{
			User *receiver_user = user_manager.get_user_by_user_id(receiver_user_id);
			int msg_id = user_manager.get_next_msg_id(receiver_user_id);

			ServerMsg server_message{msg_id, response_type, payload};
			string json_encoded = server_message.serialize();

			send_to_client(json_encoded.c_str(), (struct sockaddr *)&(receiver_user->address));

			cout << "Recipient: " << receiver_user->username << endl;
		}
		cout << endl;
	}

	void handle_login(Json::Value messageValue, struct sockaddr_in client_address)
	{
		int msg_id;
		string username = messageValue["username"].asString();
		ServerMsgType response_type;
		if (!is_valid_username(username))
		{
			cout << "Login failure: " << username << endl;
			response_type = ServerMsgType::LoginFail;
			msg_id = 0;
		}
		else
		{
			cout << "Login success: " << username << endl;
			response_type = ServerMsgType::LoginSuccess;
			int user_id = user_manager.add_or_update_user(username, client_address);
			msg_id = user_manager.get_next_msg_id(user_id);
		}
		cout << endl;

		ServerMsg response{msg_id, response_type};
		string json_encoded = response.serialize();
		this->send_to_client(json_encoded.c_str(), (struct sockaddr *)&client_address);
	}

	void handle_incoming_datagram(const char *buffer, struct sockaddr_in client_address)
	{
		Json::Reader reader;
		Json::Value messageValue;

		bool parseSuccess = reader.parse(buffer, messageValue, false);

		if (!parseSuccess)
		{
			printf("ERROR parsing message");
			return;
		}

		int client_msg_id = messageValue["id"].asInt();
		ClientMsgType client_msg_type = static_cast<ClientMsgType>(messageValue["type"].asInt());

		switch (client_msg_type)
		{
		case ClientMsgType::Login:
		{
			this->handle_login(messageValue, client_address);
			break;
		}
		case ClientMsgType::ClientMessage:
		{
			this->handle_message(messageValue, client_address);
			break;
		}
		case ClientMsgType::Follow:
		{
			this->handle_follow(messageValue, client_address);
			break;
		}
		}
		printf("Received JSON: %s\n", buffer);
	}
};

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
	if (exit_code != EXIT_SUCCESS)
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

	ServerManager server_manager(socket_descriptor);

	for (;;)
	{
		/* receive from socket */
		memset(buffer, 0, sizeof(buffer));
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
		if (exit_code == EXIT_SUCCESS)
			printf("Received %d bytes from %s:%s\n", number_of_bytes, host, service);
		else
			fprintf(stderr, "getnameinfo: %s\n", gai_strerror(exit_code));

		server_manager.handle_incoming_datagram(buffer, client_address);

		// send_to_client(socket_descriptor, "Got your message\n", (struct sockaddr *)&client_address);
	}

	close(socket_descriptor);
	return EXIT_SUCCESS;
}
