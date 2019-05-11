#include <thread>
#include <iostream>

#include "GameRoom.h"
#include "ConsoleManager.h"

static bool serverRunning = true;

void AcceptConnections(NetworkManager* net)
{
	while (serverRunning)
	{
		if(!net->AcceptConnection()) Sleep(1);
		else std::cout << "Accepted connection!" << std::endl;
	}
}

void RecvPackets(NetworkManager* net)
{
	while (serverRunning)
	{
		if (!net->RecvPacket()) Sleep(1);
		else std::cout << "Accepted packet!" << std::endl;
	}
}

int main()
{
	ConsoleManager consoleManager;
	NetworkManager* net = new NetworkManager(50001);
	GameRoom* gameRoom = new GameRoom(net, 64, 36);
	std::thread acceptConnectionsThr(AcceptConnections, net);
	std::thread recvPacketsThr(RecvPackets, net);
	recvPacketsThr.detach();
	acceptConnectionsThr.detach();
	while (true)
	{
		gameRoom->Tick();
		Sleep(1);
	}
	return 0;
}