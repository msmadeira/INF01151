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

// Signal interruption
#include <signal.h>

// libs
#include "../libs/jsoncpp/json/json.h"

#include "../shared/shared.h"
#include "hashing.h"
#include "server_broadcast.h"
#include "client_message_handler.h"
#include "server_receiver.h"
#include "user_persistence.h"
#include "user_connection.h"
#include "server_sender.h"
#include "server_message.h"
#include "disk_operations.h"

using namespace std;

volatile sig_atomic_t interruption_flag = FALSE;
void interruption_treatment(int signal)
{
	interruption_flag = TRUE;
}

int main(int argc, char *argv[])
{
	signal(SIGINT, interruption_treatment);

	socket_t socket_descriptor = fn_server_broadcast();
	if (socket_descriptor == INVALID_SOCKET)
	{
		cout << "Error: Unable to setup server broadcast." << endl
			 << endl;
		return EXIT_FAILURE;
	}

	DiskOperationsManagment *disk_managment = new DiskOperationsManagment();

	UserPersistence *user_persistence = load_user_persistence(disk_managment);
	if (user_persistence == NULL)
	{
#ifdef DEBUG
		cout << "Error: Unable to setup user persistence." << endl
			 << endl;
#endif
		return EXIT_FAILURE;
	}
	ServerReceiver *server_receiver = new ServerReceiver{socket_descriptor};
	ServerSender *server_sender = new ServerSender{socket_descriptor};

	pthread_t receiver_thread;
	pthread_t sender_thread;
	pthread_t disk_io_thread;

	pthread_create(&receiver_thread, NULL, fn_server_listener, server_receiver);
	pthread_create(&sender_thread, NULL, fn_server_sender, server_sender);
	pthread_create(&disk_io_thread, NULL, fn_disk_io, disk_managment);

	UserConnectionManager *connection_manager = new UserConnectionManager{};
	NotificationManager *notification_manager = new NotificationManager{user_persistence, connection_manager};

	ClientMessageHandler client_message_handler{user_persistence, connection_manager, notification_manager};
	vector<ServerAction> pending_actions;

	for (;;)
	{
		if (interruption_flag)
		{
			break; // Break and clean-up.
		}

		vector<ValueAddressTuple> client_messages_input_queue = server_receiver->receive_queue.drain();

		for (const ValueAddressTuple message_tuple : client_messages_input_queue)
		{ // Handle incoming client messages.

			client_message_handler.handle_incoming_datagram(&pending_actions, message_tuple.value, message_tuple.address);
			if (pending_actions.size() == 0)
			{
				continue;
			}
			for (const ServerAction server_action : pending_actions)
			{
				switch (server_action.action_type)
				{
				case ServerActionType::ActionMessageUser:
				{
					sockaddr_in address = connection_manager->get_address_from_user(server_action.action_data.message_user.user_id);
					server_sender->send_queue.push(
						ServerMessage{
							address,
							server_action.action_data.message_user.message});
					break;
				}
				case ServerActionType::ActionMessageAddress:
				{
					server_sender->send_queue.push(
						ServerMessage{
							server_action.action_data.message_address.address,
							server_action.action_data.message_address.message});
					break;
				}
				default:
				{
#ifdef DEBUG
					cout << "client_message_handler.handle_incoming_datagram() produced invalid action_type: " << server_action.action_type << endl
						 << endl;
#endif
					break;
				}
				}
			}
			pending_actions.clear();
		}
	}

	{ // Clean-up before leaving.
		server_receiver->must_terminate.write(true);
		server_sender->must_terminate.write(true);
		disk_managment->must_terminate.write(true);

		pthread_join(sender_thread, NULL);
		pthread_join(disk_io_thread, NULL);
	}

	close(socket_descriptor);
	return EXIT_SUCCESS;
}
