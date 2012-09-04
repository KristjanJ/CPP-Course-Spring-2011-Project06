#include <ctime>
#include <cstdlib>

#include "GuessingGame.h"
#include "GuessingServer.h"


int main()
{
	srand (time (0));
	
	GuessingServer server;
	server.Start ();

	while (true) {
		server.NextFrame ();
		RakSleep(30);
	}

	server.Stop ();
}
