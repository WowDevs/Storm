#ifndef NET_H
#define NET_H

// General headers
#include <stdio.h>

// Windows-specific headers
#ifdef _WIN32
#undef _WINSOCKAPI_
#define _WINSOCKAPI_
#include <windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#endif // _WIN32

enum WOWCS_OPS
{
	AUTH_LOGON_CHALLENGE = 0x00,
	AUTH_LOGON_PROOF = 0x01,
	AUTH_RECONNECT_CHALLENGE = 0x02,
	AUTH_RECONNECT_PROOF = 0x03,
	REALM_LIST = 0x10,
	XFER_INITIATE = 0x30,
	XFER_DATA = 0x31,
	XFER_ACCEPT = 0x32,
	XFER_RESUME = 0x33,
	XFER_CANCEL = 0x34
};

struct Net
{
// Windows-specific functions
#ifdef _WIN32
	int InitializeWinsock(const char *port);
	SOCKET CreateWindowsListeningSocket();
#endif

	int Initialize(const char *port);
	SOCKET CreateListeningSocket();

	struct addrinfo *m_address;
};

#endif // !NET_H
