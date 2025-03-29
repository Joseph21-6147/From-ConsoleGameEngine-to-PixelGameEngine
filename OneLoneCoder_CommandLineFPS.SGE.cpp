/*
	OneLoneCoder.com - Command Line First Person Shooter (FPS) Engine
	"Why were games not done like this is 1990?" - @Javidx9

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
	Whilst waiting for TheMexicanRunner to start the finale of his NesMania project,
	his Twitch stream had a counter counting down for a couple of hours until it started.
	With some time on my hands, I thought it might be fun to see what the graphical
	capabilities of the console are. Turns out, not very much, but hey, it's nice to think
	Wolfenstein could have existed a few years earlier, and in just ~200 lines of code.

	IMPORTANT!!!!
	~~~~~~~~~~~~~
	READ ME BEFORE RUNNING!!! This program expects the console dimensions to be set to
	120 Columns by 40 Rows. I recommend a small font "Consolas" at size 16. You can do this
	by running the program, and right clicking on the console title bar, and specifying
	the properties. You can also choose to default to them in the future.

	Future Modifications
	~~~~~~~~~~~~~~~~~~~~
	1) Shade block segments based on angle from player, i.e. less light reflected off
	walls at side of player. Walls straight on are brightest.
	2) Find an interesting and optimised ray-tracing method. I'm sure one must exist
	to more optimally search the map space
	3) Add bullets!
	4) Add bad guys!

	Author
	~~~~~~
	Twitter: @javidx9
	Blog: www.onelonecoder.com

	Video:
	~~~~~~
	xxxxxxx

	Last Updated: 27/02/2017
*/


/* Alterations to original (console) code - Joseph21 - 2022-07-30
 *   - removed "using namespace std" and added explicit scope resolution operator where needed
 *   - added include to cmath to enable calls to sqrt(), sinf(), cosf() and acos()
 * Port to ConsoleGameEngine - 2022-07-30
 *   - removed a number of include directives since they are already done in olcConsoleGameEngine.h
 *   - Created a class FPS_CGE and put most of the before global variables in it as class variables
 *   - Added OnUserCreate() and put map initialization in it
 *   - Added OnUserUpdate() and put all user interaction in it (calls to GetAsyncKeyState() rewritten
 *     to calls to GetKey())
 *   - Copied ray casting and rendering loop from old main() to OnUserUpdate() - only adapted the
 *     unicode literal numbers into PIXEL_SOLID etc
 *   - Replaced the call to swprintf_s() with DrawString() call with the same (caption) information
 *   - Replaced all references to screen[] with calls to Draw()
 *   - Rewrote main() to construct and start the console game engine
 * Port to PixelGameEngine - 2022-07-30
 *   - standard porting stuff - see cheat sheet for generic adaptations needed for port
 *   - Rewrote shade calculations and rendering for the walls and floors
 *   - rewrote map and player drawing since glyph's need to be drawn with DrawString() now
 *   - Adapted screen resolution
 * Port to SDL2-based Game Engine (SGE) - 2022-12-04
 *   - standard porting routine
 */

#include <algorithm>

//using namespace std;

#include "SGE/SGE_Core.h"


#define PIXEL_SIZE 2

int nScreenWidth = 300 * PIXEL_SIZE;			// Screen Size X (columns)
int nScreenHeight = 160 * PIXEL_SIZE;			// Screen Size Y (rows)

class FPS_SGE : public flc::SDL_GameEngine {
public:

    FPS_SGE() {
        sAppName = "console FPS ported to CGE to PGE to SGE";
    }

private:
    int nMapWidth = 16;				// World Dimensions
    int nMapHeight = 16;

    float fPlayerX = 14.7f;			// Player Start Position
    float fPlayerY = 5.09f;
    float fPlayerA = 0.0f;			// Player Start Rotation
    float fFOV = 3.14159f / 4.0f;	// Field of View
    float fDepth = 16.0f;			// Maximum rendering distance
    float fSpeed = 5.0f;			// Walking Speed

	std::string map;

protected:

    virtual bool OnUserCreate() {
        // Create Map of world space # = wall block, . = space
        map += "#########.......";
        map += "#...............";
        map += "#.......########";
        map += "#..............#";
        map += "#......##......#";
        map += "#......##......#";
        map += "#..............#";
        map += "###............#";
        map += "##.............#";
        map += "#......####..###";
        map += "#......#.......#";
        map += "#......#.......#";
        map += "#..............#";
        map += "#......#########";
        map += "#..............#";
        map += "################";

        return true;
    }

    virtual bool OnUserUpdate( float fElapsedTime ) {

		// Handle CCW Rotation
		if (GetKey( flc::Key::A).bHeld)
			fPlayerA -= (fSpeed * 0.75f) * fElapsedTime;

		// Handle CW Rotation
		if (GetKey( flc::Key::D).bHeld)
			fPlayerA += (fSpeed * 0.75f) * fElapsedTime;

		// Handle Forwards movement & collision
		if (GetKey( flc::Key::W).bHeld)
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
		if (GetKey( flc::Key::S).bHeld)
		{
			fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;;
			fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;;
				fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;;
			}
		}

		for (int x = 0; x < nScreenWidth; x++)
		{
			// For each column, calculate the projected ray angle into world space
			float fRayAngle = (fPlayerA - fFOV/2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

			// Find distance to wall
			float fStepSize = 0.1f;		  // Increment size for ray casting, decrease to increase
			float fDistanceToWall = 0.0f; //                                      resolution

			bool bHitWall = false;		// Set when ray hits wall block
			bool bBoundary = false;		// Set when ray hits boundary between two wall blocks

			float fEyeX = sinf(fRayAngle); // Unit vector for ray in player space
			float fEyeY = cosf(fRayAngle);

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

						// To highlight tile boundaries, cast a ray from each corner
						// of the tile, to the player. The more coincident this ray
						// is to the rendering ray, the closer we are to a tile
						// boundary, which we'll shade to add detail to the walls
						std::vector<std::pair<float, float>> p;

						// Test each corner of hit tile, storing the distance from
						// the player, and the calculated dot product of the two rays
						for (int tx = 0; tx < 2; tx++)
							for (int ty = 0; ty < 2; ty++)
							{
								// Angle of corner to eye
								float vy = (float)nTestY + ty - fPlayerY;
								float vx = (float)nTestX + tx - fPlayerX;
								float d = sqrt(vx*vx + vy*vy);
								float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
								p.push_back(std::make_pair(d, dot));
							}

						// Sort Pairs from closest to farthest
						std::sort(p.begin(), p.end(), [](const std::pair<float, float> &left, const std::pair<float, float> &right) {return left.first < right.first; });

						// First two/three are closest (we will never see all four)
						float fBound = 0.01;
						if (acos(p.at(0).second) < fBound) bBoundary = true;
						if (acos(p.at(1).second) < fBound) bBoundary = true;
						if (acos(p.at(2).second) < fBound) bBoundary = true;
					}
				}
			}

			// Calculate distance to ceiling and floor
			int nCeiling = (float)(nScreenHeight/2.0) - nScreenHeight / ((float)fDistanceToWall);
			int nFloor = nScreenHeight - nCeiling;

			// Shader walls based on distance
			float nShade = 0.0f;
			if (fDistanceToWall <= fDepth / 4.0f)	  nShade = 1.00f;	// Very close
			else if (fDistanceToWall < fDepth / 3.0f) nShade = 0.75f;
			else if (fDistanceToWall < fDepth / 2.0f) nShade = 0.50f;
			else if (fDistanceToWall < fDepth)		  nShade = 0.25f;
			else									  nShade = 0.00f;	// Too far away

			if (bBoundary) nShade = 0.0f; // Black it out

			for (int y = 0; y < nScreenHeight; y++)
			{
				// Each Row
				if(y <= nCeiling)
                    Draw(x, y, flc::BLACK);
				else if(y > nCeiling && y <= nFloor)
					Draw(x, y, flc::WHITE * nShade);
				else // Floor
				{
					// Shade floor based on distance
					float b = 1.0f - (((float)y -nScreenHeight/2.0f) / ((float)nScreenHeight / 2.0f));
					float fShade = 0.0f;
					if (b < 0.25)		fShade = 1.00f;
					else if (b < 0.5)	fShade = 0.75f;
					else if (b < 0.75)	fShade = 0.50f;
					else if (b < 0.9)	fShade = 0.25f;
					else				fShade = 0.00f;
					Draw(x, y, flc::DARK_GREY * fShade);
				}
			}
		}

		// Display Stats
        std::string sCaption =
            "X="   + std::to_string( fPlayerX ) +
            ", Y=" + std::to_string( fPlayerY ) +
            ", A=" + std::to_string( fPlayerA );
        DrawString(0, 0, sCaption);

		// Display Map
		for (int nx = 0; nx < nMapWidth; nx++)
			for (int ny = 0; ny < nMapWidth; ny++)
			{
				DrawString(nx * 8, (ny + 1) * 8, std::string( 1, map[ny * nMapWidth + nx]));
			}
		DrawString((int)fPlayerY * 8, ((int)fPlayerX+1) * 8, "P");

		return true;
    }
};

int main( int argc, char *argv[] )
{
    // use flcConsoleGameEngine derived app
    FPS_SGE game;
    // construct the engine with screen and pixel sizes
    if (game.Construct( nScreenWidth, nScreenHeight, PIXEL_SIZE, PIXEL_SIZE ))
        // start it
        game.Start();

    return 0;
}

