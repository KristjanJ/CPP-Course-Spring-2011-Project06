#include <iostream>
#include <cassert>

#include "GuessingClient.h"
#include "GuessingGame.h"

GuessingClient::GuessingClient(void)
{
	peerinterface = 0;
	connected = false;
	readyToPlay = false;
	highscoresReceived = false;
	guessReply = 65535;
	serverAddress = UNASSIGNED_SYSTEM_ADDRESS;
}

GuessingClient::~GuessingClient(void)
{
	Disconnect();
}

bool GuessingClient::Connect(std::string address, unsigned short port)
{
	if (connected)
	{
		std::cout << "Already connected!" << std::endl;
		return false;
	}

	peerinterface = RakPeerInterface::GetInstance();

	if (!peerinterface)
	{
		std::cout << "ERROR: Couldn't get network engine interface!" << std::endl;
		return false;
	}

	peerinterface->AllowConnectionResponseIPMigration(false);

	SocketDescriptor sock (10000, 0);

	int threadPriority = 0;
#ifdef _WIN32
	threadPriority = 0;
#else
	#include <sched.h>
	threadPriority = sched_get_priority_max (SCHED_FIFO);
#endif

	StartupResult sr = peerinterface->Startup (1, &sock, 1, threadPriority);

	if (sr != RAKNET_STARTED)
	{
		std::cout << "ERROR: Failed to bind to port 10000!" << std::endl;
		return false;
	}

	peerinterface->SetOccasionalPing(true);

	std::cout << "Connecting to " << address.c_str() << ":" << port << "." << std::endl;
	ConnectionAttemptResult car = peerinterface->Connect(address.c_str (), port, 0, 0);

	if (car != CONNECTION_ATTEMPT_STARTED)
	{
		std::cout << "Connecting failed!" << std::endl;
		return false;
	}

	return true;
}

unsigned char GuessingClient::GetPacketIdentifier(Packet* packet)
{
	assert (packet);

	if ((unsigned char)packet->data[0] == ID_TIMESTAMP)
	{
		assert(packet->bitSize > sizeof(unsigned char) + sizeof(unsigned long));
		return (unsigned char) packet->data[sizeof(unsigned char) + sizeof(unsigned long)];
	}
	else
	{
	    return (unsigned char) packet->data[0];
	}
}

bool GuessingClient::NextFrame (void)
{
	assert (peerinterface);

	Packet* packet = 0;
	unsigned char packetidentifier = 0;

	packet = peerinterface->Receive();

	if (packet == 0)
	{
	    return true;
	}

	while (packet != 0)
	{
		packetidentifier = GetPacketIdentifier (packet);

		switch (packetidentifier)
		{
			case ID_CONNECTION_REQUEST_ACCEPTED:
				std::cout << "Successfully connected to " <<  packet->systemAddress.ToString () <<  "." << std::endl;
				serverAddress = packet->systemAddress;
				connected = true;
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "Connection to " <<  packet->systemAddress.ToString () <<  " closed." << std::endl;
				break;
			case ID_ALREADY_CONNECTED:
				std::cout << "We are already connected to " <<  packet->systemAddress.ToString () <<  "." << std::endl;
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				std::cout <<  "ERROR: Failed to connect to " <<  packet->systemAddress.ToString () <<  "." << std::endl;
				return false;
			case ID_CONNECTION_LOST:
				std::cout << "Lost connection to " <<  packet->systemAddress.ToString () <<  "." << std::endl;
				break;
			case PACKET_READY:
				std::cout << "Received \"ready\" message from " <<  packet->systemAddress.ToString () <<  "." << std::endl;
				ProcessReadyMessage(packet);
				break;
            case PACKET_GUESS_REPLY:
                ProcessGuessReplyMessage(packet);
                break;
            case PACKET_HIGHSCORES:
                ProcessHighscores(packet);
                break;
			default:
				std::cout << "Received unknown packet " << (unsigned short)packetidentifier << " from " <<  packet->systemAddress.ToString () <<  "." << std::endl;
				break;

		}

		peerinterface->DeallocatePacket (packet);
		packet = peerinterface->Receive ();
	}

	return true;
}

void GuessingClient::SendIntroductionMessage(std::string& playerName)
{
	if (!connected)
	{
		std::cout << "We are not connected, can't send message!" << std::endl;
		return;
	}

	BitStream intro;
	unsigned char identifier = PACKET_INTRODUCTION;
	unsigned short nameLength = playerName.length();
	intro.Write(identifier);

	if (nameLength > 0)
	{
        intro.Write(nameLength);
        intro.Write(playerName.data(), nameLength);
	}

	peerinterface->Send(&intro, HIGH_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
}

void GuessingClient::ProcessReadyMessage (Packet* packet)
{
	assert (packet);
	BitStream ready(packet->data, packet->bitSize, false);
	unsigned char mt;
	ready.Read (mt);

	if (mt != PACKET_READY)
	{
		std::cout << "ERROR: Packet header is not PACKET_READY. Skipping processing." << std::endl;
		return;
	}

	std::cout << "Processing ready message." << std::endl;

	readyToPlay = true;
}

void GuessingClient::Disconnect (void)
{
	if (peerinterface)
	{
		std::cout << "Closing connections." << std::endl;
		peerinterface->Shutdown (1000);
		RakPeerInterface::DestroyInstance(peerinterface);
		peerinterface = 0;
	}

	connected = false;
	readyToPlay = false;
	serverAddress = UNASSIGNED_SYSTEM_ADDRESS;
}

void GuessingClient::SendGuessMessage(unsigned short guess)
{
    BitStream guesszz;
	unsigned char identifier = PACKET_GUESS;
	guesszz.Write(identifier);
	guesszz.Write(guess);
	peerinterface->Send(&guesszz, HIGH_PRIORITY, RELIABLE_ORDERED, 1, serverAddress, false);
}

void GuessingClient::ProcessGuessReplyMessage(Packet* packet)
{
	assert (packet);
	BitStream reply(packet->data, packet->bitSize, false);
	unsigned char mt;
	unsigned short result;
	reply.Read(mt);
	reply.Read(result);

	if (mt != PACKET_GUESS_REPLY)
	{
		std::cout << "ERROR: Packet header is not PACKET_GUESS_REPLY. Skipping processing." << std::endl;
		return;
	}

	guessReply = result;
}

void GuessingClient::ProcessHighscores(Packet* packet)
{
	assert (packet);
	BitStream highscores(packet->data, packet->bitSize, false);
	unsigned char mt;
	unsigned short dataLength;
	highscores.Read(mt);
	highscores.Read(dataLength);
	char* data = new char[dataLength];
	highscores.Read(data, dataLength);
	std::string highscoresString(data, dataLength);
	delete [] data;
	data = 0;

	if (mt != PACKET_HIGHSCORES)
	{
		std::cout << "ERROR: Packet header is not PACKET_HIGHSCORES. Skipping processing." << std::endl;
		return;
	}

	std::cout << "\n#### Highscores start ####" << std::endl;
	std::cout << "[RANK] [NAME] [ATTEMPTS]\n" << std::endl;
	std::cout << highscoresString << std::endl;
	std::cout << "#### Highscores end   ####\n" << std::endl;

	highscoresReceived = true;
}

unsigned short GuessingClient::GetGuessReply(void)
{
    int reply = guessReply;
    guessReply = 65535;

    return reply;
}
