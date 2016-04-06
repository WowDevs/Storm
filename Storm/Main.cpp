#include <stdio.h>
#include "Net.h"

int main()
{
	WSADATA wsaData;
	int status;

	// Initializing Winsock
	status = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (status != 0)
	{
		printf("WSAStartup failed: %d\n", status);
		return 1;
	}

	struct addrinfo *result = NULL, *ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server instance
	status = getaddrinfo(NULL, "3724", &hints, &result);
	if (status != 0)
	{
		printf("getaddrinfo failed: %d\n", status);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for the server to listen for client connections
	SOCKET listenSocket = INVALID_SOCKET;
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listenSocket == INVALID_SOCKET)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	status = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (status == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	SOCKET clientSocket;
	clientSocket = INVALID_SOCKET;

	// Accept a client socket
LABEL_ACCEPT:
	clientSocket = accept(listenSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET)
	{
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	char recvbuf[512];
	int sendResult;
	int recvbuflen = 512;

	// Receive until the peer shuts down the connection
	do 
	{
		status = recv(clientSocket, recvbuf, recvbuflen, 0);
		if (status > 0)
		{
			printf("Bytes received: %d\n", status);
			printf("MSG reads: ");
			recvbuflen = status - 1;
			do 
			{
				printf("%c", recvbuf[recvbuflen]);
				--recvbuflen;
			} while (recvbuflen != -1);
			printf("\n");

			// Reply to the client
			recvbuflen = 512;
			int msgId = 0;
			memcpy(&recvbuf[0], &msgId, 1);
			memcpy(&recvbuf[1], &msgId, 1);
			msgId = 14;
			memcpy(&recvbuf[2], &msgId, 1);
			recvbuf[3] = '\0';
			recvbuflen = sizeof(recvbuf);
			sendResult = send(clientSocket, recvbuf, recvbuflen, 0);
			if (sendResult == SOCKET_ERROR)
			{
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(clientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", sendResult);
			shutdown(clientSocket, SD_SEND);
			do 
			{
				status = recv(clientSocket, recvbuf, recvbuflen, 0);
			} while (status != -1);
			closesocket(clientSocket);
		}
		else if (status == 0)
			printf("Connection closing...\n");
		else
		{
			printf("recv failed: %d \n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}
	} while (status > 0);

	goto LABEL_ACCEPT;
	return 0;
}