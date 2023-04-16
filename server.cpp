
#include <Winsock2.h>
//#include <windows.h>
#include <string>
#include <Ws2tcpip.h>
#include "request_data.h" // DEPRECATED, DOES NOTHING
#include "threads.h"
#include "misc.h"
#pragma comment(lib,"ws2_32.lib")

#define uCLIENT_PORT 5288


namespace server {

	static SOCKET sock_input, sock_output;
	static struct sockaddr_in server, client;

	
	static char buffer[1024];
	u_short port = 348;

	void ulisten() {

		while (!shouldClose) {

			int end_of_response = 0;
			if ((end_of_response = recv(sock_input, buffer, 1023, 0)) == SOCKET_ERROR) {
				LOG("server", "Failed to receive information")
				exit(-1);
			}

			size_t nlchar_index = strcspn(buffer, "\n");
			if (nlchar_index != strlen(buffer))
				buffer[nlchar_index] = ' ';

			buffer[end_of_response] = '\0';
			LOG2("server", "New request(%s) received!", buffer);

			parse(buffer);

		}

	}

	/* Sends a message to THE client. Client port -- 5288 */
	void send(const char* message, int len) {
		LOG("server", "Sending message to client!");
		int result = sendto(sock_output, message, len, 0, (SOCKADDR*)& client, sizeof(client));
		
		if (result == SOCKET_ERROR) 
			LOG2("server", "Failed to send data. Error Code: %d", WSAGetLastError())
		
	}

	void send(const char* message) {
		send(message, (int) strlen(message));
	}

	void uclose() {
		closesocket(sock_input);
		closesocket(sock_output);
		WSACleanup();

	}

#define CLOSE_SERVER(MESSAGE, CODE)			\
		{									\
			closesocket(sock_input);		\
			closesocket(sock_output);		\
			WSACleanup();					\
			LOG2("server", MESSAGE, CODE);  \
			exit(-1);						\
		}


	void init() {

		WSADATA wsa;

		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
			CLOSE_SERVER("Failed. Error Code : %d\n", WSAGetLastError());

		/*{
			LOG2("server", "Failed. Error Code : %d\n", WSAGetLastError());
			uclose();
			exit(-1);
		}*/

		if ((sock_input = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
			CLOSE_SERVER("Failed to create input socket. Erorr Code: %d\n", WSAGetLastError());
		
		//{
		//	LOG2("server", "Failed to create input socket. Erorr Code: %d\n", WSAGetLastError());
		//	uclose();
		//	exit(-1);
		//}

		if ((sock_output = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
			CLOSE_SERVER("Failed to create output socket. Erorr Code: %d\n", WSAGetLastError());
		//{
		//	LOG2("server", "Failed to create output socket. Erorr Code: %d\n", WSAGetLastError());
		//	uclose();
		//	exit(-1);
		//}


		InetPton(AF_INET, L"127.0.0.1", &server.sin_addr.s_addr);
		server.sin_family = AF_INET;
		server.sin_port = htons(port);

		InetPton(AF_INET, L"127.0.0.1", &client.sin_addr.s_addr);
		client.sin_family = AF_INET;
		client.sin_port = htons(uCLIENT_PORT);

		if (bind(sock_input, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
			CLOSE_SERVER("Failed to bind the server socket%d\n", -1);
		
		/*{
			LOG("server", "Failed to bind the server socket\n");
			uclose();
			exit(-1);
		}*/

		LOG2("server", "Started server on port: %u", port);

		ulisten();

		uclose();


	}
}