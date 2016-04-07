#include <stdio.h>
#include "Net.h"
#include "DataStore.h"

DWORD WINAPI HandleConnection(void *clntSocket);
int LogonChallengeHandler(DataStore *msg);

int main()
{
	Net *netPtr;
	SOCKET listeningSocket;
	SOCKET clientSocket;
	int result;
	int addressSize;
	DWORD threadId;

	netPtr = (Net *)malloc(sizeof(netPtr));
	result = netPtr->Initialize("3724");
	listeningSocket = netPtr->CreateListeningSocket();
	addressSize = sizeof(netPtr->m_address);

	do 
	{
		// Accept a client socket
		clientSocket = accept(listeningSocket, 0, 0);
		if (clientSocket == INVALID_SOCKET)
		{
			printf("accept failed: %d\n", WSAGetLastError());
			closesocket(listeningSocket);
			WSACleanup();
			return 1;
		}
		threadId = 0;
		CreateThread(0, 0, HandleConnection, (void *)clientSocket, 0, &threadId);
	} while (1);
	free(netPtr);
	return 0;
}

DWORD WINAPI HandleConnection(void *clntSocket)
{
	DataStore *msg;
	char buf[255];
	SOCKET clientSocket;
	int result;
	int length;
	unsigned char cmd;
	unsigned char cmdType;

	clientSocket = (SOCKET)clntSocket;
	result = 0;
	length = 255;
	do 
	{
		result = recv(clientSocket, buf, length, 0);
		printf("Message received from socket %d\n", clientSocket);

		if (result == 0)
		{
			printf("The remote end has closed the connection\n");
			closesocket(clientSocket);
		}
		
		if (result == SOCKET_ERROR)
		{
			// The socket was likely closed
			closesocket(clientSocket);
		}

		msg = (DataStore *)malloc(sizeof(DataStore));
		memcpy(msg->m_data, buf, result);
		msg->m_read = 0;
		msg->m_size = result;
		
		DataStore::Get(msg, &cmd);
		DataStore::Get(msg, &cmdType);

		switch (cmd)
		{
		case AUTH_LOGON_CHALLENGE:
			LogonChallengeHandler(msg);
			break;
		default:
			printf("GRUNT: Unknown CMD %d\n", cmd);
			break;
		}
	} while (result > 0);

	free(msg);
	return result;
}

int LogonChallengeHandler(DataStore *msg)
{
	unsigned short size;
	int val;
	unsigned char gameName[4];
	unsigned char version[3];
	unsigned short build;
	unsigned char platform[4];
	unsigned char operatingSystem[4];
	unsigned char locale[5];
	int timezoneBias;
	unsigned char serverAddress[4];
	unsigned char accountLength;
	char accountName[17];

	DataStore::Get(msg, &size);
	DataStore::Get(msg, &val);
	val = ntohl(val);
	*(int *)gameName = val;
	gameName[0] = gameName[1];
	gameName[1] = gameName[2];
	gameName[2] = gameName[3];
	gameName[3] = 0;
	DataStore::Get(msg, &version[0]);
	DataStore::Get(msg, &version[1]);
	DataStore::Get(msg, &version[2]);
	DataStore::Get(msg, &build);
	DataStore::Get(msg, &val);
	val = ntohl(val);
	*(int *)platform = val;
	platform[0] = platform[1];
	platform[1] = platform[2];
	platform[2] = platform[3];
	platform[3] = 0;
	DataStore::Get(msg, &val);
	val = ntohl(val);
	*(int *)operatingSystem = val;
	operatingSystem[0] = operatingSystem[1];
	operatingSystem[1] = operatingSystem[2];
	operatingSystem[2] = operatingSystem[3];
	operatingSystem[3] = 0;
	DataStore::Get(msg, &val);
	val = ntohl(val);
	*(int *)locale = val;
	locale[4] = 0;
	DataStore::Get(msg, &timezoneBias);
	DataStore::Get(msg, &val);
	*(int *)serverAddress = val;
	DataStore::Get(msg, &accountLength);
	DataStore::GetString(msg, accountName, accountLength);

	printf(
		"GRUNT:\n\nLogonChallenge accepted connection from %s, patch %d.%d.%d (build %d).\n"
		"The platform is %s on %s, in %s locale.\n"
		"The account name is %s.\n"
		"The requested realmlist is %d.%d.%d.%d.\n\n",
		gameName, version[0], version[1], version[2], build, platform, operatingSystem, locale, accountName,
		serverAddress[0], serverAddress[1], serverAddress[2], serverAddress[3]
		);
return 0;
}