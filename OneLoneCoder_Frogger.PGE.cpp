/*
OneLoneCoder.com - Code-It-Yourself! Frogger at the command prompt (quick and simple c++)
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
This is a bit of an unusual one. The games mechanics are very simple, but as soon
as you add graphics the code almost doubles in size! By standardising how the
lanes work and interact with the frog, the game pretty much controls itself, meaning
the game code is really really short. Everything else is eye candy.

Controls
~~~~~~~~
Arrow keys move frog. Cross the road, then the river, to get frog to safety.

Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/QJnZ5QmpXOE

Last Updated: 20/08/2017
*/


/* Addition Joseph21 - 20220715
 *   - removed "using namespace std" and added explicit scope resolution operator where needed
 * Port to PixelGameEngine - 20220715
 *   - see cheat sheet for generic adaptations needed for port
 *   - use .png sprites instead of .spr sprites
 *   - draw the frog in olc::Pixel::MASK mode for transparency
 */

#include <iostream>
#include <string>
//using namespace std;

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


class OneLoneCoder_Frogger : public olc::PixelGameEngine
{
public:
	OneLoneCoder_Frogger()
	{
		sAppName = "Frogger";
	}

private:
	std::vector<std::pair<float, std::string>> vecLanes =
	{
		{0.0f,  "wwwhhwwwhhwwwhhwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww"}, // 64 elements per lane
		{-3.0f, ",,,jllk,,jllllk,,,,,,,jllk,,,,,jk,,,jlllk,,,,jllllk,,,,jlllk,,,,"},
		{3.0f,  ",,,,jllk,,,,,jllk,,,,jllk,,,,,,,,,jllk,,,,,jk,,,,,,jllllk,,,,,,,"},
		{2.0f,  ",,jlk,,,,,jlk,,,,,jk,,,,,jlk,,,jlk,,,,jk,,,,jllk,,,,jk,,,,,,jk,,"},
		{0.0f,  "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp"},
		{-3.0f, "....asdf.......asdf....asdf..........asdf........asdf....asdf..."},
		{ 3.0f, ".....ty..ty....ty....ty.....ty........ty..ty.ty......ty.......ty"},
		{-4.0f, "..zx.....zx.........zx..zx........zx...zx...zx....zx...zx...zx.."},
		{2.0f,  "..ty.....ty.......ty.....ty......ty..ty.ty.......ty....ty......."},
		{0.0f,  "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp"}
	};

	float fTimeSinceStart = 0.0f;
	float fFrogX = 8.0f;
	float fFrogY = 9.0f;
	int nLaneWidth = 18;
	int nCellSize = 8;

	bool *bufDanger = nullptr;

	olc::Sprite *spriteBus = nullptr;
	olc::Sprite *spriteLog = nullptr;
	olc::Sprite *spriteWater = nullptr;
	olc::Sprite *spriteFrog = nullptr;
	olc::Sprite *spritePavement = nullptr;
	olc::Sprite *spriteCar1 = nullptr;
	olc::Sprite *spriteCar2 = nullptr;
	olc::Sprite *spriteWall = nullptr;
	olc::Sprite *spriteHome = nullptr;

protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		bufDanger = new bool[ScreenWidth() * ScreenHeight()];
		memset(bufDanger, 0, ScreenWidth() * ScreenHeight() * sizeof(bool));

		// You'll need these sprites!

		spriteBus = new olc::Sprite("FroggerSprites/bus.png");
		spriteLog = new olc::Sprite("FroggerSprites/log.png");
		spriteWater = new olc::Sprite("FroggerSprites/water.png");
		spriteFrog = new olc::Sprite("FroggerSprites/frog.png");
		spritePavement = new olc::Sprite("FroggerSprites/pavement.png");
		spriteWall = new olc::Sprite("FroggerSprites/wall.png");
		spriteHome = new olc::Sprite("FroggerSprites/home.png");
		spriteCar1 = new olc::Sprite("FroggerSprites/car1.png");
		spriteCar2 = new olc::Sprite("FroggerSprites/car2.png");
		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		fTimeSinceStart += fElapsedTime;

		// Handle Input
		if (GetKey(olc::Key::UP).bReleased)	fFrogY -= 1.0f;
		if (GetKey(olc::Key::DOWN).bReleased)	fFrogY += 1.0f;
		if (GetKey(olc::Key::LEFT).bReleased)	fFrogX -= 1.0f;
		if (GetKey(olc::Key::RIGHT).bReleased)	fFrogX += 1.0f;

		// Frog is moved by platforms
		if (fFrogY <= 3)	fFrogX -= fElapsedTime * vecLanes[(int)fFrogY].first;

		// Collision detection - check four corners of frog against danger buffer
		bool tl = bufDanger[(int)(fFrogY*nCellSize + 1) * ScreenWidth() + (int)(fFrogX*nCellSize + 1)];
		bool tr = bufDanger[(int)(fFrogY*nCellSize + 1) * ScreenWidth() + (int)((fFrogX+1)*nCellSize - 1)];
		bool bl = bufDanger[(int)((fFrogY+1)*nCellSize - 1) * ScreenWidth() + (int)(fFrogX*nCellSize + 1)];
		bool br = bufDanger[(int)((fFrogY+1)*nCellSize - 1) * ScreenWidth() + (int)((fFrogX+1)*nCellSize - 1)];

		if (tl || tr || bl || br)
		{
			// Frogs been hit :-(
			fFrogX = 8.0f;
			fFrogY = 9.0f;
		}

		// Draw Lanes
		int x=-1, y=0;
		for (auto lane : vecLanes)
		{
			// Find lane offset start
			int nStartPos = (int)(fTimeSinceStart * lane.first) % 64;
			int nCellOffset = (int)((float)nCellSize* fTimeSinceStart * lane.first) % nCellSize;
			if (nStartPos < 0)	nStartPos = 64 - (abs(nStartPos) % 64);

			for (int i = 0; i < nLaneWidth; i++)
			{
				// Draw Graphics
				char graphic = lane.second[(nStartPos + i) % 64];
				//Fill((x + i)*nCellSize - nCellOffset, y*nCellSize, (x + i + 1)*nCellSize - nCellOffset, (y + 1)*nCellSize, graphic);



				switch (graphic) // Graphics always make code verbose and less clear
				{
					case 'a':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteBus, 0, 0, 8, 8);		break; // Bus 1
					case 's':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteBus, 8, 0, 8, 8);		break; // Bus 2
					case 'd':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteBus, 16, 0, 8, 8);	break; // Bus 3
					case 'f':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteBus, 24, 0, 8, 8);	break; // Bus 4

					case 'j':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteLog, 0, 0, 8, 8);		break; // Log Start
					case 'l':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteLog, 8, 0, 8, 8);		break; // Log Middle
					case 'k':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteLog, 16, 0, 8, 8);	break; // Log End

					case 'z': 	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteCar1, 0, 0, 8, 8);	break; // Car1 Back
					case 'x':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteCar1, 8, 0, 8, 8);	break; // Car1 Front

					case 't': 	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteCar2, 0, 0, 8, 8);	break; // Car2 Back
					case 'y': 	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteCar2, 8, 0, 8, 8);	break; // Car2 Front

					case 'w': 	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteWall, 0, 0, 8, 8);	break; // Wall
					case 'h':	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteHome, 0, 0, 8, 8);	break; // Home
					case ',': 	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spriteWater, 0, 0, 8, 8);	break; // Water
					case 'p': 	DrawPartialSprite((x + i)*nCellSize - nCellOffset, y*nCellSize, spritePavement, 0, 0, 8, 8); break; // Pavement
					case '.': 	FillRect((x + i)*nCellSize - nCellOffset, y*nCellSize, nCellSize, nCellSize, olc::BLACK);	break; // Road
				}

				// Fill Danger buffer
				for (int j = (x + i)*nCellSize - nCellOffset; j < (x + i + 1)*nCellSize - nCellOffset; j++)
					for (int k = y*nCellSize; k < (y + 1)*nCellSize; k++)
						if (j >= 0 && j < ScreenWidth() && k >= 0 && k < ScreenHeight())
							bufDanger[k*ScreenWidth() + j] =
								!(graphic == '.' ||
								graphic == 'j' || graphic == 'k' || graphic == 'l' ||
								graphic == 'p' ||
								graphic == 'h');
			}
			y++;
		}

		// Draw Frog in mask mode
		SetPixelMode( olc::Pixel::MASK );
		DrawSprite(fFrogX*nCellSize, fFrogY*nCellSize, spriteFrog);
		SetPixelMode( olc::Pixel::NORMAL );
		return true;
	}
};


int main()
{
	// Use olcConsoleGameEngine derived app
	OneLoneCoder_Frogger game;
	if (game.Construct(128, 80, 12, 12))
        game.Start();
	return 0;
}
