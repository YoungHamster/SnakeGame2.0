#include "NetworkEngine.h"

bool NetworkEngine::SendPacket(const char* packet, unsigned int packetSize, unsigned long long connectionUId, unsigned char packetId)
{
	*(int*)& packet[PROTOCOLIDOFFSET] = PROTOCOLID;
	*(unsigned int*)& packet[PACKETSIZEOFFSET] = packetSize;
	*(unsigned long long*)& packet[CONNECTIONUIDOFFSET] = connectionUId;
	*(unsigned char*)& packet[PACKETIDOFFSET] = packetId;
	return SendUDPPacket(sock, packet, packetSize, serverAddress.GetSockaddr());
}

bool NetworkEngine::RecvPacket()
{
	sockaddr_in from;
	int fromsize = sizeof(from);
	int recvSize = recvfrom(sock, recvPacketBuffer, MAX_PACKET_SIZE, 0, (sockaddr*)& from, &fromsize);
	if (Address(from) != serverAddress) return false;
	if (recvSize > 0 && *(int*)recvPacketBuffer[PROTOCOLIDOFFSET] == PROTOCOLID && *(int*)recvPacketBuffer[PACKETSIZEOFFSET] == recvSize)
	{
		char* data = new char[recvSize];
		memcpy(data, recvPacketBuffer, recvSize);
		receivedPackets.push_back({ Address(from), recvSize, data, clock() });
		return true;
	}
	return false;
}

bool NetworkEngine::CollectGameDataFromPackets()
{

	return false;
}

void NetworkEngine::ProcessIncomingPacket()
{
	switch (receivedPackets[0].data[PACKETIDOFFSET])
	{
	case PONG: lastPongPacketTime = receivedPackets[0].arriveTime; break;
	case GAMEDATA: CollectGameDataFromPackets(); break;
	case DISCONNECT: connectionState = DISCONNECTED; break;
	}
}

NetworkEngine::NetworkEngine(){}

NetworkEngine::~NetworkEngine(){}

bool NetworkEngine::Connect(Address address)
{
	serverAddress = address;
	char buff[DATAOFFSET];
	bool successfullyConnected = false;
	int numberOfAttemptsToConnect = 0;
	while (!successfullyConnected)
	{
		SendPacket(buff, DATAOFFSET, 0, NEWCONNECTION);
		Sleep(2000);
		for (int i = 0; i < receivedPackets.size(); i++)
		{
			if (receivedPackets.size() > 0)
			{
				if (receivedPackets[0].from != address)
				{
					receivedPackets.erase(receivedPackets.begin());
					continue;
				}
				numberOfAttemptsToConnect += 1;
				if (!*(bool*)& receivedPackets[0].data[sizeof(int)]) return false;
			}
		}
		if (numberOfAttemptsToConnect >= MaxNumberOfAttemptsToConnect) return false;
	}
	connectionState = CONNECTED;
	return false;
}

void NetworkEngine::Disconnect()
{
	char buff[DATAOFFSET];
	SendPacket(buff, DATAOFFSET, connectionUId, DISCONNECT);
	connectionState = DISCONNECTED;
}

void NetworkEngine::GameTick()
{
}

void NetworkEngine::SendInput(NetworkInput input)
{
}
