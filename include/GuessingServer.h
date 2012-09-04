#ifndef GUESSING_SERVER_H
#define GUESSING_SERVER_H

#include <map>
#include <vector>
#include <string>

#include "RakPeerInterface.h"
#include "BitStream.h"
#include "StringCompressor.h"
#include "MessageIdentifiers.h"
#include "RakSleep.h"

using namespace RakNet;

/*!
\brief This structure stores the player info.
*/
class GuessingPlayerInfo
{
	public:
        /*!
        \details Guesses.
        */
		unsigned short guesses;

		/*!
        \details Number to guess.
        */
		unsigned short number;

		/*!
        \details Name.
        */
		std::string name;

        /*!
        \details Constructor.
        */
		GuessingPlayerInfo(void)
		{
			guesses = 0;
			number = rand () % 100;
			name = "";
		}
};

/*!
\brief This is the sort predicate for sorting highscores.
*/
class SortPredicate
{
    public:
        /*!
        \details Binary predicate.
        */
        bool operator () (const std::pair<std::string, unsigned short>& player1, const std::pair<std::string, unsigned short>& player2)
        {
            return player1.second < player2.second;
        }
};

/*!
\brief This class runs the guessing server.
*/
class GuessingServer
{
	private:
		RakPeerInterface* peerinterface;
		std::map<SystemAddress, GuessingPlayerInfo> players;
		std::vector< std::pair<std::string, unsigned short> > highscores;

		unsigned char GetPacketIdentifier(Packet* packet);
		void ProcessIntroduction(Packet* packet);
		void ProcessGuess(Packet* packet);
		void SendMessageReady(SystemAddress& recipient);
		void SendMessageGuessReply(SystemAddress& recipient, unsigned short reply);

		std::string SystemAddressOrName(SystemAddress& systemAddress);
		void UpdateHighscores(std::string& playerName, unsigned short score);
		void SendHighscores(SystemAddress& recipient);

	public:
        /*!
        \details The constructor initialises default values.
        */
		GuessingServer(void);

		/*!
        \details The destructor stops the server if it is not stopped already.
        */
		~GuessingServer(void);

        /*!
        \details Starts the server.
        \returns True on success.
        */
		bool Start(void);

		/*!
        \details Runs the next frame of the server.
        \returns True on success.
        */
		bool NextFrame(void);

		/*!
        \details Stops the server.
        */
		void Stop(void);
};

#endif // GUESSING_SERVER_H
