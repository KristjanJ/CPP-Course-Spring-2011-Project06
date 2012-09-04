#include <cstdlib>
#include <iostream>

#include "GuessingGame.h"
#include "GuessingClient.h"

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Required: server address as parameter" << std::endl;
		return EXIT_FAILURE;
	}

	GuessingClient client;

	if (!client.Connect (std::string(argv[1]), 31337))
	{
		return EXIT_FAILURE;
	}

	while (!client.IsConnected ())
	{
		if (!client.NextFrame ())
		{
			return EXIT_FAILURE;
		}

		RakSleep (30);
	}

	std::cout << "Successfully connected to the server. Sending introduction." << std::endl;

	std::string playerName("");

    if (argc >= 4)
    {
        playerName = argv[3];
    }

	client.SendIntroductionMessage(playerName);
	bool ready = false;

	while (!ready)
	{
		if (!client.NextFrame())
		{
			return EXIT_FAILURE;
		}

		ready = (client.IsReadyToPlay() && playerName == "") || (client.IsReadyToPlay() && client.HighscoresReceived());

		RakSleep (30);
	}

	std::cout << "Server is ready! We can now start sending guesses.\n" << std::endl;

    bool guessing = true;
    unsigned short guess = 0;
    unsigned short guessReply = 0;
    unsigned int attempts = 0;

    if (argc >= 3 && std::string(argv[2]) == "bot")
	{
	    int lower = 0;
        int upper = 99;

	    while (guessing)
	    {
            guess = lower + (upper - lower) / 2;

            std::cout << "Trying " << guess << "." << std::endl;
            client.SendGuessMessage(guess);

            while ((guessReply = client.GetGuessReply()) == 65535)
            {
                if (!client.NextFrame ())
                {
                    return EXIT_FAILURE;
                }

                RakSleep(30);
            }

            switch (guessReply)
            {
            case GUESS_TOO_SMALL:
                lower = guess + 1;
                upper = upper;
                break;
            case GUESS_TOO_LARGE:
                lower = lower;
                upper = guess - 1;
                break;
            case GUESS_CORRECT:
                guessing = false;
                break;
            default:
                break;
            }

            attempts++;
	    }
	}
	else
	{
	    while (guessing)
        {
            std::cout << "Enter guess to send: ";
            std::cin >> guess;

            if (!std::cin.good())
            {
                std::cin.clear();
                std::cout << "Invalid input." << std::endl;
                continue;
            }

            client.SendGuessMessage(guess);

            while ((guessReply = client.GetGuessReply()) == 65535)
            {
                if (!client.NextFrame ())
                {
                    return EXIT_FAILURE;
                }

                RakSleep(30);
            }

            switch (guessReply)
            {
            case GUESS_TOO_SMALL:
                std::cout << "The guess of " << guess << " is too small." << std::endl;
                break;
            case GUESS_TOO_LARGE:
                std::cout << "The guess of " << guess << " is too large." << std::endl;
                break;
            case GUESS_CORRECT:
                guessing = false;
                break;
            default:
                break;
            }

            attempts++;
        }
	}

    std::cout << "\nFINAL RESULTS. CORRECT ANSWER: " << guess << ". ATTEMPTS: " << attempts << "." << std::endl;
	client.Disconnect ();

	return EXIT_SUCCESS;
}
