/*
OneLoneCoder.com - Splines Part 1
"Bendy Wavy Curly" - @Javidx9

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
Curvy things are always better. Splines are a nice way to approximate
curves and loops for games. This video is the first of two parts
demonstrating how Catmull-Rom splines can be implemented.

Use Z + X to select a point and move it with the arrow keys
Use A + S to move the agent around the spline loop

Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/9_aJGUTePYo

Last Updated: 06/08/2017
*/


/* Addition Joseph21 - 20220715
 *   - removed "using namespace std" and added explicit scope resolution operator where needed
 * Port to PixelGameEngine - 20220715
 *   - see cheat sheet for generic adaptations needed for port
 *   - implemented other screen characteristics
 * Port to SDL-GameEngine - 20221204
 *   - generic port, no specifics
 */

#include <iostream>
#include <string>

#include "SGE/SGE_Core.h"

struct sPoint2D
{
	float x;
	float y;
};

struct sSpline
{
	std::vector<sPoint2D> points;

	sPoint2D GetSplinePoint(float t, bool bLooped = false)
	{
		int p0, p1, p2, p3;
		if (!bLooped)
		{
			p1 = (int)t + 1;
			p2 = p1 + 1;
			p3 = p2 + 1;
			p0 = p1 - 1;
		}
		else
		{
			p1 = (int)t;
			p2 = (p1 + 1) % points.size();
			p3 = (p2 + 1) % points.size();
			p0 = p1 >= 1 ? p1 - 1 : points.size() - 1;
		}

		t = t - (int)t;

		float tt = t * t;
		float ttt = tt * t;

		float q1 = -ttt + 2.0f*tt - t;
		float q2 = 3.0f*ttt - 5.0f*tt + 2.0f;
		float q3 = -3.0f*ttt + 4.0f*tt + t;
		float q4 = ttt - tt;

		float tx = 0.5f * (points[p0].x * q1 + points[p1].x * q2 + points[p2].x * q3 + points[p3].x * q4);
		float ty = 0.5f * (points[p0].y * q1 + points[p1].y * q2 + points[p2].y * q3 + points[p3].y * q4);

		return{ tx, ty };
	}

	sPoint2D GetSplineGradient(float t, bool bLooped = false)
	{
		int p0, p1, p2, p3;
		if (!bLooped)
		{
			p1 = (int)t + 1;
			p2 = p1 + 1;
			p3 = p2 + 1;
			p0 = p1 - 1;
		}
		else
		{
			p1 = (int)t;
			p2 = (p1 + 1) % points.size();
			p3 = (p2 + 1) % points.size();
			p0 = p1 >= 1 ? p1 - 1 : points.size() - 1;
		}

		t = t - (int)t;

		float tt = t * t;
		float ttt = tt * t;

		float q1 = -3.0f * tt + 4.0f*t - 1;
		float q2 = 9.0f*tt - 10.0f*t;
		float q3 = -9.0f*tt + 8.0f*t + 1.0f;
		float q4 = 3.0f*tt - 2.0f*t;

		float tx = 0.5f * (points[p0].x * q1 + points[p1].x * q2 + points[p2].x * q3 + points[p3].x * q4);
		float ty = 0.5f * (points[p0].y * q1 + points[p1].y * q2 + points[p2].y * q3 + points[p3].y * q4);

		return{ tx, ty };
	}
};

class OneLoneCoder_Splines : public flc::SDL_GameEngine
{
public:
	OneLoneCoder_Splines()
	{
		sAppName = "Splines";
	}

private:
	sSpline path;
	int nSelectedPoint = 0;
	float fMarker = 0.0f;

protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		//path.points = { { 10, 41 },{ 40, 41 },{ 70, 41 },{ 100, 41 } };
		path.points = { { 10, 41 },{ 20, 41 },{ 30, 41 },{ 40, 41 },{ 50, 41 },{ 60, 41 },{ 70, 41 },{ 80, 41 },{ 90, 41 },{ 100, 41 } };
		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		// Clear Screen
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), flc::BLACK);

		// Handle input
		if (GetKey(flc::Key::X).bReleased)
		{
			nSelectedPoint++;
			if (nSelectedPoint >= (int)path.points.size())
				nSelectedPoint = 0;
		}

		if (GetKey(flc::Key::Z).bReleased)
		{
			nSelectedPoint--;
			if (nSelectedPoint < 0)
				nSelectedPoint = path.points.size() - 1;
		}

		if (GetKey(flc::Key::LEFT).bHeld)
			path.points[nSelectedPoint].x -= 30.0f * fElapsedTime;

		if (GetKey(flc::Key::RIGHT).bHeld)
			path.points[nSelectedPoint].x += 30.0f * fElapsedTime;

		if (GetKey(flc::Key::UP).bHeld)
			path.points[nSelectedPoint].y -= 30.0f * fElapsedTime;

		if (GetKey(flc::Key::DOWN).bHeld)
			path.points[nSelectedPoint].y += 30.0f * fElapsedTime;

		if (GetKey(flc::Key::A).bHeld)
			fMarker -= 5.0f * fElapsedTime;

		if (GetKey(flc::Key::S).bHeld)
			fMarker += 5.0f * fElapsedTime;

		if (fMarker >= (float)path.points.size())
			fMarker -= (float)path.points.size();

		if (fMarker < 0.0f)
			fMarker += (float)path.points.size();

		// Draw Spline
		for (float t = 0; t < (float)path.points.size(); t += 0.005f)
		{
			sPoint2D pos = path.GetSplinePoint(t, true);
			Draw(pos.x, pos.y);
		}

		// Draw Control Points
		for (int i = 0; i < (int)path.points.size(); i++)
		{
			FillRect(path.points[i].x - 1, path.points[i].y - 1, 3, 3, flc::RED);
			DrawString(path.points[i].x, path.points[i].y, std::to_string(i));
		}

		// Highlight control point
		FillRect(path.points[nSelectedPoint].x - 1, path.points[nSelectedPoint].y - 1, 3, 3, flc::YELLOW);
		DrawString(path.points[nSelectedPoint].x, path.points[nSelectedPoint].y, std::to_string(nSelectedPoint));

		// Draw agent to demonstrate gradient
		sPoint2D p1 = path.GetSplinePoint(fMarker, true);
		sPoint2D g1 = path.GetSplineGradient(fMarker, true);
		float r = atan2(-g1.y, g1.x);
		DrawLine(5.0f * sin(r) + p1.x, 5.0f * cos(r) + p1.y, -5.0f * sin(r) + p1.x, -5.0f * cos(r) + p1.y, flc::BLUE);
		return true;
	}
};

int main( int argc, char *argv[] )
{
	OneLoneCoder_Splines demo;
	if (demo.Construct(240, 150, 4, 4))
        demo.Start();
	return 0;
}
