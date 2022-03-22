main:
	make server
	make client

extlibs:
	g++ -c src/libs/jsoncpp/jsoncpp.cpp -o libs.o

shared:
	g++ -c src/shared/shared_helpers.cpp
	g++ -c src/shared/client_msg.cpp
	g++ -c src/shared/server_msg.cpp

server:
	make extlibs
	make shared
	g++ -c src/server/server.cpp
	g++ -c src/server/user_manager.cpp
	g++ -c src/server/hashing.cpp
	g++ -o app_servidor libs.o shared_helpers.o client_msg.o server_msg.o hashing.o user_manager.o server.o
	make clean_partial

client:
	make extlibs
	make shared
	g++ -c src/client/client_helpers.cpp
	g++ -c src/client/login.cpp
	g++ -c src/client/user_input.cpp
	g++ -c src/client/client_receiver.cpp
	g++ -c src/client/client_sender.cpp
	g++ -c src/client/client.cpp
	g++ -o app_cliente libs.o shared_helpers.o client_msg.o server_msg.o login.o user_input.o client_helpers.o client_receiver.o client_sender.o client.o -lrt -pthread
	make clean_partial

clean_partial:
	rm -f server.o client.o libs.o shared_helpers.o client_msg.o server_msg.o user_manager.o hashing.o login.o user_input.o client_helpers.o client_receiver.o client_sender.o

clean:
	make clean_partial
	rm -f app_servidor app_cliente

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
