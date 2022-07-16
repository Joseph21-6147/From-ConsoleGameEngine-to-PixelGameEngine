/*
OneLoneCoder.com - Code-It-Yourself! Racing Game at the command prompt (quick and simple c++)
"Let's go, go, go!!!" - @Javidx9

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
I'm a sim-racer when I'm not coding. Racing games are far more sophisticated than
they used to be. Frankly, retro racing games are a bit naff. But when done in the
command prompt they have a new level of craziness.

Controls
~~~~~~~~
Left Arrow/Right Arrow Steer, Up Arrow accelerates. There are no brakes!
Set the fastest lap times you can!

Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/KkMZI5Jbf18

Last Updated: 10/07/2017
*/


/* Addition Joseph21 - 20220715
 *   - removed "using namespace std" and added explicit scope resolution operator where needed
 * Port to PixelGameEngine - 20220715
 *   - see cheat sheet for generic adaptations needed for port
 *   - had to change screen dimensions and quite some positions for DrawString() calls
 *   - draw the car in red font otherwise its not visible against white road background
 */

#include <iostream>
#include <string>
//using namespace std;

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


class OneLoneCoder_FormulaOLC : public olc::PixelGameEngine
{
public:
	OneLoneCoder_FormulaOLC()
	{
		sAppName = "Formula OLC";
	}

private:

	float fDistance = 0.0f;			// Distance car has travelled around track
	float fCurvature = 0.0f;		// Current track curvature, lerped between track sections
	float fTrackCurvature = 0.0f;	// Accumulation of track curvature
	float fTrackDistance = 0.0f;	// Total distance of track

	float fCarPos = 0.0f;			// Current car position
	float fPlayerCurvature = 0.0f;			// Accumulation of player curvature
	float fSpeed = 0.0f;			// Current player speed

	std::vector<std::pair<float, float>> vecTrack; // Track sections, sharpness of bend, length of section

	std::list<float> listLapTimes;		// List of previous lap times
	float fCurrentLapTime;			// Current lap time

protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		// Define track
		vecTrack.push_back(std::make_pair(0.0f, 10.0f));		// Short section for start/finish line
		vecTrack.push_back(std::make_pair(0.0f, 200.0f));
		vecTrack.push_back(std::make_pair(1.0f, 200.0f));
		vecTrack.push_back(std::make_pair(0.0f, 400.0f));
		vecTrack.push_back(std::make_pair(-1.0f, 100.0f));
		vecTrack.push_back(std::make_pair(0.0f, 200.0f));
		vecTrack.push_back(std::make_pair(-1.0f, 200.0f));
		vecTrack.push_back(std::make_pair(1.0f, 200.0f));
		vecTrack.push_back(std::make_pair(0.0f, 200.0f));
		vecTrack.push_back(std::make_pair(0.2f, 500.0f));
		vecTrack.push_back(std::make_pair(0.0f, 200.0f));

		// Calculate total track distance, so we can set lap times
		for (auto t : vecTrack)
			fTrackDistance += t.second;

		listLapTimes = { 0,0,0,0,0 };
		fCurrentLapTime = 0.0f;

		return true;
	}




	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		// Handle control input
		int nCarDirection = 0;

		if (GetKey(olc::Key::UP).bHeld)
			fSpeed += 2.0f * fElapsedTime;
		else
			fSpeed -= 1.0f * fElapsedTime;

		// Car Curvature is accumulated left/right input, but inversely proportional to speed
		// i.e. it is harder to turn at high speed
		if (GetKey(olc::Key::LEFT).bHeld)
		{
			fPlayerCurvature -= 0.7f * fElapsedTime * (1.0f - fSpeed / 2.0f);
			nCarDirection = -1;
		}

		if (GetKey(olc::Key::RIGHT).bHeld)
		{
			fPlayerCurvature += 0.7f * fElapsedTime * (1.0f - fSpeed / 2.0f);
			nCarDirection = +1;
		}

		// If car curvature is too different to track curvature, slow down
		// as car has gone off track
		if (fabs(fPlayerCurvature - fTrackCurvature) >= 0.8f)
			fSpeed -= 5.0f * fElapsedTime;

		// Clamp Speed
		if (fSpeed < 0.0f)	fSpeed = 0.0f;
		if (fSpeed > 1.0f)	fSpeed = 1.0f;

		// Move car along track according to car speed
		fDistance += (70.0f * fSpeed) * fElapsedTime;

		// Get Point on track
		float fOffset = 0;
		int nTrackSection = 0;

		// Lap Timing and counting
		fCurrentLapTime += fElapsedTime;
		if (fDistance >= fTrackDistance)
		{
			fDistance -= fTrackDistance;
			listLapTimes.push_front(fCurrentLapTime);
			listLapTimes.pop_back();
			fCurrentLapTime = 0.0f;
		}

		// Find position on track (could optimise)
		while (nTrackSection < vecTrack.size() && fOffset <= fDistance)
		{
			fOffset += vecTrack[nTrackSection].second;
			nTrackSection++;
		}

		// Interpolate towards target track curvature
		float fTargetCurvature = vecTrack[nTrackSection - 1].first;
		float fTrackCurveDiff = (fTargetCurvature - fCurvature) * fElapsedTime * fSpeed;

		// Accumulate player curvature
		fCurvature += fTrackCurveDiff;

		// Accumulate track curvature
		fTrackCurvature += (fCurvature) * fElapsedTime * fSpeed;

		// Draw Sky - light blue and dark blue
		for (int y = 0; y < ScreenHeight() / 2; y++)
			for (int x = 0; x < ScreenWidth(); x++)
				Draw(x, y, y< ScreenHeight() / 4 ? olc::VERY_DARK_BLUE : olc::DARK_BLUE);

		// Draw Scenery - our hills are a rectified sine wave, where the phase is adjusted by the
		// accumulated track curvature
		for (int x = 0; x < ScreenWidth(); x++)
		{
			int nHillHeight = (int)(fabs(sinf(x * 0.01f + fTrackCurvature) * 16.0f));
			for (int y = (ScreenHeight() / 2) - nHillHeight; y < ScreenHeight() / 2; y++)
				Draw(x, y, olc::DARK_YELLOW);
		}


		// Draw Track - Each row is split into grass, clip-board and track
		for (int y = 0; y < ScreenHeight() / 2; y++)
			for (int x = 0; x < ScreenWidth(); x++)
			{
				// Perspective is used to modify the width of the track row segments
				float fPerspective = (float)y / (ScreenHeight()/2.0f);
				float fRoadWidth = 0.1f + fPerspective * 0.8f; // Min 10% Max 90%
				float fClipWidth = fRoadWidth * 0.15f;
				fRoadWidth *= 0.5f;	// Halve it as track is symmetrical around center of track, but offset...

				// ...depending on where the middle point is, which is defined by the current
				// track curvature.
				float fMiddlePoint = 0.5f + fCurvature * powf((1.0f - fPerspective), 3);

				// Work out segment boundaries
				int nLeftGrass = (fMiddlePoint - fRoadWidth - fClipWidth) * ScreenWidth();
				int nLeftClip = (fMiddlePoint - fRoadWidth) * ScreenWidth();
				int nRightClip = (fMiddlePoint + fRoadWidth) * ScreenWidth();
				int nRightGrass = (fMiddlePoint + fRoadWidth + fClipWidth) * ScreenWidth();

				int nRow = ScreenHeight() / 2 + y;

				// Using periodic oscillatory functions to give lines, where the phase is controlled
				// by the distance around the track. These take some fine tuning to give the right "feel"
				olc::Pixel nGrassColour = sinf(20.0f *  powf(1.0f - fPerspective,3) + fDistance * 0.1f) > 0.0f ? olc::GREEN : olc::DARK_GREEN;
				olc::Pixel nClipColour = sinf(80.0f *  powf(1.0f - fPerspective, 2) + fDistance) > 0.0f ? olc::RED : olc::WHITE;

				// Start finish straight changes the road colour to inform the player lap is reset
				olc::Pixel nRoadColour = (nTrackSection-1) == 0 ? olc::WHITE : olc::GREY;

				// Draw the row segments
				if (x >= 0 && x < nLeftGrass)
					Draw(x, nRow, nGrassColour);
				if (x >= nLeftGrass && x < nLeftClip)
					Draw(x, nRow, nClipColour);
				if (x >= nLeftClip && x < nRightClip)
					Draw(x, nRow, nRoadColour);
				if (x >= nRightClip && x < nRightGrass)
					Draw(x, nRow, nClipColour);
				if (x >= nRightGrass && x < ScreenWidth())
					Draw(x, nRow, nGrassColour);

			}

		// Draw Car - car position on road is proportional to difference between
		// current accumulated track curvature, and current accumulated player curvature
		// i.e. if they are similar, the car will be in the middle of the track
		fCarPos = fPlayerCurvature - fTrackCurvature;
		int nCarPos = ScreenWidth() / 2 + ((int)(ScreenWidth() * fCarPos) / 2.0) - 7; // Offset for sprite

		// Draw a car that represents what the player is doing. Apologies for the quality
		// of the sprite... :-(
		switch (nCarDirection)
		{
		case 0:
			DrawString(nCarPos, ScreenHeight() - 64, "   ||####||   ", olc::RED);
			DrawString(nCarPos, ScreenHeight() - 56, "      ##      ", olc::RED);
			DrawString(nCarPos, ScreenHeight() - 48, "     ####     ", olc::RED);
			DrawString(nCarPos, ScreenHeight() - 40, "     ####     ", olc::RED);
			DrawString(nCarPos, ScreenHeight() - 32, "|||  ####  |||", olc::RED);
			DrawString(nCarPos, ScreenHeight() - 24, "|||########|||", olc::RED);
			DrawString(nCarPos, ScreenHeight() - 16, "|||  ####  |||", olc::RED);
			break;

		case +1:
			DrawString(nCarPos, ScreenHeight() - 64, "      //####//", olc::RED);
			DrawString(nCarPos, ScreenHeight() - 56, "         ##   ", olc::RED);
			DrawString(nCarPos, ScreenHeight() - 48, "       ####   ", olc::RED);
			DrawString(nCarPos, ScreenHeight() - 40, "      ####    ", olc::RED);
			DrawString(nCarPos, ScreenHeight() - 32, "///  ####//// ", olc::RED);
			DrawString(nCarPos, ScreenHeight() - 24, "//#######///O ", olc::RED);
			DrawString(nCarPos, ScreenHeight() - 16, "/// #### //// ", olc::RED);
			break;

		case -1:
			DrawString(nCarPos, ScreenHeight() - 64, "\\\\####\\\\      "   , olc::RED);
			DrawString(nCarPos, ScreenHeight() - 56, "   ##         "       , olc::RED);
			DrawString(nCarPos, ScreenHeight() - 48, "   ####       "       , olc::RED);
			DrawString(nCarPos, ScreenHeight() - 40, "    ####      "       , olc::RED);
			DrawString(nCarPos, ScreenHeight() - 32, " \\\\\\\\####  \\\\\\", olc::RED);
			DrawString(nCarPos, ScreenHeight() - 24, " O\\\\\\#######\\\\"  , olc::RED);
			DrawString(nCarPos, ScreenHeight() - 16, " \\\\\\\\ #### \\\\\\", olc::RED);
			break;
		}

		// Draw Stats
		DrawString(0,  0, "Distance: " + std::to_string(fDistance));
		DrawString(0,  8, "Target Curvature: " + std::to_string(fCurvature));
		DrawString(0, 16, "Player Curvature: " + std::to_string(fPlayerCurvature));
		DrawString(0, 24, "Player Speed    : " + std::to_string(fSpeed));
		DrawString(0, 32, "Track Curvature : " + std::to_string(fTrackCurvature));

		auto disp_time = [](float t) // Little lambda to turn floating point seconds into minutes:seconds:millis string
		{
			int nMinutes = t / 60.0f;
			int nSeconds = t - (nMinutes * 60.0f);
			int nMilliSeconds = (t - (float)nSeconds) * 1000.0f;
			return std::to_string(nMinutes) + "." + std::to_string(nSeconds) + ":" + std::to_string(nMilliSeconds);
		};

		// Display current laptime
		DrawString(10, 40, disp_time(fCurrentLapTime));

		// Display last 5 lap times
		int j = 48;
		for (auto l : listLapTimes)
		{
			DrawString(10, j, disp_time(l));
			j += 8;
		}

		return true;
	}
};


int main()
{
	// Use olcConsoleGameEngine derived app
	OneLoneCoder_FormulaOLC game;
	if (game.Construct(640, 300, 2, 2))
        game.Start();

	return 0;
}
