/*
OneLoneCoder.com - MicroGame1 - Cave Diver
"Err..." - @Javidx9

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
Often as an exercise, I'll try to write code where every byte of source code
should count. I feel that this is an excellent way to practice logic, code
comprehension and smarmy arrogance.



Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/oJGx8cqaJLc

Last Updated: 16/10/2017
*/

/* Alterations to original (console) code - Joseph21 - 2022-11-18
 *   - removed "using namespace std" and added explicit scope resolution operator where needed
 *   - same for "using namespace chrono"
 * Port to ConsoleGameEngine - 2022-11-18
 *   - removed a number of include directives since they are already done in olcConsoleGameEngine.h
 *   - Created a class CaveDiver_CGE and put most of the before global variables in it as class variables
 *   - Added empty OnUserCreate()
 *   - Added OnUserUpdate() and put all user interaction in it (calls to GetAsyncKeyState() rewritten
 *     to calls to GetKey(), applied in a little lambda key_touched())
 *   - Replaced the calls to printf() with DrawString() or Draw() calls with the same (score) information
 *   - Rewrote main() to construct and start the console game engine
 */

#include <thread>

#include "olcConsoleGameEngine.h"

#define PIX_X    8
#define PIX_Y   16

int nScreenWidth  = 80;
int nScreenHeight = 30;

class CaveDiver_CGE : public olcConsoleGameEngine {
public:
    CaveDiver_CGE()
    {
        m_sAppName = L"CaveDiver_CGE";
    }

private:
	char s[80 * 30] = {'\0'};
	char n[10];

	float t = 0.0f, tt = 0.0f; // Timing Variables
	float w = 20.0f, m = 40.0f, tw = 20.0f, tm = 40.0f; // World Shape Variables
	float e = 1.0f, c = 40.0f; // Difficulty, Player Position
	int r = 0, h = 1000; // Player Score

protected:
    virtual bool OnUserCreate()
    {
        return true;
    }

    virtual bool OnUserUpdate( float f )
    {
        t  += f;
        tt += f;

        // little lambda to set char c into screen array s at position (x, y)
        auto sbxy = [&](int x, int y, char c) { s[y * 80 + x] = c; };

		// Timing =======================
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		// Input ========================

		// little lambda to combine pressing and holding of a key
        auto key_touched = [=]( int nKey ) {
            return (GetKey( nKey ).bPressed || GetKey( nKey ).bHeld);
        };

		if (key_touched( VK_LEFT  )) c -= 40.0f * f;
		if (key_touched( VK_RIGHT )) c += 40.0f * f;

		// Update World
		if (t >= 1.5f)
        {
            t -= 1.5f;
            tw = rand() % 10 + 10;
            tm = rand() % (76 - ((int)tw >> 1)) + 4;
        }
		e += f * 0.001f;
		w += (tw - w) * e * f;
		m += (tm - m) * e * f;
		float p = sin(tt * e) * m / 2.0f + 40.0f;

		// Fill Row
		for (int x = 0; x < 79; x++)
			if (x + 1 < p - w / 2 || x > p + w / 2)
                sbxy(x, r, L'.');
			else if (x == (int)(p - (w / 2)) || x == (int)(p + (w / 2)))
			    sbxy(x, r, L'#');
			else
			    sbxy(x, r, L' ');

		// Scrolling Effect
		r +=  1;
		r %= 28;

		// Draw To Screen
		for (int y = 1; y < 28; y++) {
            for (int x = 0; x < 79; x++) {
                Draw( x, y, s[((y + 28 + r) % 28) * 80 + x] );
            }
		}
		DrawString( c - 2, 2, L"O-V-O", FG_RED );
		DrawString( c - 2, 3, L" O-O ", FG_RED );
		DrawString( c - 2, 4, L"  V  ", FG_RED );

		// Collision Checking & Health Update
		h -= s[((4 + r) % 28) * 80 + ((int)c - 2)] == L'.';

		// Draw HUD
		DrawString(  0, 27, L"===============================================================================", FG_YELLOW );
		DrawString(  2, 28, L"Cave Diver - www.onelonecoder.com - Left Arrow / Right Arrow - Survive!", FG_YELLOW );
		DrawString(  2, 29, L"Distance Fallen: " + std::to_wstring( (int)(tt * 100.0f)), FG_YELLOW );
		DrawString( 40, 29, L"Health: " + std::to_wstring( h ), FG_YELLOW );

		if (h <= 0)
            Fill( 0, 0, nScreenWidth, nScreenHeight, PIXEL_SOLID, FG_BLACK );

        return h > 0;
    }
};


int main()
{
    // use olcConsoleGameEngine derived app
    CaveDiver_CGE game;
    game.ConstructConsole( nScreenWidth, nScreenHeight, PIX_X, PIX_Y );
    game.Start();

    // Oh Dear - game over!
    std::cout << "Dead..." << std::endl;

	return 0;
}
