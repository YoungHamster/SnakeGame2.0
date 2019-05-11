#include "NetworkBase.h"

Packet::~Packet()
{
	if (data)
	{
		delete[] data;
	}
}

Address::Address() {}

Address::Address(sockaddr_in addr)
	: address(addr.sin_addr.s_addr), port(addr.sin_port) {}

Address::Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port)
	: address(htonl((a << 24) | (b << 16) | (c << 8) | d)), port(htons(port)) {}

Address::Address(unsigned int address, unsigned short port)
	: address(htonl(address)), port(htons(port)) {}

unsigned int Address::GetAddress() const
{
	return address;
}

sockaddr_in Address::GetSockaddr() const
{
	return sockaddr_in();
}

unsigned char Address::GetA() const
{
	return address;
}

unsigned char Address::GetB() const
{
	return address >> 8;
}

unsigned char Address::GetC() const
{
	return address >> 16;
}

unsigned char Address::GetD() const
{
	return address >> 24;
}

unsigned short Address::GetPort() const
{
	return port;
}

bool Address::operator==(const Address& other) const
{
	if (this->address == other.address && this->port == other.port)
	{
		return true;
	}
	return false;
}

bool Address::operator!=(const Address& other) const
{
	return !(*this == other);
}

bool SendUDPPacket(SOCKET sock, const char* packet, int packetSize, sockaddr_in address)
{
	int sent_bytes = sendto(sock, packet, packetSize, 0, (sockaddr*)& address, sizeof(sockaddr_in));

	if (sent_bytes != packetSize)
	{
		std::cout << "failed to send packet: return value = " << sent_bytes << std::endl;
		return false;
	}
	return true;
}
