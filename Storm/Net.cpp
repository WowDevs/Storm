#include "Net.h"

// Windows-specific functions
#ifdef _WIN32
	int Net::InitializeWinsock(const char *port)
	{
		WSADATA wsaData;
		struct addrinfo hints;
		int result;

		// Initializing the Winsock library
		result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0)
			printf("WSAStartup failed: %d\n", result);
		else
		{
			m_address = NULL;
			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			hints.ai_flags = AI_PASSIVE;

			// Resolve the local m_address and port to be used by the server instance
			result = getaddrinfo(NULL, "3724", &hints, &m_address);
			if (result != 0)
			{
				printf("getaddrinfo failed: %d\n", result);
				WSACleanup();
				return 0;
			}
		}
		return result;
	}

	SOCKET Net::CreateWindowsListeningSocket()
	{
		int result;
		SOCKET listeningSocket;

		// Create a SOCKET for the server to listen for client connections
		listeningSocket = socket(m_address->ai_family, m_address->ai_socktype, m_address->ai_protocol);
		if (listeningSocket == INVALID_SOCKET)
		{
			printf("Error at socket(): %ld\n", WSAGetLastError());
			freeaddrinfo(m_address);
			WSACleanup();
		}
		else
		{
			// Setup the TCP listening socket
			result = bind(listeningSocket, m_address->ai_addr, (int)m_address->ai_addrlen);
			if (result == SOCKET_ERROR)
			{
				printf("bind failed with error: %d\n", WSAGetLastError());
				freeaddrinfo(m_address);
				closesocket(listeningSocket);
				WSACleanup();
				listeningSocket = INVALID_SOCKET;
			}
			else
			{
				// Freeing memory
				freeaddrinfo(m_address);

				if (listen(listeningSocket, SOMAXCONN) == SOCKET_ERROR)
				{
					printf("Listen failed with error: %ld\n", WSAGetLastError());
					closesocket(listeningSocket);
					WSACleanup();
					listeningSocket = INVALID_SOCKET;
				}
			}
		}
		return listeningSocket;
	}
#endif // _WIN32

int Net::Initialize(const char *port)
{
	int result;

#ifdef _WIN32
	result = InitializeWinsock(port);
#endif
	return result;
}

SOCKET Net::CreateListeningSocket()
{
	int result;

#ifdef _WIN32
	result = CreateWindowsListeningSocket();
#endif
	return result;
}
