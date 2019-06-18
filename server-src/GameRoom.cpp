#include "GameRoom.h"

void GameRoom::ProcessGameDataPacket(char* packet, int packetSize)
{
	switch (packet[DATAOFFSET])
	{

	}
}

void GameRoom::ProcessPlayersInput()
{
	for (int i = 0; i < players.size(); i++)
	{
		Connection* conn = net->GetConnectionByUId(players[i].connectionUID);
		if (conn == nullptr)
		{
			players.erase(players.begin() + i);
			i -= 1;
			continue;
		}
		conn->lock.lock();
		for (int j = 0; j < conn->packets.size(); j++)
		{
			if (clock() - conn->packets[j].arriveTime >= gameTickPeriod)
			{
				std::cout << "WARNING: packets from players are being processed too slow" << std::endl;
				conn->lock.unlock();
				continue;
			}
			switch (conn->packets[j].data[PACKETIDOFFSET])
			{
			case PING: 
				conn->lastPingPacketTime = conn->packets[j].arriveTime;
				net->SendPacket(sendPacketBuffer, MAX_PACKET_SIZE, conn->connectionUId, PONG);
				break;
			case GAMEDATA: ProcessGameDataPacket(conn->packets[j].data, conn->packets[j].size); break;
			case DISCONNECT:
				net->Disconnect(players[i].connectionUID); 
				players.erase(players.begin() + i); 
				i -= 1; 
				conn->lock.unlock();
				continue;
				break;
			}
		}
		if (clock() - conn->lastPingPacketTime >= 5000)// no ping packets from player for 5 seconds-disconnect him
		{
			net->Disconnect(players[i].connectionUID);
			players.erase(players.begin() + i);
			i -= 1;
		}
		conn->lock.unlock();
	}
}

void GameRoom::StartGame()
{
	state = game;
	gamePlayEngine.StartGame(players.size(), DEFAULT_SNAKE_SIZE);
	int lastPacketSize;
	unsigned int numberOfPackets = CountNumberOfPackets(gamePlayEngine.GetPhysicsSize()*sizeof(PhysicalObject), 
														2/*sizeof(unsigned char) * 2*/, &lastPacketSize);
	if (numberOfPackets > 255)
	{
		std::cout << "ERROR! Physics array is too big, can't start game" << std::endl;
	}
	int numberOfPlayersUIds = players.size();
	unsigned long long* playersUIds = new unsigned long long[numberOfPlayersUIds];
	for (int i = 0; i < numberOfPlayersUIds; i++)
		playersUIds[i] = players[i].connectionUID;

	const char* physics = (char*)gamePlayEngine.GetPhysics();
	for (unsigned char i = 0; i < numberOfPackets; i++)
	{
		memcpy(&sendPacketBuffer[DATAOFFSET + 2/*sizeof(unsigned char) * 2*/],
			   &physics[i * (FREE_PLACE_IN_SINGLE_PACKET - 2/*sizeof(unsigned char) * 2*/)],
			   (FREE_PLACE_IN_SINGLE_PACKET - 2/*sizeof(unsigned char) * 2*/));
		*(unsigned char*)& sendPacketBuffer[DATAOFFSET] = i;
		*(unsigned char*)& sendPacketBuffer[DATAOFFSET + 1] = (unsigned char)numberOfPackets;
		int packetSize = i == numberOfPackets ? lastPacketSize : SUPPOSED_MTU;
		int nocdntrp;//number of connections that didn't receive packet
		unsigned long long* failedConnections = net->ReliablySendPacketToListOfConnections(sendPacketBuffer, packetSize,
																						   STARTGAME, playersUIds,
																						   numberOfPlayersUIds, &nocdntrp);
		DeleteFailedConnections(failedConnections, playersUIds, nocdntrp, players.size());
		if (failedConnections) delete[] failedConnections;
	}
	delete[] playersUIds;
}

void GameRoom::PauseGame()
{
	state = pauseGame;
	unsigned long long* playersUIds = new unsigned long long[players.size()];
	for (int i = 0; i < players.size(); i++)
		playersUIds[i] = players[i].connectionUID;

	int numOfConnsDidntRecvPacket;//number of connections that didn't receive packet
	unsigned long long* failedConnections = net->ReliablySendPacketToListOfConnections(sendPacketBuffer, DATAOFFSET,
																					   PAUSEGAME, playersUIds,
																					   players.size(), &numOfConnsDidntRecvPacket);
	DeleteFailedConnections(failedConnections, playersUIds, numOfConnsDidntRecvPacket, players.size());
	if(failedConnections) delete[] failedConnections;
}

void GameRoom::EndGame()
{
	state = endOfGame;
}

void GameRoom::SendGameDataToPlayers()
{
	int gameDataSize;
	char* gameData = gamePlayEngine.GetGameData(&gameDataSize);
	int lastPacketSize;
	int numberOfPackets = CountNumberOfPackets(gameDataSize, sizeof(GameDataPacketHeader), &lastPacketSize);
	if (numberOfPackets >= 255)
	{
		delete[] gameData;
		return;
	}

	GameDataPacketHeader gdph;
	gdph.tickNumber = (*(GameDataHeader*)gameData).tickNumber;
	for (unsigned char i = 0; i < numberOfPackets; i++)
	{
		int packetSize = i == numberOfPackets - 1 ? lastPacketSize : SUPPOSED_MTU;
		memcpy(&sendPacketBuffer[DATAOFFSET + sizeof(GameDataPacketHeader)], &gameData[i * (FREE_PLACE_IN_SINGLE_PACKET - sizeof(GameDataPacketHeader))], packetSize - DATAOFFSET - sizeof(GameDataPacketHeader));
		gdph.packetNumber = i;
		*(GameDataPacketHeader*)& sendPacketBuffer[DATAOFFSET] = gdph;
		net->SendPacketToAll(sendPacketBuffer, packetSize, GAMEDATA);
	}
	delete[] gameData;
}

int GameRoom::CountNumberOfPackets(int dataSize, int sizeOfAdditionalHeaders, int* lastPacketSize)
{
	/* Check if dataSize / (free palce for data in single packet) is whole number and if it's not add 1 to numberOfPackets and compute lastPacketSize*/
	int numberOfPackets;
	if (dataSize % (FREE_PLACE_IN_SINGLE_PACKET - sizeOfAdditionalHeaders) == 0)
	{
		numberOfPackets = dataSize / (FREE_PLACE_IN_SINGLE_PACKET - sizeOfAdditionalHeaders);
		*lastPacketSize = SUPPOSED_MTU;
	}
	else
	{
		numberOfPackets = dataSize / (FREE_PLACE_IN_SINGLE_PACKET - sizeOfAdditionalHeaders) + 1;
		*lastPacketSize = (dataSize % (FREE_PLACE_IN_SINGLE_PACKET - sizeOfAdditionalHeaders) + DATAOFFSET + sizeOfAdditionalHeaders);
	}
	return numberOfPackets;
}

void GameRoom::DeleteFailedConnections(unsigned long long * failedConnectionsUIds, unsigned long long* fullListOfConnectionsUIds,
									   int numberOfFailedConnections, int sizeOfFullListOfConnectionsUIds)
{
	if (numberOfFailedConnections > 0)
	{
		int disconnectedPlayersCounter = 0;
		for (int j = 0; j < players.size(); j++)
		{
			if (players[j].connectionUID == failedConnectionsUIds[disconnectedPlayersCounter])
			{
				net->Disconnect(failedConnectionsUIds[disconnectedPlayersCounter]);
				players.erase(players.begin() + j);
				j -= 1;
				memcpy(&fullListOfConnectionsUIds[j],
					   &fullListOfConnectionsUIds[j + 1],
					   (sizeOfFullListOfConnectionsUIds - j) * sizeof(unsigned long long));
				disconnectedPlayersCounter += 1;
			}
		}
	}
}

GameRoom::GameRoom(NetworkManager* net, int gameFieldWidth, int gameFieldHeight)
	:net(net), gamePlayEngine(gameFieldWidth, gameFieldHeight){}

void GameRoom::Tick()
{
	ProcessPlayersInput();
	switch (state)
	{
	case game:
		if (clock() - lastGameTickTime > gameTickPeriod)
		{
			gamePlayEngine.GameTick();
		}
		SendGameDataToPlayers();
		break;
	case pauseGame:
		break;
	case endOfGame:
		break;
	case lobby:
		break;
	}
}
