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
			if (clock() - conn->packets[j].arriveTime >= gameTickPeriod * 2)
			{
				conn->packets.erase(conn->packets.begin() + j);
				j -= 1;
				continue;
			}
			switch (conn->packets[j].data[PACKETIDOFFSET])
			{
			case PING: 
				net->SendPacket(sendPacketBuffer, MAX_PACKET_SIZE, conn->connectionUId, PONG, 0);
				break;
			case GAMEDATA: ProcessGameDataPacket(conn->packets[j].data, conn->packets[j].size); break;
			case DISCONNECT:
				net->Disconnect(players[i].connectionUID); 
				players.erase(players.begin() + i); 
				i -= 1; 
				break;
			}
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
	int numberOfPackets = gameDataSize % (1300 - DATAOFFSET) == 0 ? // If (gameDataSize % (1300 - data) > 0) then we need gameDataSize / (1300 - data) + 1 packets to send all data
		gameDataSize / (1300 - DATAOFFSET) : gameDataSize / (1300 - DATAOFFSET) + 1; //1300 bytes per packet to surely fit in MTU of most devices
	int* packetsSizes = new int[numberOfPackets];
	for (int i = 0; i < numberOfPackets; i++)
	{
		packetsSizes[i] = i < (numberOfPackets - 1) ? (1300 - DATAOFFSET) : gameDataSize % (1300 - DATAOFFSET);
	}

	for (int i = 0; i < players.size(); i++)
	{
		for (int j = 0; j < numberOfPackets; j++)
		{
			memcpy(&sendPacketBuffer[DATAOFFSET], &gameData[j * (1300 - DATAOFFSET)], packetsSizes[j]);
			net->SendPacket(sendPacketBuffer, packetsSizes[j] + DATAOFFSET, players[i].connectionUID, GAMEDATA, (unsigned int)j);
		}
	}
	delete[] packetsSizes;
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
