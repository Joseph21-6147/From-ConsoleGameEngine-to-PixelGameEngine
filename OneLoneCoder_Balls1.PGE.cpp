/*
OneLoneCoder.com - Programming Balls! #1 Circle Vs Circle Collisions
"..it's just balls bangin' together init..." - @Javidx9

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
Collision detection engines can get quite complicated. This program shows the interactions
between circular objects of different sizes and masses. Use Left mouse button to select
and drag a ball to examin static collisions, and use Right mouse button to apply velocity
to the balls as if using a pool/snooker/billiards cue.

Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
Part #1 https://youtu.be/LPzyNOHY3A4

Last Updated: 21/01/2017
*/


/* Alterations Joseph21 - 20220717
 *   - removed "using namespace std" and added explicit scope resolution operator where needed
 *   - resized screen and pixel for larger viewport
 * Port to PixelGameEngine - 20220717
 *   - see cheat sheet for generic adaptations needed for port
 *   - added function DrawWireFrameModel() - this function was implemented in the CGE but is not in the PGE
 */

#include <iostream>
#include <string>
//using namespace std;

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


// vecModelCoordinates = the wire frame model
// x, y - the screenposition where to draw it
// r = angle of rotation
// s = scaling factor
void DrawWireFrameModel(
    olc::PixelGameEngine *engine,
    const std::vector<std::pair<float, float>> &vecModelCoordinates,
    float x, float y,
    float r = 0.0f,
    float s = 1.0f,
    olc::Pixel col = olc::WHITE
) {
    // pair.first  = x coordinate
    // pair.second = y coordinate

    // Create translated model vector of coordinate pairs
    std::vector<std::pair<float, float>> vecTransformedCoordinates;
    int verts = vecModelCoordinates.size();   // keep the model vector static
    vecTransformedCoordinates.resize( verts );  // create another vector the same size as the model vector

    // Rotate
    for (int i = 0; i < verts; i++) {
        vecTransformedCoordinates[i].first  = vecModelCoordinates[i].first * cosf( r ) - vecModelCoordinates[i].second * sinf( r );
        vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf( r ) + vecModelCoordinates[i].second * cosf( r );
    }
    // Scale
    for (int i = 0; i < verts; i++) {
        vecTransformedCoordinates[i].first  = vecTransformedCoordinates[i].first  * s;
        vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
    }
    // Translate
    for (int i = 0; i < verts; i++) {
        vecTransformedCoordinates[i].first  = vecTransformedCoordinates[i].first  + x;
        vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
    }
    // Draw closed polygon
    for (int i = 0; i < verts + 1; i++) {
        int j = (i + 1);
        engine->DrawLine( vecTransformedCoordinates[i % verts].first, vecTransformedCoordinates[i % verts].second,
                          vecTransformedCoordinates[j % verts].first, vecTransformedCoordinates[j % verts].second, col );
    }
}

struct sBall
{
	float px, py;
	float vx, vy;
	float ax, ay;
	float radius;
	float mass;

	int id;
};


class CirclePhysics : public olc::PixelGameEngine
{
public:
	CirclePhysics()
	{
		sAppName = "Circle Physics";
	}

private:
	std::vector<std::pair<float, float>> modelCircle;
	std::vector<sBall> vecBalls;
	sBall *pSelectedBall = nullptr;


	// Adds a ball to the vector
	void AddBall(float x, float y, float r = 5.0f)
	{
		sBall b;
		b.px = x; b.py = y;
		b.vx = 0; b.vy = 0;
		b.ax = 0; b.ay = 0;
		b.radius = r;
		b.mass = r * 10.0f;

		b.id = vecBalls.size();
		vecBalls.emplace_back(b);
	}


public:
	bool OnUserCreate()
	{
		// Define Circle Model
		modelCircle.push_back({ 0.0f, 0.0f });
		int nPoints = 20;
		for (int i = 0; i < nPoints; i++)
			modelCircle.push_back({ cosf(i / (float)(nPoints - 1) * 2.0f * 3.14159f) , sinf(i / (float)(nPoints - 1) * 2.0f * 3.14159f) });

		float fDefaultRad = 8.0f;
		//AddBall(ScreenWidth() * 0.25f, ScreenHeight() * 0.5f, fDefaultRad);
		//AddBall(ScreenWidth() * 0.75f, ScreenHeight() * 0.5f, fDefaultRad);

		// Add 10 Random Balls
		for (int i = 0; i <10; i++)
			AddBall(rand() % ScreenWidth(), rand() % ScreenHeight(), rand() % 16 + 2);


		return true;
	}

	bool OnUserUpdate(float fElapsedTime)
	{
		auto DoCirclesOverlap = [](float x1, float y1, float r1, float x2, float y2, float r2)
		{
			return fabs((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)) <= (r1 + r2)*(r1 + r2);
		};

		auto IsPointInCircle = [](float x1, float y1, float r1, float px, float py)
		{
			return fabs((x1 - px)*(x1 - px) + (y1 - py)*(y1 - py)) < (r1 * r1);
		};

		if (GetMouse(0).bPressed || GetMouse(1).bPressed)
		{
			pSelectedBall = nullptr;
			for (auto &ball : vecBalls)
			{
				if (IsPointInCircle(ball.px, ball.py, ball.radius, GetMouseX(), GetMouseY()))
				{
					pSelectedBall = &ball;
					break;
				}
			}
		}

		if (GetMouse(0).bHeld)
		{
			if (pSelectedBall != nullptr)
			{
				pSelectedBall->px = GetMouseX();
				pSelectedBall->py = GetMouseY();
			}
		}

		if (GetMouse(0).bReleased)
		{
			pSelectedBall = nullptr;
		}

		if (GetMouse(1).bReleased)
		{
			if (pSelectedBall != nullptr)
			{
				// Apply velocity
				pSelectedBall->vx = 5.0f * ((pSelectedBall->px) - (float)GetMouseX());
				pSelectedBall->vy = 5.0f * ((pSelectedBall->py) - (float)GetMouseY());
			}

			pSelectedBall = nullptr;
		}


		std::vector<std::pair<sBall*, sBall*>> vecCollidingPairs;

		// Update Ball Positions
		for (auto &ball : vecBalls)
		{
			// Add Drag to emulate rolling friction
			ball.ax = -ball.vx * 0.8f;
			ball.ay = -ball.vy * 0.8f;

			// Update ball physics
			ball.vx += ball.ax * fElapsedTime;
			ball.vy += ball.ay * fElapsedTime;
			ball.px += ball.vx * fElapsedTime;
			ball.py += ball.vy * fElapsedTime;

			// Wrap the balls around screen
			if (ball.px < 0) ball.px += (float)ScreenWidth();
			if (ball.px >= ScreenWidth()) ball.px -= (float)ScreenWidth();
			if (ball.py < 0) ball.py += (float)ScreenHeight();
			if (ball.py >= ScreenHeight()) ball.py -= (float)ScreenHeight();

			// Clamp velocity near zero
			if (fabs(ball.vx*ball.vx + ball.vy*ball.vy) < 0.01f)
			{
				ball.vx = 0;
				ball.vy = 0;
			}
		}

		// Static collisions, i.e. overlap
		for (auto &ball : vecBalls)
		{
			for (auto &target : vecBalls)
			{
				if (ball.id != target.id)
				{
					if (DoCirclesOverlap(ball.px, ball.py, ball.radius, target.px, target.py, target.radius))
					{
						// Collision has occured
						vecCollidingPairs.push_back({ &ball, &target });

						// Distance between ball centers
						float fDistance = sqrtf((ball.px - target.px)*(ball.px - target.px) + (ball.py - target.py)*(ball.py - target.py));

						// Calculate displacement required
						float fOverlap = 0.5f * (fDistance - ball.radius - target.radius);

						// Displace Current Ball away from collision
						ball.px -= fOverlap * (ball.px - target.px) / fDistance;
						ball.py -= fOverlap * (ball.py - target.py) / fDistance;

						// Displace Target Ball away from collision
						target.px += fOverlap * (ball.px - target.px) / fDistance;
						target.py += fOverlap * (ball.py - target.py) / fDistance;
					}
				}
			}
		}

		// Now work out dynamic collisions
		for (auto c : vecCollidingPairs)
		{
			sBall *b1 = c.first;
			sBall *b2 = c.second;

			// Distance between balls
			float fDistance = sqrtf((b1->px - b2->px)*(b1->px - b2->px) + (b1->py - b2->py)*(b1->py - b2->py));

			// Normal
			float nx = (b2->px - b1->px) / fDistance;
			float ny = (b2->py - b1->py) / fDistance;

			// Tangent
			float tx = -ny;
			float ty = nx;

			// Dot Product Tangent
			float dpTan1 = b1->vx * tx + b1->vy * ty;
			float dpTan2 = b2->vx * tx + b2->vy * ty;

			// Dot Product Normal
			float dpNorm1 = b1->vx * nx + b1->vy * ny;
			float dpNorm2 = b2->vx * nx + b2->vy * ny;

			// Conservation of momentum in 1D
			float m1 = (dpNorm1 * (b1->mass - b2->mass) + 2.0f * b2->mass * dpNorm2) / (b1->mass + b2->mass);
			float m2 = (dpNorm2 * (b2->mass - b1->mass) + 2.0f * b1->mass * dpNorm1) / (b1->mass + b2->mass);

			// Update ball velocities
			b1->vx = tx * dpTan1 + nx * m1;
			b1->vy = ty * dpTan1 + ny * m1;
			b2->vx = tx * dpTan2 + nx * m2;
			b2->vy = ty * dpTan2 + ny * m2;

			// Wikipedia Version - Maths is smarter but same
			//float kx = (b1->vx - b2->vx);
			//float ky = (b1->vy - b2->vy);
			//float p = 2.0 * (nx * kx + ny * ky) / (b1->mass + b2->mass);
			//b1->vx = b1->vx - p * b2->mass * nx;
			//b1->vy = b1->vy - p * b2->mass * ny;
			//b2->vx = b2->vx + p * b1->mass * nx;
			//b2->vy = b2->vy + p * b1->mass * ny;
		}

		// Clear Screen
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

		// Draw Balls
		for (auto ball : vecBalls)
			DrawWireFrameModel(this, modelCircle, ball.px, ball.py, atan2f(ball.vy, ball.vx), ball.radius, olc::WHITE);

		// Draw static collisions
		for (auto c : vecCollidingPairs)
			DrawLine(c.first->px, c.first->py, c.second->px, c.second->py, olc::RED);

		// Draw Cue
		if (pSelectedBall != nullptr)
			DrawLine(pSelectedBall->px, pSelectedBall->py, GetMouseX(), GetMouseY(), olc::BLUE);

		return true;
	}

};


int main()
{
	CirclePhysics game;
	if (game.Construct(320, 240, 4, 4))
		game.Start();
	else
		std::cout << "Could not construct console" << std::endl;

	return 0;
};
