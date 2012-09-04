#ifndef GUESSINGCLIENT_H_INCLUDED
#define GUESSINGCLIENT_H_INCLUDED

#include <string>

#include "RakPeerInterface.h"
#include "BitStream.h"
#include "StringCompressor.h"
#include "MessageIdentifiers.h"
#include "RakSleep.h"

using namespace RakNet;

/*!
\brief This class contains everything needed to connect and play a game.
*/
class GuessingClient
{
	private:
		RakPeerInterface* peerinterface;
		SystemAddress serverAddress;
		bool connected;
		bool readyToPlay;
		bool highscoresReceived;
		unsigned short guessReply;

		unsigned char GetPacketIdentifier(Packet* packet);
		void ProcessReadyMessage(Packet* packet);
		void ProcessGuessReplyMessage(Packet* packet);
		void ProcessHighscores(Packet* packet);

	public:
        /*!
        \details The constructor sets the peerinterface pointer to zero.
        */
		GuessingClient(void);

		/*!
        \details The destructor attempts to stop the server if it hasn't been done yet.
        */
		~GuessingClient(void);

		/*!
        \details This method connects to a server.
        \param address IP address.
        \param port Port number.
        \returns true on success, false otherwize.
        */
		bool Connect (std::string address, unsigned short port);

		/*!
        \details This method should be called every once in a while to process messages.
        \returns false on error, true otherwize.
        */
		bool NextFrame(void);

		/*!
        \details This method sends the introduction message to the server.
        \param playerName Player name, if empty no name will be sent. If name is sent the client will assume the server also provides highscores.
        */
		void SendIntroductionMessage(std::string& playerName);

		/*!
        \details This method disconnects from the server.
        */
		void Disconnect(void);

		/*!
        \details Returns true, if we are connected and can send the introduction.
        \returns True, if we are connected and can send the introduction.
        */
		inline bool IsConnected(void) { return connected; }

		/*!
        \details Returns true, if the player can start guessing.
        \returns True, if the player can start guessing.
        */
		inline bool IsReadyToPlay(void) { return readyToPlay; }

		/*!
        \details Returns true, if highscores have arrived.
        \returns True, if highscores have arrived.
        */
		inline bool HighscoresReceived(void) { return highscoresReceived; }

		/*!
        \details Sends a guess message.
        \param guess Guess.
        */
		void SendGuessMessage(unsigned short guess);

		/*!
        \details Returns the latest guess reply. Reply will be reset after this method call.
        \returns The latest guess reply. If there's no reply 65535 will be returned.
        */
		unsigned short GetGuessReply(void);
};

#endif // GUESSINGCLIENT_H_INCLUDED
