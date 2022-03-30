// Multithreading
#include <pthread.h>
#include <semaphore.h>

// Signal interruption
#include <signal.h>

// C++
#include <iostream>
#include <string>

#include "../libs/jsoncpp/json/json.h"
#include "../shared/shared.h"
#include "client_connection.h"
#include "client_receiver.h"
#include "client_sender.h"
#include "user_input.h"
#include "login.h"

using namespace std;

volatile sig_atomic_t interruption_flag = FALSE;
void interruption_treatment(int signal)
{
	interruption_flag = TRUE;
}

int main(int argc, char *argv[])
{
	signal(SIGINT, interruption_treatment);

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

	ConnectionManager *connection_manager = connect_to_address_port(server_address, server_port);
	if (connection_manager == NULL)
	{
		cout << "Failed to connect to server." << endl
				 << "Server address: " << server_address << endl
				 << "Server port: " << server_port << endl
				 << endl;
		exit(EXIT_FAILURE);
	}

	{ // Login
		bool login_success = try_login(ClientMsgType::ClientLogin, &username, connection_manager, buffer);
		if (!login_success)
		{
			cout << "Failed to login." << endl
					 << "Username: " << username << endl
					 << endl;
			exit(EXIT_FAILURE);
		}
	}

	ClientReceiver *client_receiver = new ClientReceiver{
			connection_manager,
			AtomicVecQueue<Json::Value>{},
	};

	ClientSender *client_sender = new ClientSender{
			connection_manager,
			AtomicVecQueue<ClientMessageData>{},
	};

	UserInputManager *user_input_manager = new UserInputManager{
			AtomicVar<UserInput>(
					UserInput{
							UserInputType::NoInput})};

	pthread_t receiver_thread;
	pthread_t sender_thread;
	pthread_t input_thread;

	pthread_create(&receiver_thread, NULL, fn_client_listener, client_receiver);
	pthread_create(&sender_thread, NULL, fn_client_sender, client_sender);
	pthread_create(&input_thread, NULL, fn_user_input, user_input_manager);

	MsgIdManager msg_id_manager;

	bool finish_program = false;

	for (;;)
	{
		if (finish_program || interruption_flag)
		{
			break; // Break and clean-up.
		}

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

				ClientMsgType msg_type = ClientMsgType::ClientFollow;
				msg_id_t msg_id = msg_id_manager.next_msg_id();
				ClientMsgPayload payload;
				strcpy(payload.username, username);
				ClientMessageData follow_request{msg_id, msg_type, payload};

				client_sender->send_queue.push(follow_request);
#ifdef DEBUG
				cout << "main() processed InputFollow" << endl
						 << endl;
#endif
				break;
			}
			case UserInputType::InputSend:
			{
				char message[128];
				strcpy(message, user_input.input_data.message);
				user_input_manager->user_command.locked_write(UserInput{UserInputType::NoInput});
				user_input_manager->user_command.unlock();

				ClientMsgType msg_type = ClientMsgType::ClientSend;
				msg_id_t msg_id = msg_id_manager.next_msg_id();
				ClientMsgPayload payload;
				strcpy(payload.message, message);
				ClientMessageData send_request{msg_id, msg_type, payload};

				client_sender->send_queue.push(send_request);
#ifdef DEBUG
				cout << "main() processed InputSend" << endl
						 << endl;
#endif
				break;
			}
			case UserInputType::InputQuit:
			{
				finish_program = true;
				break;
			}
			default:
			{
#ifdef DEBUG
				cout << "Error while processing user_input.input_type in main()" << endl
						 << "Invalid value: " << user_input.input_type << endl
						 << endl;
#endif
				user_input_manager->user_command.unlock();
			}
			}
		}
		{ // Handle server input.
			vector<Json::Value> server_messages_input_queue = client_receiver->receive_queue.drain();

			for (const Json::Value message_value : server_messages_input_queue)
			{
				msg_id_t server_msg_id = message_value["msg_id"].asInt();
				ServerMsgType server_msg_type = static_cast<ServerMsgType>(message_value["msg_type"].asInt());
				switch (server_msg_type)
				{
				case ServerMsgType::ServerSendCommand:
				{
#ifdef DEBUG
					cout << "Received message: " << endl;
#endif
					string username = message_value["username"].asString();
#ifdef DEBUG
					cout << "From: @" << username << endl;
#endif
					string body = message_value["body"].asString();
#ifdef DEBUG
					cout << "Message: " << body << " " <<  message_value["time"].asString() << endl
							 << endl;
#endif
					break;
				}
				case ServerMsgType::FollowCommandFail:
				{
#ifdef DEBUG
					cout << "Follow command rejected for user: " << message_value["username"].asString() << endl
							 << endl;
#endif
					break;
				}
				case ServerMsgType::FollowCommandSuccess:
				{
#ifdef DEBUG
					cout << "Follow command successful for user: " << message_value["username"].asString() << endl
							 << endl;
#endif
					break;
				}
				case ServerMsgType::SendCommandFail:
				{
#ifdef DEBUG
					cout << "Send command rejected." << endl
							 << endl;
#endif
					break;
				}
#ifdef DEBUG
				case ServerMsgType::SendCommandSuccess:
				{
					cout << "Send command successful: " << endl
							 << endl;
#endif
					break;
				}
				default:
				{
#ifdef DEBUG
					cout << "Invalid server message type received on fn_client_listener: " << server_msg_type << endl;
#endif
					finish_program = true;
					break;
				}
				}
			}
		}
	}

	{ // Clean-up before leaving.
		user_input_manager->must_terminate.write(true);
		client_receiver->must_terminate.write(true);

		ClientMsgType msg_type = ClientMsgType::ClientLogout;
		msg_id_t msg_id = msg_id_manager.next_msg_id();
		ClientMsgPayload payload;
		ClientMessageData send_request{msg_id, msg_type, payload};

		client_sender->send_queue.push(send_request);
		pthread_join(input_thread, NULL);
		pthread_join(sender_thread, NULL);
	}

	connection_manager->close_socket();

	return EXIT_SUCCESS;
}
