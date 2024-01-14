CFLAGS = -lws2_32 -lwinmm

all: server client

server: server.c
	gcc -o server enet/*.c server.c $(CFLAGS)

client: client.c
	gcc -o client enet/*.c client.c $(CFLAGS)

clean:
	rm -vf server.exe client.exe

.PHONY: all clean