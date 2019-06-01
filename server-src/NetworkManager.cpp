#include "NetworkManager.h"

#include <iostream>
#include <stdio.h>

NetworkManager::NetworkManager(unsigned short port)
{
	WSADATA WsaData;
	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != NO_ERROR)
	{
		__debugbreak();
	}

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socket <= 0)
	{
		std::cout << "failed to create socket\n";
		__debugbreak();
	}

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(sock, (const sockaddr*)& address, sizeof(sockaddr_in)) < 0)
	{
		std::cout << "failed to bind socket\n";
		__debugbreak();
	}

	DWORD nonBlocking = 1;
	if (ioctlsocket(sock, FIONBIO, &nonBlocking) != 0)
	{
		std::cout << "failed to set non-blocking socket\n";
		__debugbreak();
	}
}

NetworkManager::~NetworkManager()
{
	networkManagerWorking = false;
	closesocket(sock);
	WSACleanup();
}

bool NetworkManager::SendPacket(const char* packet, int packetSize, sockaddr_in address)
{
	*(int*)& packet[PROTOCOLIDOFFSET] = UNSAFEPROTOCOLID;
	return SendUDPPacket(sock, packet, packetSize, address);
}

bool NetworkManager::SendPacket(const char* packet, unsigned int packetSize, unsigned long long connectionUId, unsigned char packetId)
{
	*(PacketHeader*)packet = { UNSAFEPROTOCOLID, packetSize, packetId, connectionUId };
	return SendUDPPacket(sock, packet, packetSize, GetConnectionByUId(connectionUId)->address.GetSockaddr());
}

bool NetworkManager::SendPacketToAll(const char* packet, unsigned int packetSize, unsigned char packetId)
{
	bool returnValue = true;
	for (int i = 0; i < maxNumberOfConnections; i++)
	{
		if (connections[i].active)
		{
			returnValue = SendPacket(packet, packetSize, connections[i].connectionUId, packetId);
		}
	}
	return returnValue;
}

bool NetworkManager::SendPacketToListOfConnection(const char* packet, unsigned int packetSize, unsigned char packetId, unsigned long long* connectionsUIds, int numberOfConnectionsUIds)
{
	bool returnValue = true;
	for (int i = 0; i < numberOfConnectionsUIds; i++)
	{
		if (connections[i].active)
		{
			returnValue = SendPacket(packet, packetSize, connectionsUIds[i], packetId);
		}
	}
	return returnValue;
}

bool NetworkManager::ReliablySendPacket(const char* packet, unsigned int packetSize, unsigned long long connectionUId, unsigned char packetId)
{
	*(PacketHeader*)packet = { SAFEPROTOCOLID, packetSize, packetId, connectionUId };
	Connection* conn = GetConnectionByUId(connectionUId);
	clock_t packetSentTime;
	bool failed = false;
	bool sentPacket = false;
	unsigned int numberOfTries = 0;
	while (!failed)
	{
		if (!sentPacket)
		{
			conn->lock.lock();
			SendUDPPacket(sock, packet, packetSize, conn->address.GetSockaddr());
			conn->lock.unlock();
			packetSentTime = clock();
			numberOfTries += 1;
			sentPacket = true;
		}
		if (numberOfTries > NUMBER_OF_TRIES_IN_RELIABLE_TRANSFER_BEFORE_FAILURE)
		{
			return false;
		}
		conn->lock.lock();
		for (int i = 0; i < conn->packets.size(); i++)
		{
			if (*(int*)& conn->packets[i].data[PROTOCOLIDOFFSET] == SAFEPROTOCOLID)
			{
				if (conn->packets[i].data[PACKETIDOFFSET] == CONF)
				{
					conn->lock.unlock();
					return true;
				}
				if (conn->packets[i].data[PACKETIDOFFSET] == NOCONF)
				{
					sentPacket = false;
				}
			}
		}
		conn->lock.unlock();
		if (clock() - packetSentTime > SINGLE_TRY_IN_RELIABLE_TRANSFER_DELAY)
		{
			sentPacket = false;
		}
		Sleep(1);
	}
}

unsigned long long* NetworkManager::ReliablySendPacketToListOfConnections(const char* packet, unsigned int packetSize, unsigned char packetId, 
																		 unsigned long long* connectionsUIds, int numberOfConnectionsUIds, 
																		 int* numberOfConnectionsThatDidntReceivePacket)
{
	*(PacketHeader*)packet = { SAFEPROTOCOLID, packetSize, packetId, 0 };
	unsigned long long* connectionUId = (unsigned long long*) & packet[CONNECTIONUIDOFFSET];

	ConnInfoForRDT* connectionsData = new ConnInfoForRDT[numberOfConnectionsUIds];

	bool done = false;
	while (!done)
	{
		done = true;
		for (int i = 0; i < numberOfConnectionsUIds; i++)
		{
			if (connectionsData[i].failed || connectionsData[i].succeed) continue;
			/* Will be really slow until start using hash table for connections */
			Connection* conn = GetConnectionByUId(connectionsUIds[i]);

			bool doneWithThisConnection = false;
			if (!connectionsData[i].sentPacket)
			{
				*connectionUId = connections[i].connectionUId;
				SendUDPPacket(sock, packet, packetSize, connections[i].address.GetSockaddr());
				connectionsData[i].packetSentTime = clock();
				connectionsData[i].numberOfTries += 1;
				connectionsData[i].sentPacket = true;
			}

			if (connectionsData[i].numberOfTries > NUMBER_OF_TRIES_IN_RELIABLE_TRANSFER_BEFORE_FAILURE)
			{
				doneWithThisConnection = true;
				connectionsData[i].failed = true;
			}

			connections[i].lock.lock();
			for (int j = 0; j < connections[i].packets.size(); j++)
			{
				if (*(int*)& connections[i].packets[j].data[PROTOCOLIDOFFSET] == SAFEPROTOCOLID)
				{
					if (connections[i].packets[j].data[PACKETIDOFFSET] == CONF)
					{
						doneWithThisConnection = true;
						connectionsData[i].succeed = true;
					}
					if (connections[i].packets[j].data[PACKETIDOFFSET] == NOCONF)
					{
						connectionsData[i].sentPacket = false;
					}
					if(connections[i].packets[j].data[PACKETIDOFFSET] == CONF ||
						connections[i].packets[j].data[PACKETIDOFFSET] == NOCONF)
						j = connections[i].packets.size();
				}
			}
			connections[i].lock.unlock();

			if (clock() - connectionsData[i].packetSentTime > SINGLE_TRY_IN_RELIABLE_TRANSFER_DELAY)
			{
				connectionsData[i].sentPacket = false;
			}
			/* not done until done with every single connection */
			if(done) done = doneWithThisConnection;
		}
		Sleep(1);
	}

	unsigned long long* connectionsThatFailed = nullptr;
	if (numberOfConnectionsThatDidntReceivePacket != nullptr)
	{
		*numberOfConnectionsThatDidntReceivePacket = 0;
		for (int i = 0; i < numberOfConnectionsUIds; i++)
		{
			if (connectionsData[i].failed)* numberOfConnectionsThatDidntReceivePacket += 1;
		}
		connectionsThatFailed = new unsigned long long[*numberOfConnectionsThatDidntReceivePacket];
		int connectionsThatFailedCounter = 0;
		for (int i = 0; i < numberOfConnectionsUIds; i++)
		{
			if (connectionsData[i].failed)
			{
				connectionsThatFailed[connectionsThatFailedCounter] = connectionsUIds[i];
				connectionsThatFailedCounter += 1;
			}
		}
	}

	delete[] connectionsData;
	return connectionsThatFailed;
}

Connection* NetworkManager::GetConnectionByUId(unsigned long long connectionUId)
{
	for (int i = 0; i < maxNumberOfConnections; i++)
	{
		connections[i].lock.lock();
		if (connections[i].connectionUId == connectionUId)
		{
			connections[i].lock.unlock();
			return &connections[i];
		}
		connections[i].lock.unlock();
	}
	return nullptr;
}

bool NetworkManager::RecvPacket()
{
	sockaddr_in from;
	int fromsize = sizeof(from);
	int recvSize = recvfrom(sock, recvPacketBuffer, MAX_PACKET_SIZE, 0, (sockaddr*)& from, &fromsize);
	if (recvSize > 0 && 
		(*(int*)recvPacketBuffer[PROTOCOLIDOFFSET] == UNSAFEPROTOCOLID || *(int*)recvPacketBuffer[PROTOCOLIDOFFSET] == SAFEPROTOCOLID))
	{
		if (*(int*)recvPacketBuffer[PROTOCOLIDOFFSET] == SAFEPROTOCOLID &&
			recvPacketBuffer[PACKETIDOFFSET] != CONF &&
			recvPacketBuffer[PACKETIDOFFSET] != NOCONF)
		{
			char packet[DATAOFFSET];
			if (*(int*)recvPacketBuffer[PACKETSIZEOFFSET] == recvSize)
				SendPacket(packet, DATAOFFSET, *(unsigned long long*)recvPacketBuffer[CONNECTIONUIDOFFSET], CONF); // inform client that packet was received
			else
			{
				*(PacketHeader*)packet = { UNSAFEPROTOCOLID, DATAOFFSET, CONF, 0};
				SendUDPPacket(sock, packet, DATAOFFSET, from);
			}
		}
		if (*(int*)recvPacketBuffer[PACKETSIZEOFFSET] != recvSize) return false;

		char* data = new char[recvSize];
		memcpy(data, recvPacketBuffer, recvSize);

		/* Accept the incoming connection */
		if (data[PACKETIDOFFSET] == NEWCONNECTION)
		{
			incomingConnectionsLock.lock();
			incomingConnections.push_back({ from, recvSize, data, clock() });
			incomingConnectionsLock.unlock();
			return true;
		}
		unsigned long long connectionUId = *(unsigned long long*)&data[CONNECTIONUIDOFFSET];
		Connection* conn = GetConnectionByUId(connectionUId);
		if (conn != nullptr)
		{
			conn->lock.lock();

			/* if user was disconnected send him disconnection packet second time in case he didn't receive first */
			if (!conn->active)
			{
				char packet[DATAOFFSET];
				SendPacket(packet, DATAOFFSET, connectionUId, DISCONNECT);
				conn->connectionUId = 0;
			}
			else
			{
				conn->packets.push_back({ from, recvSize, data, clock() });
			}
			conn->lock.unlock();
		}
		/* because other thread can get connection pointer and use it without lock */
		return true;
	}
	return false;
}

bool NetworkManager::AcceptConnection()
{
	incomingConnectionsLock.lock();
	if (incomingConnections.size() > 0)
	{
		incomingConnectionsLock.unlock();

		/* protocoluid(int) + connection accepted/rejected(bool) + connectionuid(ull) */
		char* buff = new char[sizeof(int) + sizeof(char) + sizeof(unsigned long long)];
		
		/* find not active connection for new connection */
		int newConnectionId = -1;
		for(int i = 0; i < maxNumberOfConnections; i++)
		{
			connections[i].lock.lock();
			if (!connections[i].active)
			{
				newConnectionId = i;
				connections[i].active = true;
				i = maxNumberOfConnections;
			}
			connections[i].lock.unlock();
		}

		/* Not using offsets from enum PacketOffsets, because it is not usual packet */
		buff[sizeof(int)] = (char)!(newConnectionId == -1);
		if (buff[sizeof(int)])
		{
			incomingConnectionsLock.lock();
			connections[newConnectionId].lock.lock();
			connections[newConnectionId].address = incomingConnections[incomingConnections.size() - 1].from;
			incomingConnectionsLock.unlock();
			connections[newConnectionId].connectionUId = newConnectionUId;
			connections[newConnectionId].lastPingPacketTime = clock();
			connections[newConnectionId].lock.unlock();

			*(unsigned long long*)& buff[sizeof(int) + sizeof(char)] = newConnectionUId;
			newConnectionUId += 1;
			numberOfActiveConnections += 1;
		}

		incomingConnectionsLock.lock();
		SendPacket(buff, sizeof(int) + sizeof(char) + sizeof(unsigned long long), incomingConnections[incomingConnections.size() - 1].from.GetSockaddr());
		incomingConnections.pop_back();
		incomingConnectionsLock.unlock();

		return buff[sizeof(int)];
	}
	incomingConnectionsLock.unlock();
	return false;
}

void NetworkManager::Disconnect(unsigned long long connectionUId)
{
	char packet[DATAOFFSET];
	SendPacket(packet, DATAOFFSET, connectionUId, DISCONNECT);
	GetConnectionByUId(connectionUId)->active = false;
}
