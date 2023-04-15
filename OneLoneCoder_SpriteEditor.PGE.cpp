/*
OneLoneCoder.com - Sprite Editor
"Stop Crying about Paint ya big baby" - @Javidx9

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
Editing ASCII is not a simple as it should be, especially if you want to
include all the weird characters

Controls
~~~~~~~~


Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
Several...

Last Updated:
*/

/* Addition Joseph21 - 20230415
 *   - removed "using namespace std" and added explicit scope resolution operator where needed
 * Port to PixelGameEngine - 20230415
 *   - see cheat sheet for generic adaptations needed for port
 *   Had to do quite some additional porting effort:
 *   - replaced nCurrentGlyph, nCurrentBGColour and nCurrentFGColour by olc::Pixel pCurrentColour,
 *     float fCurrentIntensity (to mimic the PIXEL_QUARTER upto PIXEL_SOLID behaviour) and
 *     int nCurrentColIndex (to mimic "paging" through the colours)
 *   - I set current filename to wip_file.png (iso fps_fireball1.spr), to prevent overwriting some original sprite file
 *   - changed screen and pixel dimensions, and multiplied all screen constants by 8
 *   - replaced a couple of Draw() calls by FillRect() calls
 *   Specifically for file writing:
 *   - added define directive on OLC_IMAGE_STB - this is necessary to enable saving to file
 *   - renamed sprite methods Load() to LoadFromFile(), and Save() to SaveToFile()
 *   NOTE - you need to have this SaveToFile() function built into your PGE version. Ping me if you need help with that.
 */

#include <iostream>
#include <string>
//using namespace std;

#define OLC_IMAGE_STB   // to make PGE use stb_image.h

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


class OneLoneCoder_SpriteEditor : public olc::PixelGameEngine
{
public:
	OneLoneCoder_SpriteEditor()
	{
		sAppName = "Sprite Editor";
	}

private:
	int nPosX = 0;
	int nPosY = 0;
	int nOffsetX = 0;
	int nOffsetY = 0;
	int nZoom = 2;

	olc::Pixel pCurrentColour = olc::RED;
	float fCurrentIntensity = 1.0f;
	int nCurrentColIndex = 4;     // corresponds with olc::RED

	olc::Sprite *sprite = nullptr;
	std::string sCurrentSpriteFile;

protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		sprite = new olc::Sprite(8, 32);
		sCurrentSpriteFile = "wip_file.png";

		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		// Zooming
		if (GetKey( olc::Key::PGDN ).bReleased)
			nZoom <<= 1;

		if (GetKey( olc::Key::PGUP ).bReleased)
			nZoom >>= 1;

		if (nZoom > 32) nZoom = 32;
		if (nZoom < 2) nZoom = 2;

		// Brushes
		if (GetKey( olc::Key::F1 ).bReleased) fCurrentIntensity = 1.00f;
		if (GetKey( olc::Key::F2 ).bReleased) fCurrentIntensity = 0.75f;
		if (GetKey( olc::Key::F3 ).bReleased) fCurrentIntensity = 0.50f;
		if (GetKey( olc::Key::F4 ).bReleased) fCurrentIntensity = 0.25f;

		// Colours - original (CGE) code distinguished foreground and background colour - this code doesn't

		// original colour palette was based on small integers 0-7
		// need some way to create olc::Pixel from these numbers
        auto colour_by_index = [=]( int nIndex ) {
            switch (nIndex) {
                case 0: return olc::PixelF( 0.0f, 0.0f, 0.0f );
                case 1: return olc::PixelF( 0.0f, 0.0f, 1.0f );
                case 2: return olc::PixelF( 0.0f, 1.0f, 0.0f );
                case 3: return olc::PixelF( 0.0f, 1.0f, 1.0f );
                case 4: return olc::PixelF( 1.0f, 0.0f, 0.0f );
                case 5: return olc::PixelF( 1.0f, 0.0f, 1.0f );
                case 6: return olc::PixelF( 1.0f, 1.0f, 0.0f );
                case 7: return olc::PixelF( 1.0f, 1.0f, 1.0f );
            }
            return olc::MAGENTA;
        };
		if (GetKey( olc::Key::K0 ).bReleased) { nCurrentColIndex = 0; pCurrentColour = colour_by_index( 0 ); }
		if (GetKey( olc::Key::K1 ).bReleased) { nCurrentColIndex = 1; pCurrentColour = colour_by_index( 1 ); }
		if (GetKey( olc::Key::K2 ).bReleased) { nCurrentColIndex = 2; pCurrentColour = colour_by_index( 2 ); }
		if (GetKey( olc::Key::K3 ).bReleased) { nCurrentColIndex = 3; pCurrentColour = colour_by_index( 3 ); }
		if (GetKey( olc::Key::K4 ).bReleased) { nCurrentColIndex = 4; pCurrentColour = colour_by_index( 4 ); }
		if (GetKey( olc::Key::K5 ).bReleased) { nCurrentColIndex = 5; pCurrentColour = colour_by_index( 5 ); }
		if (GetKey( olc::Key::K6 ).bReleased) { nCurrentColIndex = 6; pCurrentColour = colour_by_index( 6 ); }
		if (GetKey( olc::Key::K7 ).bReleased) { nCurrentColIndex = 7; pCurrentColour = colour_by_index( 7 ); }


		if (GetKey( olc::Key::F7 ).bReleased) {
			nCurrentColIndex--;
		}
		if (GetKey( olc::Key::F8 ).bReleased) {
			nCurrentColIndex++;
		}
		if (nCurrentColIndex < 0) nCurrentColIndex = 7;
		if (nCurrentColIndex > 7) nCurrentColIndex = 0;
		pCurrentColour = colour_by_index( nCurrentColIndex );

		// Cursing :-)
		if (GetKey( olc::Key::SHIFT ).bHeld)
		{
			if (GetKey( olc::Key::UP    ).bReleased) nOffsetY++;
			if (GetKey( olc::Key::DOWN  ).bReleased) nOffsetY--;
			if (GetKey( olc::Key::LEFT  ).bReleased) nOffsetX++;
			if (GetKey( olc::Key::RIGHT ).bReleased) nOffsetX--;
		}
		else
		{
			if (GetKey( olc::Key::UP    ).bReleased) nPosY--;
			if (GetKey( olc::Key::DOWN  ).bReleased) nPosY++;
			if (GetKey( olc::Key::LEFT  ).bReleased) nPosX--;
			if (GetKey( olc::Key::RIGHT ).bReleased) nPosX++;
		}

		if (sprite != nullptr)
		{
			if (nPosX < 0) nPosX = 0;
			if (nPosX >= sprite->width) nPosX = sprite->width - 1;
			if (nPosY < 0) nPosY = 0;
			if (nPosY >= sprite->height) nPosY = sprite->height - 1;

			if (GetKey( olc::Key::SPACE ).bReleased)
			{
				sprite->SetPixel(nPosX - 0, nPosY - 0, pCurrentColour * fCurrentIntensity );
			}

			if (GetKey( olc::Key::DEL ).bReleased)
			{
				sprite->SetPixel(nPosX - 0, nPosY - 0, olc::BLACK );
			}

			if (GetKey( olc::Key::F9 ).bReleased)
			{
				sprite->LoadFromFile(sCurrentSpriteFile);
			}

			if (GetKey( olc::Key::F10 ).bReleased)
			{
				sprite->SaveToFile(sCurrentSpriteFile);
			}
		}

		// Erase All
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

		// Draw Menu
		DrawString(8, 8, "F1 = 100%  F2 = 75%  F3 = 50%  F4 = 25%    F9 = Load File  F10 = Save File");
		for (int i = 0; i < 8; i++)
		{
			DrawString(8 + 48 * i, 24, std::to_string(i) + " = ");
            FillRect( 8 + 48 * i + 32, 24, 8, 8, colour_by_index( i ) * 1.0f );
		}

		DrawString(8, 40, "Current Brush = ");
		FillRect(136, 40, 8, 8, pCurrentColour * fCurrentIntensity);

		// Draw Canvas
		for (int x = 72; x < 138 * 8; x += 8)
		{
			FillRect(x, 8 *  9, 8, 8);
			FillRect(x, 8 * 74, 8, 8);
		}

		for (int y = 72; y < 75 * 8; y += 8)
		{
			FillRect(8 *   9, y, 8, 8);
			FillRect(8 * 138, y, 8, 8);
		}

		// Draw Visible Sprite
		if (sprite != nullptr)
		{
			int nVisiblePixelsX = 8 * 128 / nZoom;
			int nVisiblePixelsY = 8 *  64 / nZoom;

			for (int x = 0; x < nVisiblePixelsX; x++)
				for (int y = 0; y < nVisiblePixelsY; y++)
				{
					if (x - nOffsetX < sprite->width && y - nOffsetY < sprite->height && x - nOffsetX >= 0 && y - nOffsetY >= 0)
					{
						// Draw Sprite
						FillRect(8 * (x * nZoom + 10), 8 * (y*nZoom + 10), 8 * nZoom, 8 * nZoom, sprite->GetPixel(x - nOffsetX, y - nOffsetY));

						// Draw Pixel Markers
						if (sprite->GetPixel(x - nOffsetX, y - nOffsetY) == olc::BLACK)
							DrawString(8 * ((x)* nZoom + 10), 8 * ((y)* nZoom + 10), ".");
					}

					if (x - nOffsetX == nPosX && y - nOffsetY == nPosY)
						DrawString(8 * ((x)* nZoom + 10), 8 * ((y)* nZoom + 10), "O");
				}
		}

		// Draw Actual Sprite
		for (int x = 0; x < sprite->width; x++)
			for (int y = 0; y < sprite->height; y++)
				Draw(x + 80, y + 640, sprite->GetPixel(x, y));

		return true;
	}
};


int main()
{
	// Use olc::PixelGameEngine derived app
	OneLoneCoder_SpriteEditor game;
	if (game.Construct(1280, 800, 1, 1))
        game.Start();

	return 0;
}
