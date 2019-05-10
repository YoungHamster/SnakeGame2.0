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

bool NetworkManager::SendPacket(const char* packet, int packet_size, sockaddr_in address)
{
	*(int*)& packet[PROTOCOLIDOFFSET] = PROTOCOLID;
	int sent_bytes = sendto(sock, packet, packet_size, 0, (sockaddr*)& address, sizeof(sockaddr_in));

	if (sent_bytes != packet_size)
	{
		std::cout << "failed to send packet: return value = " << sent_bytes << std::endl;
		return false;
	}
	return true;
}

bool NetworkManager::SendPacket(const char* packet, int packet_size, unsigned long long connectionUId)
{
	unsigned long long* connId = (unsigned long long*)&packet[CONNECTIONUIDOFFSET];
	*connId = connectionUId;
	sockaddr_in addr = GetConnectionByUId(connectionUId)->address.GetSockaddr();
	return SendPacket(packet, packet_size, addr);
}

Connection* NetworkManager::GetConnectionByUId(unsigned long long connectionUId)
{
	for (int i = 0; i < numberOfActiveConnections; i++)
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
	int recvSize = recvfrom(sock, newPacketBuffer, maxPacketSize, 0, (sockaddr*)& from, &fromsize);
	if (recvSize > 0 && *(int*)newPacketBuffer[PROTOCOLIDOFFSET] == PROTOCOLID && *(int*)newPacketBuffer[PACKETSIZEOFFSET] == recvSize)
	{
		char* data = new char[recvSize];
		memcpy(data, newPacketBuffer, recvSize);

		/* Accept the incoming connection */
		if (data[PACKETIDOFFSET] == NEWCONNECTION)
		{
			incomingConnectionsLock.lock();
			incomingConnections.push_back({ from, recvSize, data, clock() });
			incomingConnectionsLock.unlock();
			return true;
		}
		unsigned long long connectionUId = *(unsigned long long*)&data[CONNECTIONUIDOFFSET];
		/* not thread safe */Connection* conn = GetConnectionByUId(connectionUId);
		if (conn != nullptr)
		{
			conn->lock.lock();
			conn->packets.push_back({ from, recvSize, data, clock() });
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
		
		/* Not using offsets from enum PacketOffsets, because it is not usual packet */
		buff[sizeof(int)] = (char)!(numberOfActiveConnections == maxNumberOfConnections);
		if (buff[sizeof(int)])
		{
			connections[numberOfActiveConnections].lock.lock();
			connections[numberOfActiveConnections].address = incomingConnections[incomingConnections.size() - 1].from;
			connections[numberOfActiveConnections].connectionUId = newConnectionUId;
			connections[numberOfActiveConnections].lastPingPacketTime = clock();
			connections[numberOfActiveConnections].lock.unlock();

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

}
