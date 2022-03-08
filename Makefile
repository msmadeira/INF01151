main:
	make server
	make client

server:
	gcc -c src/server.c
	gcc -o server server.o
	make clean_partial

client:
	gcc -c src/client.c
	gcc -o client client.o
	make clean_partial

clean_partial:
	rm -f server.o client.o

clean:
	make clean_partial
	rm -f server client

runser:
	make server
	./server

runcli:
	make client
	./client 127.0.0.1
