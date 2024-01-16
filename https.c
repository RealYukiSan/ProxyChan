#include <string.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "tlse/tlse.c"
#include "enet/include/enet.h"

#ifndef __packed
#define __packed __attribute__((__packed__))
#endif

#ifndef __aligned
#define __aligned(X) __attribute__((__aligned__(X)))
#endif

#define HTTP_SERVER 0x0211c6a2 // 2.17.198.162
#define PORT_SERVER 443

struct EnetGrowtopia {
	unsigned short port;
	char ip_string[16];
};

int send_pending(int client_sock, struct TLSContext *context) {
	unsigned int out_buffer_len = 0;
	const unsigned char *out_buffer = tls_get_write_buffer(context, &out_buffer_len);
	unsigned int out_buffer_index = 0;
	int send_res = 0;
	while ((out_buffer) && (out_buffer_len > 0)) {
		int res = send(client_sock, (char *)&out_buffer[out_buffer_index], out_buffer_len, 0);
		if (res <= 0) {
			send_res = res;
			break;
		}
		out_buffer_len -= res;
		out_buffer_index += res;
	}
	tls_buffer_clear(context);
	return send_res;
}

void request_enet_addr(struct EnetGrowtopia *enetGrowtopia)
{
	WSADATA wsaData;

	int sockfd;
	unsigned int ip_addr = htonl(HTTP_SERVER);
	struct sockaddr_in server_addr;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == INVALID_SOCKET)
		abort();

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(443);
	memcpy(&server_addr.sin_addr, &ip_addr, sizeof(ip_addr));

	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
		abort();

	struct TLSContext *context = tls_create_context(0, TLS_V12);

	tls_client_connect(context);
	send_pending(sockfd, context);

	unsigned char client_message[0xFFFF];
	int sent = 0;
	int read_size;
	while ((read_size = recv(sockfd, client_message, sizeof(client_message) , 0)) > 0) {
		tls_consume_stream(context, client_message, read_size, NULL);
		send_pending(sockfd, context);
		if (tls_established(context)) {
			if (!sent) {
				const char *request = "POST /growtopia/server_data.php HTTP/1.1\r\nUser-Agent: UbiServices_SDK_2019.Release.27_PC64_unicode_static\r\nHost: www.growtopia1.com\r\nAccept: */*\r\nConnection: close\r\nContent-Length: 0\r\n\r\n";
				tls_write(context, (unsigned char *)request, strlen(request));
				send_pending(sockfd, context);
				sent = 1;
			}

			char read_buffer[0xFFFF];
			int read_size = tls_read(context, read_buffer, 0xFFFF - 1);
			if (read_size > 0) {
				char *content;
				char *terminator;
				content = strstr(read_buffer, "server");
				while (1) {
					if (*content == '|') {
						content++;
						terminator = content;
						while (1) {
							if (*terminator == '\n') {
								*terminator = '\0';
								break;
							}
							terminator++;
						}
						strcpy(&enetGrowtopia->ip_string, content);
						content = ++terminator;
						break;
					}
					content++;
				}
				while (1) {
					if (*content == '|') {
						content++;
						terminator = content;
						while (1) {
							if (*terminator == '\n') {
								*terminator = '\0';
								break;
							}
							terminator++;
						}
						enetGrowtopia->port = atoi(content);
						content = ++terminator;
						break;
					}
					content++;
				}
			}
		}
	}
}