/*
OneLoneCoder.com - What are Cellular Automata? John Conway's Game Of Life
"Get lost..." - @Javidx9

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
Sometimes, its worth reflecting on the fact that from simple things can emerge
significant beauty. This is an implementaion of Game Of Life that runs in the console

Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/E7CxMHsYzSs

Last Updated: 14/07/2017
*/


/* Addition Joseph21 - 20220715
 *   - removed "using namespace std" and added explicit scope resolution operator where needed
 * Port to PixelGameEngine - 20220715
 *   - see cheat sheet for generic adaptations needed for port
 *   - no specifics
 * Port to SDL-based GameEngine - 20221204
 *   - generic port, no specifics
 */

#include <iostream>

#include "SGE/SGE_Core.h"

class OneLoneCoder_GameOfLife : public flc::SDL_GameEngine
{
public:
	OneLoneCoder_GameOfLife()
	{
		sAppName = "Game Of Life";
	}

private:
	int *m_output;
	int *m_state;



protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		m_output = new int[ScreenWidth() * ScreenHeight()];
		m_state = new int[ScreenWidth() * ScreenHeight()];

		memset(m_output, 0, ScreenWidth() * ScreenHeight() * sizeof(int));
		memset(m_state, 0, ScreenWidth() * ScreenHeight() * sizeof(int));

		for (int i = 0; i < ScreenWidth()*ScreenHeight(); i++)
			m_state[i] = rand() % 2;

		auto set = [&](int x, int y, std::string s)
		{
			int p = 0;
			for (auto c : s)
			{
				m_state[y*ScreenWidth() + x + p] = c == '#' ? 1 : 0;
				p++;
			}
		};

		/*
		// R-Pentomino
		set(80, 50, "  ## ");
		set(80, 51, " ##  ");
		set(80, 52, "  #  ");

		// Gosper Glider Gun
		set(60, 45, "........................#............");
		set(60, 46, "......................#.#............");
		set(60, 47, "............##......##............##.");
		set(60, 48, "...........#...#....##............##.");
		set(60, 49, "##........#.....#...##...............");
		set(60, 50, "##........#...#.##....#.#............");
		set(60, 51, "..........#.....#.......#............");
		set(60, 52, "...........#...#.....................");
		set(60, 53, "............##.......................");

		// Infinite Growth
		set(20, 50, "########.#####...###......#######.#####");
		*/

		return true;
	}



	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		//this_thread::sleep_for(50ms);

		//if (!m_keys[VK_SPACE].bReleased)
		//	return true;

		auto cell = [&](int x, int y)
		{
			return m_output[y * ScreenWidth() + x];
		};

		// Store output state
		for (int i = 0; i < ScreenWidth()*ScreenHeight(); i++)
			m_output[i] = m_state[i];

		for (int x = 1; x < ScreenWidth() - 1; x++)
			for (int y = 1; y < ScreenHeight() - 1; y++)
			{
				// The secret of artificial life =================================================
				int nNeighbours =	cell(x - 1, y - 1) + cell(x - 0, y - 1) + cell(x + 1, y - 1) +
									cell(x - 1, y + 0) +          0         + cell(x + 1, y + 0) +
									cell(x - 1, y +	1) + cell(x + 0, y + 1) + cell(x + 1, y + 1);

				if (cell(x, y) == 1)
					m_state[y*ScreenWidth() + x] = nNeighbours == 2 || nNeighbours == 3;
				else
					m_state[y*ScreenWidth() + x] = nNeighbours == 3;
				// ===============================================================================


				if (cell(x, y) == 1)
					Draw(x, y, flc::WHITE);
				else
					Draw(x, y, flc::BLACK);
			}

		return true;
	}
};


int main( int argc, char *argv[] )
{
	// Seed random number generator
	srand(clock());

	// Use SDL-GameEngine derived app
	OneLoneCoder_GameOfLife game;
	if (game.Construct(160, 100, 8, 8))
        game.Start();

	return 0;
}
