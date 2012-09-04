#include <iostream>
#include <map>
#include <algorithm>
#include <functional>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cassert>

#include "GuessingGame.h"
#include "GuessingServer.h"

GuessingServer::GuessingServer (void)
{
	peerinterface = 0;
}

GuessingServer::~GuessingServer (void)
{
    Stop();
}

bool GuessingServer::Start (void)
{
	peerinterface = RakPeerInterface::GetInstance();

	if (!peerinterface)
	{
		std::cout << "ERROR: Couldn't get network engine interface!" << std::endl;
		return false;
	}

	SocketDescriptor sock (31337, 0);

	int threadPriority = 0;
#ifdef _WIN32
	threadPriority = 0;
#else
	#include <sched.h>
	threadPriority = sched_get_priority_max (SCHED_FIFO);
#endif

	StartupResult sr = peerinterface->Startup (32, &sock, 1, threadPriority);

	if (sr != RAKNET_STARTED)
	{
		std::cout << "ERROR: Failed to bind to port 31337!" << std::endl;
		return false;
	}

	peerinterface->SetMaximumIncomingConnections(32);
	peerinterface->SetOccasionalPing(true);
	std::cout << "Listening at port 31337 on localhost." << std::endl;

	return true;
}

unsigned char GuessingServer::GetPacketIdentifier(Packet* packet)
{
	assert (packet);

	if ((unsigned char)packet->data[0] == ID_TIMESTAMP)
	{
		assert(packet->length > sizeof(unsigned char) + sizeof(unsigned long));
		return (unsigned char) packet->data[sizeof(unsigned char) + sizeof(unsigned long)];
	}
	else
	{
		return (unsigned char) packet->data[0];
	}
}

bool GuessingServer::NextFrame(void)
{
	assert(peerinterface);
	Packet* packet = 0;
	unsigned char packetidentifier = 0;
	packet = peerinterface->Receive();

	if (packet == 0)
	{
		return false;
	}

	while (packet != 0)
	{
		packetidentifier = GetPacketIdentifier(packet);

		switch (packetidentifier)
        {
			case ID_NEW_INCOMING_CONNECTION:
				std::cout << "Incoming connection from " << SystemAddressOrName(packet->systemAddress) <<  "." << std::endl;
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				std::cout <<  "ERROR: Failed to connect to " << SystemAddressOrName(packet->systemAddress) <<  "." << std::endl;
				return false;
            case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "Connection to " << SystemAddressOrName(packet->systemAddress) <<  " closed." << std::endl;
				break;
			case ID_CONNECTION_LOST:
				std::cout << "Lost connection to " << SystemAddressOrName(packet->systemAddress) <<  "." << std::endl;
				break;
			case PACKET_INTRODUCTION:
				std::cout << "Received introduction from " << SystemAddressOrName(packet->systemAddress) <<  "." << std::endl;
				ProcessIntroduction (packet);
				break;
			case PACKET_GUESS:
				std::cout << "Received guess from " << SystemAddressOrName(packet->systemAddress) <<  "." << std::endl;
				ProcessGuess (packet);
				break;
			default:
				std::cout << "Received unknown packet " << (unsigned short)packetidentifier << " from " << SystemAddressOrName(packet->systemAddress) <<  "." << std::endl;
				break;
		}

		peerinterface->DeallocatePacket (packet);
		packet = peerinterface->Receive ();
	}

	return true;
}

void GuessingServer::ProcessIntroduction (Packet* packet)
{
	assert(packet);

	BitStream intro (packet->data, packet->length, false);

	unsigned char mt;
	unsigned short nameLength;
	std::string name;
	intro.Read(mt);
	intro.Read(nameLength);
	char* data = new char[nameLength];
	intro.Read(data, nameLength);
	name = std::string(data, nameLength);
	delete [] data;
	data = 0;

	if (mt != PACKET_INTRODUCTION)
	{
		std::cout << "ERROR: Packet header is not PACKET_INTRODUCTION. Skipping processing." << std::endl;
		return;
	}

	std::cout << "Processing introduction message." << std::endl;
	std::map<SystemAddress, GuessingPlayerInfo>::iterator iter = players.find(packet->systemAddress);
	GuessingPlayerInfo playerinfo;
	playerinfo.name = name;

	if (iter != players.end ())
	{
		std::cout << "Player " << SystemAddressOrName(packet->systemAddress) << " reintroduced themselves, resetting the game." << std::endl;
		iter->second = playerinfo;
	}
	else
	{
		std::cout << "Starting a new game with " << SystemAddressOrName(packet->systemAddress) << "." << std::endl;
		players.insert(std::make_pair(packet->systemAddress, playerinfo));
	}

	SendMessageReady(packet->systemAddress);
	SendHighscores(packet->systemAddress);
}

void GuessingServer::ProcessGuess(Packet* packet)
{
	assert (packet);
	BitStream guess (packet->data, packet->length, false);
	unsigned char mt;
	guess.Read (mt);

	if (mt != PACKET_GUESS)
	{
		std::cout << "ERROR: Packet header is not PACKET_GUESS. Skipping processing." << std::endl;
		return;
	}

	unsigned short guessvalue;
	guess.Read (guessvalue);
	unsigned short result = GUESS_TOO_LARGE;
	std::map<SystemAddress,GuessingPlayerInfo>::iterator iter = players.find(packet->systemAddress);

	if (iter != players.end ())
	{
		std::cout << "Processing guess." << std::endl;
		GuessingPlayerInfo& plinfo  = iter->second;
		plinfo.guesses++;

		if (guessvalue > plinfo.number)
		{
			std::cout << "Player " << SystemAddressOrName(packet->systemAddress) << " has a large guess." << std::endl;
			result = GUESS_TOO_LARGE;
		}
		else if (guessvalue < plinfo.number)
		{
			std::cout << "Player " << SystemAddressOrName(packet->systemAddress) << " has a small guess." << std::endl;
			result = GUESS_TOO_SMALL;
		}
		else
		{
			std::cout << "Player " << SystemAddressOrName(packet->systemAddress) << " has a correct guess in " << plinfo.guesses << " tries." << std::endl;
			UpdateHighscores(plinfo.name, plinfo.guesses);
			result = GUESS_CORRECT;
			players.erase (iter);
		}

		SendMessageGuessReply (packet->systemAddress, result);

		if (result == GUESS_CORRECT)
		{
			peerinterface->CloseConnection (packet->systemAddress, true);
		}
	}
	else
	{
		std::cout << "The player has not intruced themselves. Guess rejected." << std::endl;
	}
}

void GuessingServer::SendMessageReady(SystemAddress& recipient)
{
	std::cout << "Sending ready message to " << SystemAddressOrName(recipient) << std::endl;
	BitStream message;
	unsigned char identifier = PACKET_READY;
	message.Write (identifier);
	peerinterface->Send(&message, HIGH_PRIORITY, RELIABLE_ORDERED, 1, recipient, false);
}

void GuessingServer::SendMessageGuessReply(SystemAddress& recipient, unsigned short result)
{
	std::cout << "Sending guess reply message to " << SystemAddressOrName(recipient) << std::endl;
	BitStream replymessage;
	unsigned char identifier = PACKET_GUESS_REPLY;
	replymessage.Write (identifier);
	replymessage.Write (result);
	peerinterface->Send(&replymessage, HIGH_PRIORITY, RELIABLE_ORDERED, 1, recipient, false);
}

void GuessingServer::Stop(void)
{
	if (peerinterface)
	{
		std::cout << "Stopping server." << std::endl;
		peerinterface->Shutdown (1000);
		RakPeerInterface::DestroyInstance(peerinterface);
		peerinterface = 0;
	}
}

std::string GuessingServer::SystemAddressOrName(SystemAddress& systemAddress)
{
    std::map<SystemAddress, GuessingPlayerInfo>::iterator iter = players.find(systemAddress);

    if (iter != players.end() && iter->second.name != "")
    {
        return iter->second.name;
    }
    else
    {
        return systemAddress.ToString();
    }
}

void GuessingServer::UpdateHighscores(std::string& playerName, unsigned short score)
{
    highscores.push_back(std::make_pair(playerName, score));
    SortPredicate sortPredicate;
    std::stable_sort(highscores.begin(), highscores.end(), sortPredicate);
}

void GuessingServer::SendHighscores(SystemAddress& recipient)
{
	std::cout << "Sending highscores message to " << SystemAddressOrName(recipient) << std::endl;

	std::string highscoresString;
	std::ostringstream outStringStream;

	if (highscores.size() > 0)
	{
	    unsigned int size = highscores.size();

	    for (unsigned int i = 0; i < size && i < 10; i++)
	    {
	        outStringStream << (i + 1);
	        outStringStream << ". ";
	        outStringStream << highscores[i].first;
	        outStringStream << " - ";
	        outStringStream << highscores[i].second;
	        outStringStream << "\n";
	    }

	    highscoresString = outStringStream.str();
	}
	else
	{
	    highscoresString = "EMPTY HIGHSCORES\n";
	}

	BitStream message;
	unsigned char identifier = PACKET_HIGHSCORES;
	unsigned short dataLength = highscoresString.length();
	message.Write (identifier);
	message.Write(dataLength);
	message.Write(highscoresString.data(), dataLength);
	peerinterface->Send(&message, HIGH_PRIORITY, RELIABLE_ORDERED, 1, recipient, false);
}

