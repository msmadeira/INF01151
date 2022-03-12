main:
	make server
	make client

server:
	g++ -c src/server/server.cpp
	g++ -o app_servidor server.o
	make clean_partial

client:
	g++ -c src/client/client.cpp
	g++ -o app_cliente client.o
	make clean_partial

clean_partial:
	rm -f server.o client.o

clean:
	make clean_partial
	rm -f app_servidor app_cliente

runser:
	make server
	./app_servidor

runcli:
	make client
	./app_cliente 127.0.0.1
