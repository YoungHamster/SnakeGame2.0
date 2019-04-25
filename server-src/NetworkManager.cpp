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

	newPacketBuffer = new char[maxPacketSize];
	connections = new Connection[maxNumberOfConnections];
}

NetworkManager::~NetworkManager()
{
	networkManagerWorking = false;
	delete[] newPacketBuffer;
	newPacketBuffer = nullptr;
	closesocket(sock);
	WSACleanup();
}

bool NetworkManager::SendPacket(const char* packet, int packet_size, sockaddr_in address)
{
	*(int*)& packet[protocolID] = PROTOCOLID;
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
	unsigned long long* connId = (unsigned long long*)&packet[connectionUID];
	*connId = connectionUId;
	lock.lock();
	sockaddr_in addr = GetConnectionByUId(connectionUId).address.GetSockaddr();
	lock.unlock();
	return SendPacket(packet, packet_size, addr);
}

Connection NetworkManager::GetConnectionByUId(unsigned long long connectionUId)
{
	lock.lock();
	for (int i = 0; i < numberOfActiveConnections; i++)
	{
		if (connections[numberOfActiveConnections].connectionUId == connectionUId)
		{
			lock.unlock();
			return connections[numberOfActiveConnections];
		}
	}
	lock.unlock();
	return Connection();
}

bool NetworkManager::RecvPacket()
{
	sockaddr_in from;
	int fromsize = sizeof(from);
	int recvSize = recvfrom(sock, newPacketBuffer, maxPacketSize, 0, (sockaddr*)& from, &fromsize);
	if (recvSize > 0 && *(int*)newPacketBuffer[protocolID] == PROTOCOLID)
	{
		char* data = new char[recvSize];
		memcpy(data, newPacketBuffer, recvSize);

		/* Accept the incoming connection */
		if (data[packetID] == newConnection)
		{
			lock.lock();
			incomingConnections.push_back({ from, recvSize, data, clock() });
			lock.unlock();
			return true;
		}
		int* connectionId = (int*)&data[connectionUID];
		lock.lock();
		connections[*connectionId].packets.push_back({ from, recvSize, data, clock() });
		lock.unlock();
		return true;
	}
	return false;
}

bool NetworkManager::AcceptConnection()
{
	lock.lock();
	if (incomingConnections.size() > 0)
	{
		/* protocoluid(int) + connection accepted/rejected(bool) + connectionuid(ull) */
		char* buff = new char[sizeof(int) + sizeof(char) + sizeof(unsigned long long)];
		
		/* Not using offsets from enum PacketOffsets, because it is not usual packet */
		buff[sizeof(int)] = (char)!(numberOfActiveConnections == maxNumberOfConnections);
		if (buff[sizeof(int)])
		{
			connections[numberOfActiveConnections].address = incomingConnections[incomingConnections.size() - 1].from;
			connections[numberOfActiveConnections].connectionUId = newConnectionUId;
			connections[numberOfActiveConnections].lastPingPacketTime = clock();
			*(unsigned long long*)& buff[sizeof(int) + sizeof(char)] = newConnectionUId;
			newConnectionUId += 1;
			numberOfActiveConnections += 1;
		}
		SendPacket(buff, sizeof(int) + sizeof(char) + sizeof(unsigned long long), incomingConnections[incomingConnections.size() - 1].from.GetSockaddr());
		incomingConnections.pop_back();
		lock.unlock();
		return buff[sizeof(int)];
	}
	lock.unlock();
	return false;
}

void NetworkManager::Disconnect(unsigned long long connectionUId)
{

}
