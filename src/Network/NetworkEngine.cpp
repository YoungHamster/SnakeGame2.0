#include "NetworkEngine.h"

bool NetworkEngine::SendPacket()
{
	return false;
}

bool NetworkEngine::RecvPacket()
{
	sockaddr_in from;
	int fromsize = sizeof(from);
	int recvSize = recvfrom(sock, recvPacketBuffer, MAX_PACKET_SIZE, 0, (sockaddr*)& from, &fromsize);
	if (Address(from) != serverAddress) return false;
	if (recvSize > 0 && *(int*)recvPacketBuffer[PROTOCOLIDOFFSET] == PROTOCOLID && *(int*)recvPacketBuffer[PACKETSIZEOFFSET] == recvSize)
	{

	}
	return false;
}

bool NetworkEngine::CollectGameDataFromPackets()
{
	return false;
}

NetworkEngine::NetworkEngine(){}

NetworkEngine::~NetworkEngine(){}

bool NetworkEngine::Connect(Address address)
{
	return false;
}

void NetworkEngine::Disconnect()
{
}

void NetworkEngine::GameTick()
{
}

void NetworkEngine::SendInput(NetworkInput input)
{
}
