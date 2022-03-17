main:
	make server
	make client

extlibs:
	g++ -c src/libs/jsoncpp/jsoncpp.cpp -o libs.o

server:
	make extlibs
	g++ -c src/server/server.cpp
	g++ -o app_servidor libs.o server.o
	make clean_partial

client:
	make extlibs
	g++ -c src/client/client.cpp
	g++ -o app_cliente libs.o client.o
	make clean_partial

clean_partial:
	rm -f server.o client.o libs.o

clean:
	make clean_partial
	rm -f app_servidor app_cliente

runser:
	make server
	./app_servidor

runcli:
	make client
	./app_cliente 127.0.0.1
