#include "GameRoom.h"

void GameRoom::HandlePlayersInput()
{
	for (int i = 0; i < players.size(); i++)
	{
		Connection* conn = net->GetConnectionByUId(players[i].connectionUID);
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
			case NEWCONNECTION: break;
			case PING: break;
			case GAMEDATA: break;
			case DISCONNECT: 
				net->Disconnect(players[i].connectionUID); 
				players.erase(players.begin() + i); 
				i -= 1; 
				break;
			}
		}
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

	char packet[1300];
	*(int*)&packet[PACKETIDOFFSET] = GAMEDATA;
	for (int i = 0; i < players.size(); i++)
	{
		for (int j = 0; j < numberOfPackets; j++)
		{
			memcpy(&packet[DATAOFFSET], &gameData[j * (1300 - DATAOFFSET)], packetsSizes[j]);
			*(int*)& packet[PACKETNUMBEROFFSET] = j;
			net->SendPacket(packet, packetsSizes[j] + DATAOFFSET, players[i].connectionUID);
		}
	}
	delete[] packetsSizes;
	delete[] gameData;
}

GameRoom::GameRoom(NetworkManager* net, int physw, int physh)
	:net(net), gamePlayEngine(physw, physh){}

void GameRoom::Tick()
{
	HandlePlayersInput();
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
