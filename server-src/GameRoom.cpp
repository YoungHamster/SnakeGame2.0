#include "GameRoom.h"

void GameRoom::MovePlayers()
{
	gamePlayEngine.GameTick();
	for (int i = 0; i < players.size(); i++)
	{

	}
}

void GameRoom::HandlePlayersInput()
{
	for (int i = 0; i < players.size(); i++)
	{
		
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

void GameRoom::Tick()
{
	switch (state)
	{
	case game:
		break;
	case pauseGame:
		break;
	case endOfGame:
		break;
	case lobby:
		break;
	}
}
