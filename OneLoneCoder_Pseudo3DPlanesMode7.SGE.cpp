/*
OneLoneCoder.com - Programming Pseudo 3D planes, aka MODE7
"The SNES was too advanced for its time" - @Javidx9

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
Pseudo 3D was a technique made popular by the Super Nintendo
in games such as Super Mario Kart. This code demonstrates the
effect of that hardware mode.

Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/ybLZyY655iY

Last Updated: 15/04/2018
*/


/* Alterations Joseph21 - 2022-07-18
 *   - changed path where sprites reside
 * Port to PixelGameEngine - 2022-07-18
 *   - see cheat sheet for generic adaptations needed for port
 *   - no specific points
 * Port to SDL-GameEngine - 2022-12-09
 *   - generic port actions, no specifics needed
 */


#include "SGE/SGE_Core.h"

class OneLoneCoder_FakeMode7 : public flc::SDL_GameEngine
{
public:
	OneLoneCoder_FakeMode7()
	{
		sAppName = "Pseudo 3D Planes";
	}


private:
	float fWorldX = 1000.0f;
	float fWorldY = 1000.0f;
	float fWorldA = 0.1f;
	float fNear = 0.005f;
	float fFar = 0.03f;
	float fFoVHalf = 3.14159f / 4.0f;

	flc::Sprite *sprGround;
	flc::Sprite *sprSky;

	int nMapSize = 1024;

protected:
	virtual bool OnUserCreate()
	{
		// Create a large sprite and fill it with horizontal and vertical lines
		//sprGround = new flc::Sprite(nMapSize, nMapSize);

		//for (int x = 0; x <= nMapSize; x += 32)
		//{
		//	for (int y = 0; y < nMapSize; y++)
		//	{
		//		sprGround->SetColour(x, y, FG_MAGENTA);
		//		sprGround->SetGlyph(x, y, PIXEL_SOLID);

		//		sprGround->SetColour(x+1, y, FG_MAGENTA);
		//		sprGround->SetGlyph(x+1, y, PIXEL_SOLID);

		//		sprGround->SetColour(x-1, y, FG_MAGENTA);
		//		sprGround->SetGlyph(x-1, y, PIXEL_SOLID);

		//		sprGround->SetColour(y, x, FG_BLUE);
		//		sprGround->SetGlyph(y, x, PIXEL_SOLID);

		//		sprGround->SetColour(y, x+1, FG_BLUE);
		//		sprGround->SetGlyph(y, x+1, PIXEL_SOLID);

		//		sprGround->SetColour(y, x-1, FG_BLUE);
		//		sprGround->SetGlyph(y, x-1, PIXEL_SOLID);
		//	}
		//}

		// Simply load very large sprites from file
		sprGround = new flc::Sprite("Mode7Sprites/mariokart.png");
		sprSky    = new flc::Sprite("Mode7Sprites/sky1.png");

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{

		// Control rendering parameters dynamically
		if (GetKey(flc::Key::Q).bHeld) fNear += 0.1f * fElapsedTime;
		if (GetKey(flc::Key::A).bHeld) fNear -= 0.1f * fElapsedTime;

		if (GetKey(flc::Key::W).bHeld) fFar += 0.1f * fElapsedTime;
		if (GetKey(flc::Key::S).bHeld) fFar -= 0.1f * fElapsedTime;

		if (GetKey(flc::Key::Z).bHeld) fFoVHalf += 0.1f * fElapsedTime;
		if (GetKey(flc::Key::X).bHeld) fFoVHalf -= 0.1f * fElapsedTime;


		// Create Frustum corner points
		float fFarX1 = fWorldX + cosf(fWorldA - fFoVHalf) * fFar;
		float fFarY1 = fWorldY + sinf(fWorldA - fFoVHalf) * fFar;

		float fNearX1 = fWorldX + cosf(fWorldA - fFoVHalf) * fNear;
		float fNearY1 = fWorldY + sinf(fWorldA - fFoVHalf) * fNear;

		float fFarX2 = fWorldX + cosf(fWorldA + fFoVHalf) * fFar;
		float fFarY2 = fWorldY + sinf(fWorldA + fFoVHalf) * fFar;

		float fNearX2 = fWorldX + cosf(fWorldA + fFoVHalf) * fNear;
		float fNearY2 = fWorldY + sinf(fWorldA + fFoVHalf) * fNear;

		// Starting with furthest away line and work towards the camera point
		for (int y = 0; y < ScreenHeight() / 2; y++)
		{
			// Take a sample point for depth linearly related to rows down screen
			float fSampleDepth = (float)y / ((float)ScreenHeight() / 2.0f);

			// Use sample point in non-linear (1/x) way to enable perspective
			// and grab start and end points for lines across the screen
			float fStartX = (fFarX1 - fNearX1) / (fSampleDepth) + fNearX1;
			float fStartY = (fFarY1 - fNearY1) / (fSampleDepth) + fNearY1;
			float fEndX = (fFarX2 - fNearX2) / (fSampleDepth) + fNearX2;
			float fEndY = (fFarY2 - fNearY2) / (fSampleDepth) + fNearY2;

			// Linearly interpolate lines across the screen
			for (int x = 0; x < ScreenWidth(); x++)
			{
				float fSampleWidth = (float)x / (float)ScreenWidth();
				float fSampleX = (fEndX - fStartX) * fSampleWidth + fStartX;
				float fSampleY = (fEndY - fStartY) * fSampleWidth + fStartY;

				// Wrap sample coordinates to give "infinite" periodicity on maps
				fSampleX = fmod(fSampleX, 1.0f);
				fSampleY = fmod(fSampleY, 1.0f);

				// Sample symbol and colour from map sprite, and draw the
				// pixel to the screen
				flc::Pixel col = sprGround->Sample(fSampleX, fSampleY);
				Draw(x, y + (ScreenHeight() / 2), col);

				// Sample symbol and colour from sky sprite, we can use same
				// coordinates, but we need to draw the "inverted" y-location
				col = sprSky->Sample(fSampleX, fSampleY);
				Draw(x, (ScreenHeight() / 2) - y, col);
			}
		}

		// Draw a blanking line to fill the gap between sky and ground
		DrawLine(0, ScreenHeight() / 2, ScreenWidth(), ScreenHeight() / 2, flc::CYAN);

		// Handle user navigation with arrow keys
		if (GetKey(flc::Key::LEFT).bHeld)
			fWorldA -= 1.0f * fElapsedTime;

		if (GetKey(flc::Key::RIGHT).bHeld)
			fWorldA += 1.0f * fElapsedTime;

		if (GetKey(flc::Key::UP).bHeld)
		{
			fWorldX += cosf(fWorldA) * 0.2f * fElapsedTime;
			fWorldY += sinf(fWorldA) * 0.2f * fElapsedTime;
		}

		if (GetKey(flc::Key::DOWN).bHeld)
		{
			fWorldX -= cosf(fWorldA) * 0.2f * fElapsedTime;
			fWorldY -= sinf(fWorldA) * 0.2f * fElapsedTime;
		}

		return true;
	}
};

int main( int argc, char *argv[] )
{
	OneLoneCoder_FakeMode7 game;
	if (game.Construct(320, 240, 4, 4))
        game.Start();
	return 0;
}
