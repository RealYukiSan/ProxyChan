#include <ws2tcpip.h>
#include <string.h>
#include <stdio.h>
#include "enet/include/enet.h"
#include "https.c"

int main(void)
{
	ENetHost *proxy_client;
	ENetPeer *peer;
	ENetEvent client_event;
	ENetEvent proxy_client_event;
	ENetAddress gt_server;
	struct EnetGrowtopia enetGrowtopia;

	enet_initialize();
	request_enet_addr(&enetGrowtopia);
	printf("%s\n", enetGrowtopia.ip_string);
	inet_pton(AF_INET, enetGrowtopia.ip_string, &gt_server.host);
	gt_server.port = enetGrowtopia.port;
	proxy_client = enet_host_create(NULL, 1, 2, 0, 0);
	proxy_client->checksum = enet_crc32;
	proxy_client->usingNewPacket = 1;
	enet_host_compress_with_range_coder(proxy_client);
	peer = enet_host_connect(proxy_client, &gt_server, 2, 0);

	if (!peer) return EXIT_FAILURE;
	
	for (;;) {
		if (enet_host_service(proxy_client, &client_event, 5) > 0) {
			switch (client_event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				puts("connected");
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				puts("disconnected");
				break;
			}
		}
	}

	enet_host_destroy(proxy_client);
	enet_deinitialize();

	return 0;
}
