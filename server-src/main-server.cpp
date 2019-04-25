#include <thread>
#include <iostream>

#include "GamePlayEngine_server.h"
#include "NetworkManager.h"
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
	NetworkManager net(50001);
	GamePlayEngine_server gamePlayEngine(64, 36);
	ConsoleManager consoleManager;
	std::thread acceptConnectionsThr(AcceptConnections, &net);
	std::thread recvPacketsThr(RecvPackets, &net);
	recvPacketsThr.detach();
	acceptConnectionsThr.detach();
	std::cout << sizeof(PacketOffsets) << std::endl;
	std::cout << sizeof(PacketsIDs) << std::endl;
	while (true)
	{
		//net.AcceptConnection();
		//net.RecvPacket();
		gamePlayEngine.MoveSnakes();
		
		Sleep(1);
	}
	return 0;
}