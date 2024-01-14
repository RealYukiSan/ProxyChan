#include <ws2tcpip.h>
#include <string.h>
#include <stdio.h>
#include "enet/include/enet.h"

int main(void)
{
	ENetAddress server_addr;
	ENetHost *server;
	ENetEvent server_event;

	enet_initialize();

	memset(&server_addr, 0, sizeof(server_addr));
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.host = 0;
	server_addr.port = 6969;
	server = enet_host_create(&server_addr, 2, 2, 0, 0);

	printf("[SERVER] Uncle SAM Listening...\n");

	for (;;) {
		while(enet_host_service(server, &server_event, 5) > 0) {
			switch (server_event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				printf("New cliento connected desu\n");
				printf("%x:%u", server_event.peer->address.host, server_event.peer->address.port);
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				printf("a packet of length %u bytes, containing %s was received from %s on channel %u.\n",
					server_event.packet->dataLength,
					server_event.packet->data,
					server_event.peer->data,
					server_event.channelID
				);
				enet_packet_destroy(server_event.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				printf("%x:%u", server_event.peer->address.host, server_event.peer->address.port);
				printf("The client disconnected desu\n");
				server_event.peer->data = NULL;
				break;
			}
		}
	}

	enet_host_destroy(server);
	enet_deinitialize();

	return 0;
}
