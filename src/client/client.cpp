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

// Multithreading
#include <pthread.h>
#include <semaphore.h>

// C++
#include <iostream>

#include "../libs/jsoncpp/json/json.h"
#include "../shared/shared.h"
#include "client_helpers.h"
#include "client_receiver.h"
#include "client_sender.h"
#include "user_input.h"

using namespace std;

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
		cout << "Username must be between four and twenty characters long." << endl;
		fprintf(stderr, "usage %s username address port\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (argc > 4)
	{
		cout << "WARNING, correct usage: " << argv[0] << " username address port" << endl
			 << "Extra passed arguments ignored." << endl
			 << endl;
	}

	string server_address(argv[2]);
	string server_port(argv[3]);

	ClientReceiver client_receiver = {
		&username,
		&server_address,
		&server_port,
		AtomicVecQueue<Json::Value>{},
	};

	ClientSender client_sender = {
		&username,
		&server_address,
		&server_port,
		AtomicVecQueue<ClientMsg>{},
	};

	UserInputManager *user_input_manager = new UserInputManager{
		AtomicVar<UserInput>(
			UserInput{
				UserInputType::NoInput})};

	pthread_t receiver_thread,
		sender_thread, input_thread;
	pthread_create(&receiver_thread, NULL, fn_client_listener, &client_receiver);
	pthread_create(&sender_thread, NULL, fn_client_sender, &client_sender);
	pthread_create(&input_thread, NULL, fn_user_input, user_input_manager);

	MsgIdManager msg_id_manager;

	for (;;)
	{
		{ // Handle user input.
			user_input_manager->user_command.lock();
			UserInput user_input = user_input_manager->user_command.locked_read();
			switch (user_input.input_type)
			{
			case UserInputType::NoInput:
			{
				user_input_manager->user_command.unlock();
				break;
			}
			case UserInputType::InputFollow:
			{
				char username[20];
				strcpy(username, user_input.input_data.username);
				user_input_manager->user_command.locked_write(UserInput{UserInputType::NoInput});
				user_input_manager->user_command.unlock();

				ClientMsgType msg_type = ClientMsgType::Follow;
				int id = msg_id_manager.nextId();
				ClientMsgPayload payload;
				strcpy(payload.username, username);
				ClientMsg follow_request{id, msg_type, payload};

				client_sender.send_queue.push(follow_request);
				cout << "main() processed InputFollow" << endl
					 << endl;
				break;
			}
			case UserInputType::InputSend:
			{
				char message[128];
				strcpy(message, user_input.input_data.message);
				user_input_manager->user_command.locked_write(UserInput{UserInputType::NoInput});
				user_input_manager->user_command.unlock();

				ClientMsgType msg_type = ClientMsgType::ClientSend;
				int id = msg_id_manager.nextId();
				ClientMsgPayload payload;
				strcpy(payload.message, message);
				ClientMsg send_request{id, msg_type, payload};

				client_sender.send_queue.push(send_request);
				cout << "main() processed InputSend" << endl
					 << endl;
				break;
			}
			case UserInputType::InputQuit:
			{
				// TO DO: Handle elegant closing-up.
				exit(EXIT_SUCCESS);
			}
			default:
			{
				cout << "Error while processing user_input.input_type in main()" << endl
					 << "Invalid value: " << user_input.input_type << endl
					 << endl;
				user_input_manager->user_command.unlock();
			}
			}
		}
		{ // Handle server input.
			vector<Json::Value> server_input_queue = client_receiver.receive_queue.drain();

			for (const Json::Value message_value : server_input_queue)
			{
				int server_msg_id = message_value["id"].asInt();
				ServerMsgType server_msg_type = static_cast<ServerMsgType>(message_value["type"].asInt());
				switch (server_msg_type)
				{
				case ServerMsgType::ServerMessage:
				{
					cout << "Received message: " << endl;
					string username = message_value["username"].asString();
					cout << "From: @" << username << endl;
					string body = message_value["body"].asString();
					cout << "Message: " << body << endl
						 << endl;
					break;
				}
				default:
				{
					cout << "Invalid server message type received on fn_client_listener: " << server_msg_type << endl;
					exit(EXIT_FAILURE);
					break;
				}
				}
			}
		}
	}

	// close(socket_descriptor);
	return EXIT_SUCCESS;
}
