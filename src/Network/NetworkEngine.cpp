#include "NetworkEngine.h"

bool NetworkEngine::SendPacket(const char* packet, unsigned int packetSize, unsigned long long connectionUId, unsigned char packetId)
{
	*(PacketHeader*)packet = { UNSAFEPROTOCOLID, packetSize, packetId, connectionUId };
	return SendUDPPacket(sock, packet, packetSize, serverAddress.GetSockaddr());
}

bool NetworkEngine::RecvPacket()
{
	sockaddr_in from;
	int fromsize = sizeof(from);
	int recvSize = recvfrom(sock, recvPacketBuffer, MAX_PACKET_SIZE, 0, (sockaddr*)& from, &fromsize);
	if (Address(from) != serverAddress) return false;
	if (recvSize > 0 && 
		(*(int*)recvPacketBuffer[PROTOCOLIDOFFSET] == UNSAFEPROTOCOLID || *(int*)recvPacketBuffer[PROTOCOLIDOFFSET] == SAFEPROTOCOLID))
	{
		if (*(int*)recvPacketBuffer[PROTOCOLIDOFFSET] == SAFEPROTOCOLID &&
			recvPacketBuffer[PACKETIDOFFSET] != CONF &&
			recvPacketBuffer[PACKETIDOFFSET] != NOCONF)
		{
			if(*(int*)recvPacketBuffer[PACKETSIZEOFFSET] == recvSize)
				SendPacket(sendPacketBuffer, DATAOFFSET, connectionUId, CONF); // inform server that packet was received
			else
				SendPacket(sendPacketBuffer, DATAOFFSET, connectionUId, NOCONF); // inform server that packet wasn't received
		}
		if (*(int*)recvPacketBuffer[PACKETSIZEOFFSET] != recvSize) return false;

		char* data = new char[recvSize];
		memcpy(data, recvPacketBuffer, recvSize);
		receivedPackets.push_back({ Address(from), recvSize, data, clock() });
		return true;
	}
	return false;
}

bool NetworkEngine::CollectGameDataFromPackets()
{
	for (int i = 0; i < receivedPackets.size(); i++)
	{
		if (receivedPackets[i].data[PACKETIDOFFSET] == GAMEDATA)
		{
			GameDataPacketHeader gdph = *(GameDataPacketHeader*)& receivedPackets[i].data[DATAOFFSET];
			if (gdph.packetNumber == 0)
			{
				collectingCurrentTickGameData = true;
				GameDataHeader gdh = *(GameDataHeader*)& receivedPackets[i].data[DATAOFFSET + 1];
				currentTickNumber = gdh.tickNumber;
				if (currentTickGameData)
				{
					if (prevTickGameData) delete[] prevTickGameData;
					prevTickGameData = new char[(*(GameDataHeader*)currentTickGameData).sizeOfGameData];
					memcpy(prevTickGameData, currentTickGameData, (*(GameDataHeader*)currentTickGameData).sizeOfGameData);
					delete[] currentTickGameData;
				}
				currentTickGameData = new char[gdh.sizeOfGameData];
			}
		}
	}

	if (collectingCurrentTickGameData)
	{
		for (int i = 0; i < receivedPackets.size(); i++)
		{
			if (receivedPackets[i].data[PACKETIDOFFSET] == GAMEDATA)
			{
				GameDataPacketHeader gdph = *(GameDataPacketHeader*)& receivedPackets[i].data[DATAOFFSET];
				if (gdph.tickNumber == currentTickNumber)
				{
					memcpy(&currentTickGameData[(FREE_PLACE_IN_SINGLE_PACKET - sizeof(GameDataPacketHeader)) * gdph.packetNumber],
						   &receivedPackets[i].data[DATAOFFSET + sizeof(GameDataPacketHeader)],
						   receivedPackets[i].size - (DATAOFFSET + sizeof(GameDataPacketHeader)));
				}
				receivedPackets.erase(receivedPackets.begin() + i);
				i -= 1;
			}
		}
	}
	return false;
}

void NetworkEngine::ProcessIncomingPacket()
{
	if (receivedPackets.size() == 0) return;
	switch (receivedPackets[0].data[PACKETIDOFFSET])
	{
	case PONG: lastPongPacketTime = receivedPackets[0].arriveTime; break;
	case GAMEDATA: CollectGameDataFromPackets(); break;
	case DISCONNECT: connectionState = DISCONNECTED; break;
	default:  break;
	}
	receivedPackets.erase(receivedPackets.begin());
}

void NetworkEngine::StartGame()
{
	// TODO
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
	while (receivedPackets.size() > 0)
	{
		ProcessIncomingPacket();
	}
}

void NetworkEngine::SendInput(NetworkInput input)
{
}
