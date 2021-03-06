#include "NetworkEngine.h"

bool NetworkEngine::SendPacket(const char* packet, unsigned int packetSize, unsigned long long connectionUId, unsigned char packetId)
{
	*(PacketHeader*)packet = { UNSAFEPROTOCOLID, packetSize, packetId, connectionUId };
	return SendUDPPacket(sock, packet, packetSize, serverAddress.GetSockaddr());
}

bool NetworkEngine::ReliablySendPacket(const char* packet, unsigned int packetSize, unsigned char packetId)
{
	*(PacketHeader*)packet = { SAFEPROTOCOLID, packetSize, packetId, connectionUId };
	if (connectionState == DISCONNECTED) return false;
	clock_t packetSentTime;
	bool failed = false;
	bool sentPacket = false;
	unsigned int numberOfTries = 0;
	while (!failed)
	{
		if (!sentPacket)
		{
			SendUDPPacket(sock, packet, packetSize, serverAddress.GetSockaddr());
			packetSentTime = clock();
			numberOfTries += 1;
			sentPacket = true;
		}
		if (numberOfTries > NUMBER_OF_TRIES_IN_RELIABLE_TRANSFER_BEFORE_FAILURE)
		{
			return false;
		}
		for (int i = 0; i < receivedPackets.size(); i++)
		{
			if (*(int*)& receivedPackets[i].data[PROTOCOLIDOFFSET] == SAFEPROTOCOLID)
			{
				if (receivedPackets[i].data[PACKETIDOFFSET] == CONF)
				{
					return true;
				}
				if (receivedPackets[i].data[PACKETIDOFFSET] == NOCONF)
				{
					sentPacket = false;
				}
			}
		}
		if (clock() - packetSentTime > SINGLE_TRY_IN_RELIABLE_TRANSFER_DELAY)
		{
			sentPacket = false;
		}
		Sleep(1);
	}
	return false;
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

bool NetworkEngine::StartGame()
{
	ReliablySendPacket(sendPacketBuffer, DATAOFFSET, STARTGAME);
	clock_t startTime = clock();
	bool receivedFirstPacket = false;
	bool* receivedPhysicsPackets;
	unsigned char numberOfPackets = 0;
	bool receivedPhysics = false;
	bool DONE = false;
	while (!DONE)
	{
		for (int i = 0; i < receivedPackets.size(); i++)
		{
			if (receivedPackets[i].data[PACKETIDOFFSET] == STARTGAME)
			{
				if (!receivedFirstPacket)
				{
					if (gamePlayData.physics)
						delete[] gamePlayData.physics;
					numberOfPackets = *(unsigned char*)& receivedPackets[i].data[DATAOFFSET + 1];
					gamePlayData.physics = new PhysicalObject[numberOfPackets];
					receivedPhysicsPackets = new bool[numberOfPackets];
					memset(receivedPhysicsPackets, false, numberOfPackets);
				} 
				char* gpdphys = (char*)gamePlayData.physics;//char type to count offsets in bytes
				memcpy(&gpdphys[(FREE_PLACE_IN_SINGLE_PACKET - 2/*sizeof(unsigned char) * 2*/) *
					   *(unsigned char*)& receivedPackets[i].data[DATAOFFSET]],
					   &receivedPackets[i].data[DATAOFFSET + 2/*sizeof(unsigned char) * 2*/],
					   (FREE_PLACE_IN_SINGLE_PACKET - 2/*sizeof(unsigned char) * 2*/));
				receivedPhysicsPackets[*(unsigned char*)& receivedPackets[i].data[DATAOFFSET]] = true;
				receivedPackets.erase(receivedPackets.begin() + i);
				i -= 1;
			}
		}
		unsigned char numberOfReceivedPackets = 0;
		for (int i = 0; i < numberOfPackets; i++)
			if (receivedPhysicsPackets[i]) numberOfReceivedPackets += 1;
		receivedPhysics = numberOfReceivedPackets == numberOfPackets;
		DONE = receivedPhysics || (clock() - startTime > SINGLE_TRY_IN_RELIABLE_TRANSFER_DELAY * 10);
	}
	if (receivedPhysicsPackets) delete[] receivedPhysicsPackets;
	if (!receivedPhysics) return false;
	//TODO
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
