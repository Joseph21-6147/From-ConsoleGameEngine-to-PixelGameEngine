/*
	OneLoneCoder.com - Upgraded Command Line First Person Shooter (FPS) Engine
	"Bricks and Lamps people, bricks and lamps..." - @Javidx9

	License
	~~~~~~~
	One Lone Coder Console Game Engine  Copyright (C) 2018  Javidx9
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
	The FPS video was one of my first youtube videos, and I feel one of my better
	ones, but its not had the popularity it deserves. So I'm upgrading the engine
	to make it more appealling.

	IMPORTANT!!
	~~~~~~~~~~~
	You'll need the FPSSprites folder too!


	Author
	~~~~~~
	Twitter: @javidx9
	Blog: www.onelonecoder.com

	Video:
	~~~~~~
	https://youtu.be/HEb2akswCcw

	Last Updated: 23/10/2017
*/


/* Alterations Joseph21 - 20220716
 *   - removed "using namespace std" and added explicit scope resolution operator where needed
 * Port to PixelGameEngine - 20220716
 *   - see cheat sheet for generic adaptations needed for port
 *   - resized screen and pixel size
 *   - converted .spr files to .png
 *   - adapted the drawing of the top down map using FillRect() calls
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <list>

#include "SGE/SGE_Core.h"

//using namespace std;

class OneLoneCoder_UltimateFPS : public flc::SDL_GameEngine
{
public:
	OneLoneCoder_UltimateFPS()
	{
		sAppName = "Ultimate First Person Shooter";
	}

private:
	int nMapWidth = 32;				// World Dimensions
	int nMapHeight = 32;

	float fPlayerX = 14.7f;			// Player Start Position
	float fPlayerY = 8;// 5.09f;
	float fPlayerA = -3.14159f / 2.0f;			// Player Start Rotation
	float fFOV = 3.14159f / 4.0f;	// Field of View
	float fDepth = 16.0f;			// Maximum rendering distance
	float fSpeed = 5.0f;			// Walking Speed
	std::string map;

	flc::Sprite	*spriteWall;
	flc::Sprite	*spriteLamp;
	flc::Sprite	*spriteFireBall;

	float *fDepthBuffer = nullptr;

	struct sObject
	{
		float x;
		float y;
		float vx;
		float vy;
		bool bRemove;
		flc::Sprite *sprite;
	};

	std::list<sObject> listObjects;

protected:
	virtual bool OnUserCreate()
	{
		map += "#########.......#########.......";
		map += "#...............#...............";
		map += "#.......#########.......########";
		map += "#..............##..............#";
		map += "#......##......##......##......#";
		map += "#......##..............##......#";
		map += "#..............##..............#";
		map += "###............####............#";
		map += "##.............###.............#";
		map += "#............####............###";
		map += "#..............................#";
		map += "#..............##..............#";
		map += "#..............##..............#";
		map += "#...........#####...........####";
		map += "#..............................#";
		map += "###..####....########....#######";
		map += "####.####.......######..........";
		map += "#...............#...............";
		map += "#.......#########.......##..####";
		map += "#..............##..............#";
		map += "#......##......##.......#......#";
		map += "#......##......##......##......#";
		map += "#..............##..............#";
		map += "###............####............#";
		map += "##.............###.............#";
		map += "#............####............###";
		map += "#..............................#";
		map += "#..............................#";
		map += "#..............##..............#";
		map += "#...........##..............####";
		map += "#..............##..............#";
		map += "################################";


		spriteWall = new flc::Sprite("FPSSprites/fps_wall1.png");
		if (spriteWall == nullptr)
            std::cout << "ERROR: OnUserCreate() --> can't read sprite file" << std::endl;
        else
            std::cout << "OnUserCreate() --> sprite file read succesfully" << std::endl;

		spriteLamp = new flc::Sprite("FPSSprites/fps_lamp1.png");
		spriteFireBall = new flc::Sprite("FPSSprites/fps_fireball1.png");
		fDepthBuffer = new float[ScreenWidth()];

		listObjects = {
			{ 8.5f, 8.5f, 0.0f, 0.0f, false, spriteLamp },
			{ 7.5f, 7.5f, 0.0f, 0.0f, false, spriteLamp },
			{ 10.5f, 3.5f, 0.0f, 0.0f, false, spriteLamp },
		};
		return true;
	}


	virtual bool OnUserUpdate(float fElapsedTime)
	{
		// Handle CCW Rotation
		if (GetKey(flc::Key::A).bHeld)
			fPlayerA -= (fSpeed * 0.5f) * fElapsedTime;

		// Handle CW Rotation
		if (GetKey(flc::Key::D).bHeld)
			fPlayerA += (fSpeed * 0.5f) * fElapsedTime;

		// Handle Forwards movement & collision
		if (GetKey(flc::Key::W).bHeld)
		{
			fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;;
			fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;;
				fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;;
			}
		}

		// Handle backwards movement & collision
		if (GetKey(flc::Key::S).bHeld)
		{
			fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;;
			fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;;
				fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;;
			}
		}

		// Handle Strafe Right movement & collision
		if (GetKey(flc::Key::E).bHeld)
		{
			fPlayerX += cosf(fPlayerA) * fSpeed * fElapsedTime;
			fPlayerY -= sinf(fPlayerA) * fSpeed * fElapsedTime;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX -= cosf(fPlayerA) * fSpeed * fElapsedTime;
				fPlayerY += sinf(fPlayerA) * fSpeed * fElapsedTime;
			}
		}

		// Handle Strafe Left movement & collision
		if (GetKey(flc::Key::Q).bHeld)
		{
			fPlayerX -= cosf(fPlayerA) * fSpeed * fElapsedTime;
			fPlayerY += sinf(fPlayerA) * fSpeed * fElapsedTime;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX += cosf(fPlayerA) * fSpeed * fElapsedTime;
				fPlayerY -= sinf(fPlayerA) * fSpeed * fElapsedTime;
			}
		}

		// Fire Bullets
		if (GetKey(flc::Key::SPACE).bReleased)
		{
			sObject o;
			o.x = fPlayerX;
			o.y = fPlayerY;
			float fNoise = (((float)rand() / (float)RAND_MAX) - 0.5f) * 0.1f;
			o.vx = sinf(fPlayerA + fNoise) * 8.0f;
			o.vy = cosf(fPlayerA + fNoise) * 8.0f;
			o.sprite = spriteFireBall;
			o.bRemove = false;
			listObjects.push_back(o);
		}

		for (int x = 0; x < ScreenWidth(); x++)
		{
			// For each column, calculate the projected ray angle into world space
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)ScreenWidth()) * fFOV;

			// Find distance to wall
			float fStepSize = 0.01f;	  // Increment size for ray casting, decrease to increase
			float fDistanceToWall = 0.0f; //                                      resolution

			bool bHitWall = false;		// Set when ray hits wall block
			bool bBoundary = false;		// Set when ray hits boundary between two wall blocks

			float fEyeX = sinf(fRayAngle); // Unit vector for ray in player space
			float fEyeY = cosf(fRayAngle);

			float fSampleX = 0.0f;

			bool bLit = false;

			// Incrementally cast ray from player, along ray angle, testing for
			// intersection with a block
			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += fStepSize;
				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

				// Test if ray is out of bounds
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitWall = true;			// Just set distance to maximum depth
					fDistanceToWall = fDepth;
				}
				else
				{
					// Ray is inbounds so test to see if the ray cell is a wall block
					if (map.c_str()[nTestX * nMapWidth + nTestY] == '#')
					{
						// Ray has hit wall
						bHitWall = true;

						// Determine where ray has hit wall. Break Block boundary
						// int 4 line segments
						float fBlockMidX = (float)nTestX + 0.5f;
						float fBlockMidY = (float)nTestY + 0.5f;

						float fTestPointX = fPlayerX + fEyeX * fDistanceToWall;
						float fTestPointY = fPlayerY + fEyeY * fDistanceToWall;

						float fTestAngle = atan2f((fTestPointY - fBlockMidY), (fTestPointX - fBlockMidX));

						if (fTestAngle >= -3.14159f * 0.25f && fTestAngle < 3.14159f * 0.25f)
							fSampleX = fTestPointY - (float)nTestY;
						if (fTestAngle >= 3.14159f * 0.25f && fTestAngle < 3.14159f * 0.75f)
							fSampleX = fTestPointX - (float)nTestX;
						if (fTestAngle < -3.14159f * 0.25f && fTestAngle >= -3.14159f * 0.75f)
							fSampleX = fTestPointX - (float)nTestX;
						if (fTestAngle >= 3.14159f * 0.75f || fTestAngle < -3.14159f * 0.75f)
							fSampleX = fTestPointY - (float)nTestY;
					}
				}
			}

			// Calculate distance to ceiling and floor
			int nCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)fDistanceToWall);
			int nFloor = ScreenHeight() - nCeiling;

			// Update Depth Buffer
			fDepthBuffer[x] = fDistanceToWall;

			for (int y = 0; y < ScreenHeight(); y++)
			{
				// Each Row
				if (y <= nCeiling)
					Draw(x, y, ' ');
				else if (y > nCeiling && y <= nFloor)
				{
					// Draw Wall
					if (fDistanceToWall < fDepth)
					{
						float fSampleY = ((float)y - (float)nCeiling) / ((float)nFloor - (float)nCeiling);
						Draw(x, y, spriteWall->Sample(fSampleX, fSampleY));
					}
					else
						Draw(x, y, flc::BLACK);
				}
				else // Floor
				{
					Draw(x, y, flc::DARK_GREEN);
				}
			}
		}

		// Update & Draw Objects
		for (auto &object : listObjects)
		{
			// Update Object Physics
			object.x += object.vx * fElapsedTime;
			object.y += object.vy * fElapsedTime;

			// Check if object is inside wall - set flag for removal
			if (map.c_str()[(int)object.x * nMapWidth + (int)object.y] == '#')
				object.bRemove = true;

			// Can object be seen?
			float fVecX = object.x - fPlayerX;
			float fVecY = object.y - fPlayerY;
			float fDistanceFromPlayer = sqrtf(fVecX*fVecX + fVecY*fVecY);

			float fEyeX = sinf(fPlayerA);
			float fEyeY = cosf(fPlayerA);

			// Calculate angle between lamp and players feet, and players looking direction
			// to determine if the lamp is in the players field of view
			float fObjectAngle = atan2f(fEyeY, fEyeX) - atan2f(fVecY, fVecX);
			if (fObjectAngle < -3.14159f)
				fObjectAngle += 2.0f * 3.14159f;
			if (fObjectAngle > 3.14159f)
				fObjectAngle -= 2.0f * 3.14159f;

			bool bInPlayerFOV = fabs(fObjectAngle) < fFOV / 2.0f;

			if (bInPlayerFOV && fDistanceFromPlayer >= 0.5f && fDistanceFromPlayer < fDepth && !object.bRemove)
			{
				float fObjectCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)fDistanceFromPlayer);
				float fObjectFloor = ScreenHeight() - fObjectCeiling;
				float fObjectHeight = fObjectFloor - fObjectCeiling;
				float fObjectAspectRatio = (float)object.sprite->height / (float)object.sprite->width;
				float fObjectWidth = fObjectHeight / fObjectAspectRatio;
				float fMiddleOfObject = (0.5f * (fObjectAngle / (fFOV / 2.0f)) + 0.5f) * (float)ScreenWidth();

				// Draw Lamp
				for (float lx = 0; lx < fObjectWidth; lx++)
				{
					for (float ly = 0; ly < fObjectHeight; ly++)
					{
						float fSampleX = lx / fObjectWidth;
						float fSampleY = ly / fObjectHeight;
						flc::Pixel c = object.sprite->Sample(fSampleX, fSampleY);
						int nObjectColumn = (int)(fMiddleOfObject + lx - (fObjectWidth / 2.0f));
						if (nObjectColumn >= 0 && nObjectColumn < ScreenWidth())
							if (c != flc::BLACK && fDepthBuffer[nObjectColumn] >= fDistanceFromPlayer)
							{
								Draw(nObjectColumn, fObjectCeiling + ly, c);
								fDepthBuffer[nObjectColumn] = fDistanceFromPlayer;
							}
					}
				}
			}
		}

		// Remove dead objects from object list
		listObjects.remove_if([](sObject &o) {return o.bRemove; });

		// Display Map & Player
		int nMapScale = 2;
		for (int nx = 0; nx < nMapWidth; nx++)
			for (int ny = 0; ny < nMapWidth; ny++)
                FillRect((nx+1) * nMapScale, (ny+1) * nMapScale, nMapScale, nMapScale, map[ny * nMapWidth + nx] == '#' ? flc::WHITE : flc::VERY_DARK_GREY);

		FillRect( (1 + (int)fPlayerY) * nMapScale, (1 + (int)fPlayerX) * nMapScale, nMapScale, nMapScale, flc::YELLOW);

		return true;
	}

};

int main( int argc, char *argv[] )
{
	OneLoneCoder_UltimateFPS game;
	if (game.Construct(640, 400, 2, 2))
        game.Start();
	return 0;
}
