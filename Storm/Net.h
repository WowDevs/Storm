#ifndef NET_H

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#endif // _WIN32

struct DataStore
{
	DataStore *Put(DataStore *msgPtr, int val);
	DataStore *Get(DataStore *msgPtr, int *val);

	char *m_data;
	unsigned int m_base;
	unsigned int m_alloc;
	unsigned int m_size;
	unsigned int m_read;
};

DataStore *DataStore::Put(DataStore *msgPtr, int val)
{
	DataStore *msg;

	msg = msgPtr;
	return msg;
}

DataStore *DataStore::Get(DataStore *msgPtr, int *val)
{
	DataStore *msg;

	msg = msgPtr;
	//*val = *(&msg->m_data[msg->m_read] - *v6);
	return msg;
}

#endif // !NET_H
