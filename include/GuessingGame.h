#ifndef GUESSINGGAME_H_INCLUDED
#define GUESSINGGAME_H_INCLUDED

// This header contains common constants and structures for the Guessing game
// client and server. This sets the protocol understood by both parties.

// enum is used for definining groups of constants. You can give each constant
// an explicit value or let the compiler do it.

#include "MessageIdentifiers.h"

// GuessResults enumerates possible answers from the server
enum
{
	GUESS_TOO_LARGE = 1,
	GUESS_TOO_SMALL = 2,
	GUESS_CORRECT = 3
};

// Now we define packet headers. Our first header has the ID
// of the first free packet header ID as defined by RakNet
enum
{
	PACKET_INTRODUCTION = ID_USER_PACKET_ENUM,
	PACKET_READY,
	PACKET_GUESS,
	PACKET_GUESS_REPLY,
	PACKET_HIGHSCORES
};

#endif // GUESSINGGAME_H_INCLUDED
