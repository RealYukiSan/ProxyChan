#include <stdio.h>
#include "enet/include/enet.h"
#include <processthreadsapi.h>

int main(void)
{
	ENetAddress server_addr;
	ENetPacket *pkt;
	ENetPeer *peer;

	ENetHost *client;
	ENetEvent client_event;

	enet_initialize();
	enet_address_set_host(&server_addr, "127.0.0.1");

	server_addr.port = 6969;
	client = enet_host_create(0, 1, 2, 0, 0);
	peer = enet_host_connect(client, &server_addr, 2, 0);

	for (;;) {
		if (peer == NULL) {
			fprintf(stderr, "No avalaible peers to initiating an Enet connection");
			exit(EXIT_FAILURE);
		}

		if (enet_host_service(client, &client_event, 5) > 0) {
			switch (client_event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				printf("Yatta!\n");
				
				break;
			}
		}

		pkt = enet_packet_create("Woshhh~", strlen("Woshhh~") + 1, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, pkt);
	}

	enet_host_destroy(client);
	enet_deinitialize();

	return 0;
}