main:
	make server
	make client

extlibs:
	g++ -c src/libs/jsoncpp/jsoncpp.cpp -o libs.o

shared:
	g++ -c src/shared/msg_id.cpp
	g++ -c src/shared/shared_helpers.cpp
	g++ -c src/shared/client_message.cpp

server:
	make extlibs
	make shared
	g++ -c src/server/hashing.cpp
	g++ -c src/server/notification.cpp
	g++ -c src/server/disk_operations.cpp
	g++ -c src/server/server_message.cpp
	g++ -c src/server/server_sender.cpp
	g++ -c src/server/client_message_handler.cpp
	g++ -c src/server/server_broadcast.cpp
	g++ -c src/server/user_persistence.cpp
	g++ -c src/server/user_connection.cpp
	g++ -c src/server/server_receiver.cpp
	g++ -c src/server/server.cpp
	g++ -o app_servidor libs.o msg_id.o shared_helpers.o disk_operations.o server_message.o client_message.o server_sender.o hashing.o notification.o client_message_handler.o server_broadcast.o user_persistence.o user_connection.o server_receiver.o server.o -lrt -pthread
	make clean_partial

client:
	make extlibs
	make shared
	g++ -c src/client/client_connection.cpp
	g++ -c src/client/login.cpp
	g++ -c src/client/user_input.cpp
	g++ -c src/client/client_receiver.cpp
	g++ -c src/client/client_sender.cpp
	g++ -c src/client/client.cpp
	g++ -o app_cliente libs.o msg_id.o shared_helpers.o client_message.o login.o user_input.o client_connection.o client_receiver.o client_sender.o client.o -lrt -pthread
	make clean_partial

clean_partial:
	rm -f server.o client.o libs.o msg_id.o shared_helpers.o disk_operations.o server_message.o client_message.o server_sender.o client_message_handler.o server_broadcast.o hashing.o notification.o login.o user_input.o client_connection.o client_receiver.o client_sender.o user_persistence.o user_connection.o server_receiver.o

clean:
	make clean_partial
	rm -f app_servidor app_cliente

runclient1:
	./app_cliente teste1 127.0.0.1 4000

runclient2:
	./app_cliente teste2 127.0.0.1 4000

runser:
	make server
	./app_servidor

runcli1:
	make client
	./app_cliente teste1 127.0.0.1 4000

runcli2:
	make client
	./app_cliente teste2 127.0.0.1 4000

runcli3:
	make client
	./app_cliente teste3 127.0.0.1 4000
