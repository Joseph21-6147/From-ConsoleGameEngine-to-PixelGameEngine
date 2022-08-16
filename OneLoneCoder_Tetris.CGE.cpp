/*
	OneLoneCoder.com - Command Line Tetris
	"Put Your Money Where Your Mouth Is" - @Javidx9

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
	I made a video "8-Bits of advice for new programmers" (https://youtu.be/vVRCJ52g5m4)
	and suggested that building a tetris clone instead of Dark Sould IV might be a better
	approach to learning to code. Tetris is nice as it makes you think about algorithms.

	Controls are Arrow keys Left, Right & Down. Use Z to rotate the piece.
	You score 25pts per tetronimo, and 2^(number of lines)*100 when you get lines.

	Future Modifications
	~~~~~~~~~~~~~~~~~~~~
	1) Show next block and line counter

	Author
	~~~~~~
	Twitter: @javidx9
	Blog: www.onelonecoder.com

	Video:
	~~~~~~
	https://youtu.be/8OK8_tHeCIA

	Last Updated: 30/03/2017
*/


/* Alterations to original (console) code - Joseph21 - 2022-08-16
 *   - removed "using namespace std" and added explicit scope resolution operator where needed
 *   - added "using namespace std::chrono_literals to resolve errors on ms literal
 * Port to ConsoleGameEngine - 2022-08-16
 *   - removed a number of include directives since they are already done in olcConsoleGameEngine.h
 *   - Created a class Tetris_CGE and put most of the before global variables in it as class variables
 *   - Added OnUserCreate() and put tetromino initialization in it
 *   - Added OnUserUpdate() and put all user interaction in it (calls to GetAsyncKeyState() rewritten
 *     to calls to GetKey(), applied in a little lambda key_touched())
 *   - Replaced the call to swprintf_s() with DrawString() call with the same (score) information
 *   - Replaced all references to screen[] with calls to Draw()
 *   - Rewrote main() to construct and start the console game engine
 */

#include <thread>

using namespace std::chrono_literals;

#include "olcConsoleGameEngine.h"

#define PIX_X      8
#define PIX_Y     16

int nScreenWidth = 80;			// Console Screen Size X (columns)
int nScreenHeight = 30;			// Console Screen Size Y (rows)

int nScore = 0;

class Tetris_CGE : public olcConsoleGameEngine {
public:

    Tetris_CGE() {
        m_sAppName = L"Tetris_CGE";
    }

private:
    std::wstring tetromino[7];       // for storing tetris objects

    int nFieldWidth  = 12;      // dimensions of playing field
    int nFieldHeight = 18;
    unsigned char *pField = nullptr;   // pointer to dyn. alloc.d playing field

    int Rotate(int px, int py, int r)
    {
        int pi = 0;
        switch (r % 4)
        {
        case 0: // 0 degrees			// 0  1  2  3
            pi = py * 4 + px;			// 4  5  6  7
            break;						// 8  9 10 11
                                        //12 13 14 15

        case 1: // 90 degrees			//12  8  4  0
            pi = 12 + py - (px * 4);	//13  9  5  1
            break;						//14 10  6  2
                                        //15 11  7  3

        case 2: // 180 degrees			//15 14 13 12
            pi = 15 - (py * 4) - px;	//11 10  9  8
            break;						// 7  6  5  4
                                        // 3  2  1  0

        case 3: // 270 degrees			// 3  7 11 15
            pi = 3 - py + (px * 4);		// 2  6 10 14
            break;						// 1  5  9 13
        }								// 0  4  8 12

        return pi;
    }

    bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
    {
        // All Field cells >0 are occupied
        for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++)
            {
                // Get index into piece
                int pi = Rotate(px, py, nRotation);

                // Get index into field
                int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

                // Check that test is in bounds. Note out of bounds does
                // not necessarily mean a fail, as the long vertical piece
                // can have cells that lie outside the boundary, so we'll
                // just ignore them
                if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
                {
                    if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
                    {
                        // In Bounds so do collision check
                        if (tetromino[nTetromino][pi] != L'.' && pField[fi] != 0)
                            return false; // fail on first hit
                    }
                }
            }

        return true;
    }

	int nCurrentPiece = 0;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2;
	int nCurrentY = 0;
	int nSpeed = 20;
	int nSpeedCount = 0;
	bool bForceDown = false;
	bool bRotateHold = true;
	int nPieceCount = 0;
	std::vector<int> vLines;
	bool bGameOver = false;

protected:
    virtual bool OnUserCreate()
    {
        tetromino[0].append(L"..X...X...X...X."); // Tetronimos 4x4
        tetromino[1].append(L"..X..XX...X.....");
        tetromino[2].append(L".....XX..XX.....");
        tetromino[3].append(L"..X..XX..X......");
        tetromino[4].append(L".X...XX...X.....");
        tetromino[5].append(L".X...X...XX.....");
        tetromino[6].append(L"..X...X..XX.....");

        pField = new unsigned char[nFieldWidth*nFieldHeight]; // Create play field buffer
        for (int x = 0; x < nFieldWidth; x++) // Board Boundary
            for (int y = 0; y < nFieldHeight; y++)
                pField[y*nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

        return true;
    }

    virtual bool OnUserUpdate( float fElapsedTime )
	{
		// Timing =======================
		std::this_thread::sleep_for(50ms); // Small Step = 1 Game Tick
		nSpeedCount++;
		bForceDown = (nSpeedCount == nSpeed);

		// Input ========================

        auto key_touched = [=]( int nKey ) {
            return (GetKey( nKey ).bPressed || GetKey( nKey ).bHeld);
        };

		// Handle player movement
        if (key_touched( VK_RIGHT ) && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY )) {
                nCurrentX = nCurrentX + 1;
        }
        if (key_touched( VK_LEFT ) && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY )) {
                nCurrentX = nCurrentX - 1;
        }
        if (key_touched( VK_DOWN ) && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1 )) {
                nCurrentY = nCurrentY + 1;
        }

		// Rotate, but latch to stop wild spinning
		if (key_touched( L'Z' ))
		{
			nCurrentRotation += (bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
			bRotateHold = false;
		}
		else
			bRotateHold = true;

		// Game Logic ===================

		// Force the piece down the playfield if it's time
		if (bForceDown)
		{
			// Update difficulty every 50 pieces
			nSpeedCount = 0;
			nPieceCount++;
			if (nPieceCount % 50 == 0)
				if (nSpeed >= 10) nSpeed--;

			// Test if piece can be moved down
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY++; // It can, so do it!
			else
			{
				// It can't! Lock the piece in place
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

				// Check for lines
				for (int py = 0; py < 4; py++)
					if(nCurrentY + py < nFieldHeight - 1)
					{
						bool bLine = true;
						for (int px = 1; px < nFieldWidth - 1; px++)
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

						if (bLine)
						{
							// Remove Line, set to =
							for (int px = 1; px < nFieldWidth - 1; px++)
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;
							vLines.push_back(nCurrentY + py);
						}
					}

				nScore += 25;
				if(!vLines.empty())	nScore += (1 << vLines.size()) * 100;

				// Pick New Piece
				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;

				// If piece does not fit straight away, game over!
				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}
		}

		// Display ======================

		// Draw Field
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
                Draw( x + 2, y + 2, L" ABCDEFG=#"[pField[y*nFieldWidth + x]] );

		// Draw Current Piece
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
                    Draw( nCurrentX + px + 2, nCurrentY + py + 2, nCurrentPiece + 65 );

		// Draw Score
        std::wstring sScore = L"SCORE: " + std::to_wstring( nScore );
        DrawString( nFieldWidth + 6, 2, sScore );

		// Animate Line Completion
		if (!vLines.empty())
		{
			for (auto &v : vLines)
				for (int px = 1; px < nFieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
					pField[px] = 0;
				}
			vLines.clear();
		}

        return !bGameOver;
	}
};

int main()
{
    // use olcConsoleGameEngine derived app
    Tetris_CGE game;
    game.ConstructConsole( nScreenWidth, nScreenHeight, PIX_X, PIX_Y );
    game.Start();

    // Oh Dear - game over!
    std::cout << "Game over!! Score: " << nScore << std::endl;
    system("pause");

    return 0;
}

