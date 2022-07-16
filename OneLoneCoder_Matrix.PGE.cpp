/*
OneLoneCoder.com - Programming The Matrix
"Ribbet" - @Javidx9

License
~~~~~~~
Copyright (C) 2018  Javidx9
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; See license for details.
Original works located at:
https://www.github.com/onelonecoder
https://www.onelonecoder.com
https://www.youtube.com/javidx9

GNU GPLv3
https://github.com/OneLoneCoder/videos/blob/master/LICENSE

From Javidx9 :)
~~~~~~~~~~~~~~~
Hello! Ultimately I don't care what you use this for. It's intended to be
educational, and perhaps to the oddly minded - a little bit of fun.
Please hack this, change it and use it in any way you see fit. You acknowledge
that I am not responsible for anything bad that happens as a result of
your actions. However this code is protected by GNU GPLv3, see the license in the
github repo. This means you must attribute me if you use it. You can view this
license here: https://github.com/OneLoneCoder/videos/blob/master/LICENSE
Cheers!


Background
~~~~~~~~~~
I hit 0x1FF subscribers, so celebrated with a live stream!

Controls
~~~~~~~~
None, just spot the blonde, brunette, etc, etc...

Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/s7EbrvA188A


Last Updated: 17/09/2017
*/


/* Addition Joseph21 - 20220716
 *   - removed "using namespace std" and added explicit scope resolution operator where needed
 * Port to PixelGameEngine - 20220715
 *   - see cheat sheet for generic adaptations needed for port
 *   - resized screen since characters are 8x8 pixels
 *   - also adapted positioning of streamers and characters
 *   - stopped using unicode for the characters to display (random ascii is displayed atm)
 */

#include <iostream>
#include <string>
//using namespace std;

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


class OneLoneCoder_Matrix : public olc::PixelGameEngine
{
public:
	OneLoneCoder_Matrix()
	{
		sAppName = "Matrix";
	}

private:
	struct sStreamer
	{
		int nColumn = 0;
		float fPosition = 0;
		float fSpeed = 0;
		std::string sText;
	};

	std::list<sStreamer> listStreamers;

	int nMaxStreamers = 300;

	char RandomCharacter()
	{
//		return (wchar_t)(rand() % 0x1EF + 0x00C0);
		return (char)(rand() % 93 + 33); // Random ASCII
	}

	void PrepareStreamer(sStreamer *s)
	{
		s->nColumn = rand() % ScreenWidth();
		s->fPosition = 0;
		s->fSpeed = rand() % 40 + 5;
		s->sText.clear();

		int nStreamerLength = rand() % 80 + 10;
		for (int i = 0; i < nStreamerLength; i++)
			s->sText.append(1, RandomCharacter());

//		s->sText = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	}


protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		for (int n = 0; n < nMaxStreamers; n++)
		{
			sStreamer s;
			PrepareStreamer(&s);
			listStreamers.push_back(s);
		}
		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		// Clear Screen
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

		for (auto &s : listStreamers)
		{
			s.fPosition += fElapsedTime * s.fSpeed;
			for (int i = 0; i < s.sText.size(); i++)
			{
				// If you hate ternary operators do this
				// start ->
				olc::Pixel col = s.fSpeed < 15.0f ? olc::DARK_GREEN : olc::GREEN; // ;-)
				if (i == 0)
					col = olc::WHITE;
				else
					if (i <= 3)
						col = olc::GREY;

				int nCharIndex = (i - (int)s.fPosition) % s.sText.size();

                DrawString( s.nColumn, 8 * ((int)s.fPosition - i), std::string( 1, s.sText[nCharIndex] ), col );
				// <- end

				// If you like them, do this!
				//Draw(s.nColumn, 8 * (int)s.fPosition - i, s.sText[(abs(i - (int)s.fPosition) % s.sText.size())], i == 0 ? olc::WHITE : i<=3 ? olc::GREY: i>=s.sText.size()-2 || s.fSpeed < 15.0f? olc::DARK_GREEN: olc::GREEN);

				// Missed this out on live stream, occasionally glitch a character
				if (rand() % 1000 < 5)
					s.sText[i] = RandomCharacter();
			}

			if ( 8 * (s.fPosition - s.sText.size()) >= ScreenHeight())
				PrepareStreamer(&s);

		}
		return true;
	}
};


int main()
{
	// Use olcConsoleGameEngine derived app
	OneLoneCoder_Matrix game;
	if (game.Construct(1000, 600, 1, 1))
        game.Start();
	return 0;
}
