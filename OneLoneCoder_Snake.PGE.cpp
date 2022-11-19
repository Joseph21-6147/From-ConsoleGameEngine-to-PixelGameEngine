/*
OneLoneCoder.com - Command Line Snake
"Give me a break, I'm on holiday..." - @Javidx9

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
Classic Snake! Controls are Arrow keys Left & Right, eat food, grow larger, avoid self!


Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/e8lYLYlrGLg

Last Updated: 24/06/2017
*/


/* Alterations to original (console) code - Joseph21 - 2022-11-19
 *   - removed "using namespace std" and added explicit scope resolution operator where needed
 *   - added explicit cast std::chrono::milliseconds() around 120ms and 200ms to solve errors
 * Port to ConsoleGameEngine - 2022-11-19
 *   - removed a number of include directives since they are already done in olcConsoleGameEngine.h
 *   - Created a class Snake_CGE and put most of the before global variables in it as class variables
 *   - Added a reset function to put all class variables to initial values and added OnUserCreate()
 *   - Added OnUserUpdate() and put all user interaction in it (calls to GetAsyncKeyState() rewritten
 *     to calls to GetKey()
 *   - Replaced all references to the screen buffer with calls to Draw() and DrawString()
 *   - Rewrote main() to construct and start the console game engine
 * Port to PixelGameEngine - 2022-11-19
 *   - standard porting stuff - see cheat sheet for generic adaptations needed for port
 *   - Introduced a lambda CGE_Draw() to emulate the Draw() of one glyph CGE style
 *   - Similarly for CGE_DrawString() as wrapper around DrawString()
 *   - Removed the check if a game position was empty when selecting a random food position, since
 *     this requires an additional data structure to implement
 *   - Adapted screen resolution
 *   - Added some colour for fun :)
 */

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
//using namespace std;

int nScreenWidth = 120;
int nScreenHeight = 30;

struct sSnakeSegment
{
	int x;
	int y;
};

class Snake_PGE : public olc::PixelGameEngine {
    public:

        Snake_PGE() {
            sAppName = "Snake_PGE";
        }

    private:
		// Reset to known state
		std::list<sSnakeSegment> snake = { {60,15},{61,15},{62,15},{63,15},{64,15},{65,15},{66,15},{67,15},{68,15},{69,15} };
		int nFoodX = 30;
		int nFoodY = 15;
		int nScore = 0;
		int nSnakeDirection = 3;
		bool bDead = false;
		bool bKeyLeft = false, bKeyRight = false, bKeyLeftOld = false, bKeyRightOld = false;

        float fElapsedTimeCumulated = 0.0f;

    protected:

        void ResetGame() {
            // initial position for snake
            snake = { {60,15},{61,15},{62,15},{63,15},{64,15},{65,15},{66,15},{67,15},{68,15},{69,15} };
            // initial location where food is
		    nFoodX = 30;
		    nFoodY = 15;
		    nScore = 0;

            nSnakeDirection = 3;   // 0 = north, 1 = east, 2 = south, 3 = west
            bKeyLeft = false, bKeyRight = false, bKeyLeftOld = false, bKeyRightOld = false;

            bDead = false;

            fElapsedTimeCumulated = 0.0f;
        }

        virtual bool OnUserCreate() {

            srand( time( 0 ));

            ResetGame();

            return true;
        }

        virtual bool OnUserUpdate( float fElapsedTime ) {

            // ==========/ stage 1 - timing & input /==========

            // compensate for the x steps being smaller than the y steps on the screen
#define TICK_X 0.120f
#define TICK_Y 0.200f

            float fLocalTick = ((nSnakeDirection % 2 == 1) ? TICK_X : TICK_Y);
            fElapsedTimeCumulated += fElapsedTime;
            // NOTE to prevent very fast game, the user input and game logic is only triggered at "ticks"
            // the rendering is done every frame though
            if (!bDead && fElapsedTimeCumulated > fLocalTick) {
                fElapsedTimeCumulated -= fLocalTick;
                // Get input - be sensitive to D or right arrow and A or left arrow
                auto key_touched = [=]( olc::Key nKey ) {
                    return (GetKey( nKey ).bHeld || GetKey( nKey ).bPressed);
                };
                bKeyRight = key_touched( olc::Key::D ) || key_touched( olc::Key::RIGHT );
                bKeyLeft  = key_touched( olc::Key::A ) || key_touched( olc::Key::LEFT  );

				if (bKeyRight && !bKeyRightOld)
				{
					nSnakeDirection++;
					if (nSnakeDirection == 4) nSnakeDirection = 0;
				}

				if (bKeyLeft && !bKeyLeftOld)
				{
					nSnakeDirection--;
					if (nSnakeDirection == -1) nSnakeDirection = 3;
				}

				bKeyLeftOld  = bKeyLeft;
				bKeyRightOld = bKeyRight;

                // ==== Logic

                // Update Snake Position, place a new head at the front of the list in
                // the right direction
                switch (nSnakeDirection)
                {
                    case 0: snake.push_front({ snake.front().x    , snake.front().y - 1 }); break; // UP
                    case 1: snake.push_front({ snake.front().x + 1, snake.front().y     }); break; // RIGHT
                    case 2: snake.push_front({ snake.front().x    , snake.front().y + 1 }); break; // DOWN
                    case 3: snake.push_front({ snake.front().x - 1, snake.front().y     }); break; // LEFT
                }

                // Collision Detect Snake V Food
                if (snake.front().x == nFoodX && snake.front().y == nFoodY)
                {
                    nScore++;
                    // there's no checking if the randomly selected position is empty...
                    nFoodX =  rand() %  (ScreenWidth()  / 8);
                    nFoodY = (rand() % ((ScreenHeight() / 8) - 4)) + 3;

                    for (int i = 0; i < 5; i++)
                        snake.push_back({ snake.back().x, snake.back().y });
                }

                // Collision Detect Snake V World
                if (snake.front().x < 0 || snake.front().x >= nScreenWidth)
                    bDead = true;
                if (snake.front().y < 3 || snake.front().y >= nScreenHeight)
                    bDead = true;

                // Collision Detect Snake V Snake
                for (std::list<sSnakeSegment>::iterator i = snake.begin(); i != snake.end(); i++)
                    if (i != snake.begin() && i->x == snake.front().x && i->y == snake.front().y)
                        bDead = true;

                // Chop off Snakes tail :-/
                snake.pop_back();
			}

			// ==== Presentation

			// Clear Screen
            Clear( olc::BLACK );

            // little lambda to emulate CGE's Draw() call
            auto CGE_Draw = [=]( int x, int y, char c, olc::Pixel col = olc::WHITE ) {
                DrawString( x * 8, y * 8, std::string( 1, c ), col );
            };
            auto CGE_DrawString = [=]( int x, int y, std::string s, olc::Pixel col = olc::WHITE ) {
                DrawString( x * 8, y * 8, s, col );
            };

            // Draw stats & border
            for (int i = 1; i < ScreenWidth() - 1; i++) {
                CGE_Draw( i, 0, '=', olc::DARK_YELLOW );
                CGE_Draw( i, 2, '=', olc::DARK_YELLOW );
            }
            CGE_DrawString( 5, 1, "www.OneLoneCoder.com - S N A K E ! !                SCORE: " + std::to_string( nScore ), olc::YELLOW );

			// Draw Snake Body
			for (auto s : snake)
                CGE_Draw( s.x, s.y, bDead ? '+' : '0', olc::DARK_CYAN );
			// Draw Snake Head
            CGE_Draw( snake.front().x, snake.front().y, bDead ? 'X' : '@', olc::CYAN );
			// Draw Food
            CGE_Draw( nFoodX, nFoodY, '%', olc::RED );

            if (bDead) {
                std::string sText = "PRESS <SPACE> TO PLAY AGAIN";
                CGE_DrawString( (ScreenWidth() - sText.length()) / 2, ScreenHeight() / 2, sText, olc::MAGENTA );
                if (GetKey( olc::Key::SPACE ).bPressed) {
                    bDead = false;
                    ResetGame();
                }
            }

            return true;
        }
};

#define PIX_X     1
#define PIX_Y     2

int main()
{
    Snake_PGE game;
    if (game.Construct( nScreenWidth * 8, nScreenHeight * 8, PIX_X, PIX_Y ))
        game.Start();

    return 0;
}
