main:
	make server
	make client

extlibs:
	g++ -c src/libs/jsoncpp/jsoncpp.cpp -o libs.o

shared:
	g++ -c src/shared/shared.cpp

server:
	make extlibs
	make shared
	g++ -c src/server/server.cpp
	g++ -c src/server/user_manager.cpp
	g++ -c src/server/hashing.cpp
	g++ -o app_servidor libs.o shared.o hashing.o user_manager.o server.o
	make clean_partial

client:
	make extlibs
	make shared
	g++ -c src/client/client.cpp
	g++ -o app_cliente libs.o shared.o client.o -lrt -pthread
	make clean_partial

clean_partial:
	rm -f server.o client.o libs.o shared.o user_manager.o hashing.o

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
