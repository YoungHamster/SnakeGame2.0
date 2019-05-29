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
}

void GameRoom::PauseGame()
{
	state = pauseGame;
}

void GameRoom::EndGame()
{
	state = endOfGame;
}

void GameRoom::SendGameDataToPlayers()
{
	int gameDataSize;
	char* gameData = gamePlayEngine.GetGameData(&gameDataSize);
	int numberOfPackets = gameDataSize % (FREE_PLACE_IN_SINGLE_GAMEDATA_PACKET) == 0 ? /* Check if gameDataSize / (free palce for data in single packet) is whole number
																								   and if it's not add 1 to numberOfPackets*/
		gameDataSize / (FREE_PLACE_IN_SINGLE_GAMEDATA_PACKET) : gameDataSize / (FREE_PLACE_IN_SINGLE_GAMEDATA_PACKET) + 1;
	if (numberOfPackets >= 255)
	{
		delete[] gameData;
		return;
	}
	int lastPacketSize = (gameDataSize % (FREE_PLACE_IN_SINGLE_GAMEDATA_PACKET) == 0) ? SUPPOSED_MTU : 
		(gameDataSize % (FREE_PLACE_IN_SINGLE_GAMEDATA_PACKET) + DATAOFFSET + sizeof(GameDataPacketHeader));

	GameDataPacketHeader gdph;
	gdph.tickNumber = (*(GameDataHeader*)gameData).tickNumber;
	for (unsigned char i = 0; i < numberOfPackets; i++)
	{
		int packetSize = i == numberOfPackets - 1 ? lastPacketSize : SUPPOSED_MTU;
		memcpy(&sendPacketBuffer[DATAOFFSET + sizeof(GameDataPacketHeader)], &gameData[i * (FREE_PLACE_IN_SINGLE_GAMEDATA_PACKET)], packetSize - DATAOFFSET - sizeof(GameDataPacketHeader));
		gdph.packetNumber = i;
		*(GameDataPacketHeader*)& sendPacketBuffer[DATAOFFSET] = gdph;
		net->SendPacketToAll(sendPacketBuffer, packetSize, GAMEDATA);
	}
	delete[] gameData;
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
