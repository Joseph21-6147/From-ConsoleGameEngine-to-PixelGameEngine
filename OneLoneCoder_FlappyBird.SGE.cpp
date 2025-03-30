/*
OneLoneCoder.com - Code-It-Yourself! Flappy Bird
"You asked for it... wait, no one did, ever..." - @Javidx9

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
Look it's Flappy Bird, OK? Press Space Bar to Flap. Get a high score.

Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/b6A4XHkTjs8

Last Updated: 05/11/2017
*/


/* Alterations Joseph21 - 20220716
 *   - removed "using namespace std" and added explicit scope resolution operator where needed
 * Port to PixelGameEngine - 20220716
 *   - see cheat sheet for generic adaptations needed for port
 *   - resized screen and pixel size
 *   - adapted the collision detection part with GetPixel() calls to the screen sprite
 *   - since the bird is character oriented, a lot of the screen coordinates must be multiplied by 8 (character size in pixels)
 *     to get the correct result.
 * Port to SDL-GameEngine - 20220412
 *   - generic port, no specifics
 */

#include <iostream>
#include <string>
#include <list>

#include "SGE/SGE_Core.h"


class OneLoneCoder_FlappyBird : public flc::SDL_GameEngine
{
public:
	OneLoneCoder_FlappyBird()
	{
		sAppName = "Flappy Bird";
	}

private:

	float fBirdPosition = 0.0f;
	float fBirdVelocity = 0.0f;
	float fBirdAcceleration = 0.0f;
	float fGravity = 100.0f * 8.0f;
	float fLevelPosition = 0.0f;

	float fSectionWidth;
	std::list<int> listSection;

	bool bHasCollided = false;
	bool bResetGame = false;

	int nAttemptCount = 0;
	int nFlapCount = 0;
	int nMaxFlapCount = 0;


protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		listSection = { 0, 0, 0, 0 };
		bResetGame = true;
		fSectionWidth = (float)ScreenWidth() / (float)(listSection.size() - 1);
		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		if (bResetGame)
		{
			bHasCollided = false;
			bResetGame = false;
			listSection = { 0, 0, 0, 0 };
			fBirdAcceleration = 0.0f;
			fBirdVelocity = 0.0f;
			fBirdPosition = ScreenHeight() / 2.0f;
			nFlapCount = 0;
			nAttemptCount++;
		}

		// Game
		if (bHasCollided)
		{
			// Do nothing until user releases space
			if (GetKey(flc::Key::SPACE).bReleased)
				bResetGame = true;
		}
		else
		{

			if (GetKey(flc::Key::SPACE).bPressed && fBirdVelocity >= fGravity / 10.0f)
			{
				fBirdAcceleration = 0.0f;
				fBirdVelocity = -fGravity * 0.25f;
				nFlapCount++;
				if (nFlapCount > nMaxFlapCount)
					nMaxFlapCount = nFlapCount;
			}
			else
				fBirdAcceleration += fGravity * fElapsedTime;

			if (fBirdAcceleration >= fGravity)
				fBirdAcceleration = fGravity;

			fBirdVelocity += fBirdAcceleration * fElapsedTime;
			fBirdPosition += fBirdVelocity * fElapsedTime;
			fLevelPosition += 14.0f * 8.0f * fElapsedTime;

			if (fLevelPosition > fSectionWidth)
			{
				fLevelPosition -= fSectionWidth;
				listSection.pop_front();
				int i = rand() % (ScreenHeight() - 20 * 8);
				if (i <= 10 * 8) i = 0;
				listSection.push_back(i);
			}

			// Display
			FillRect(0, 0, ScreenWidth(), ScreenHeight(), flc::BLACK);

			// Draw Sections
			int nSection = 0;
			for (auto s : listSection)
			{
				if (s != 0)
				{
					FillRect(nSection * fSectionWidth + 10 * 8 - fLevelPosition, ScreenHeight() - s, 5 * 8, s, flc::GREEN);
					FillRect(nSection * fSectionWidth + 10 * 8 - fLevelPosition, 0, 5 * 8, ScreenHeight() - s - (15 * 8), flc::GREEN );
				}
				nSection++;
			}

			int nBirdX = (int)(ScreenWidth() / 3.0f);

            // Collision detection

            // grab screen sprite pointer
            flc::Sprite *screenSprite = GetDrawTarget();
            // work out bird collision box
            int nLt = nBirdX - 1;
            int nRt = nBirdX + 6 * 8;
            int nUp = (int)(fBirdPosition +  0);
            int nDn = (int)(fBirdPosition + 16);
            // check against screen boundaries and against non black pixels
            bHasCollided = fBirdPosition < 16 || fBirdPosition > ScreenHeight() - 16 ||
                    screenSprite->GetPixel( nLt, nUp ) != flc::BLACK ||
                    screenSprite->GetPixel( nLt, nDn ) != flc::BLACK ||
                    screenSprite->GetPixel( nRt, nUp ) != flc::BLACK ||
                    screenSprite->GetPixel( nRt, nDn ) != flc::BLACK ;

			// Draw Bird
			if (fBirdVelocity > 0)
			{
				DrawString(nBirdX, fBirdPosition + 0, "\\\\\\");
				DrawString(nBirdX, fBirdPosition + 8, "<\\\\\\=Q");
			}
			else
			{
				DrawString(nBirdX, fBirdPosition + 0, "<///=Q");
				DrawString(nBirdX, fBirdPosition + 8, "///");
			}

			DrawString(8, 8, "Attempt: " + std::to_string(nAttemptCount) + " Score: " + std::to_string(nFlapCount) + " High Score: " + std::to_string(nMaxFlapCount));
		}

		return true;
	}
};


int main( int argc, char *argv[] )
{
	// Use SDL-GameEngine derived app
	OneLoneCoder_FlappyBird game;
	if (game.Construct(640, 300, 2, 2))
        game.Start();

	return 0;
}
