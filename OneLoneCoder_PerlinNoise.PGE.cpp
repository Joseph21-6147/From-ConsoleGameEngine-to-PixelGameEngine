/*
OneLoneCoder.com - What Is Perlin Noise?
"Mountains, Clouds, Worms Landscapes?" - @Javidx9

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
Noise is random - a bad thing when trying to procedurally generate content. Perlin
noise adds coherence at varying spatial scales which create natural looking noise
arrays. Perlin noise can be further processed into all sorts of assets, such as
mountains, maps, rooms, textures, data sets.


Author
~~~~~~
Twitter: @javidx9
Blog: www.onelonecoder.com

Video:
~~~~~~
https://youtu.be/6-0UaeJBumA

Last Updated: 29/10/2017
*/


/* Alterations Joseph21 - 20220716
 *   - removed "using namespace std" and added explicit scope resolution operator where needed
 * Port to PixelGameEngine - 20220716
 *   - see cheat sheet for generic adaptations needed for port
 *   - had to rewrite mode 2 and mode 3 color picking - this is much different for the PGE than for the CGE
 */

#include <iostream>
#include <string>
#include <algorithm>
//using namespace std;

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class OneLoneCoder_PerlinNoiseDemo : public olc::PixelGameEngine
{
public:
	OneLoneCoder_PerlinNoiseDemo()
	{
		sAppName = "Perlin Noise";
	}

private:
	// 2D noise variables
	int nOutputWidth = 256;
	int nOutputHeight = 256;
	float *fNoiseSeed2D = nullptr;
	float *fPerlinNoise2D = nullptr;

	// 1D noise variables
	float *fNoiseSeed1D = nullptr;
	float *fPerlinNoise1D = nullptr;
	int nOutputSize = 256;


	int nOctaveCount = 1;
	float fScalingBias = 2.0f;
	int nMode = 1;


	virtual bool OnUserCreate()
	{
		nOutputWidth = ScreenWidth();
		nOutputHeight = ScreenHeight();

		fNoiseSeed2D = new float[nOutputWidth * nOutputHeight];
		fPerlinNoise2D = new float[nOutputWidth * nOutputHeight];
		for (int i = 0; i < nOutputWidth * nOutputHeight; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;

		nOutputSize = ScreenWidth();
		fNoiseSeed1D = new float[nOutputSize];
		fPerlinNoise1D = new float[nOutputSize];
		for (int i = 0; i < nOutputSize; i++) fNoiseSeed1D[i] = (float)rand() / (float)RAND_MAX;

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

		if (GetKey(olc::Key::SPACE).bReleased)
			nOctaveCount++;

		if (GetKey(olc::Key::K1).bReleased)
			nMode = 1;

		if (GetKey(olc::Key::K2).bReleased)
			nMode = 2;

		if (GetKey(olc::Key::K3).bReleased)
			nMode = 3;

		if (GetKey(olc::Key::Q).bReleased)
			fScalingBias += 0.2f;

		if (GetKey(olc::Key::A).bReleased)
			fScalingBias -= 0.2f;

		if (fScalingBias < 0.2f)
			fScalingBias = 0.2f;

		if (nOctaveCount == 9)
			nOctaveCount = 1;

		if (nMode == 1) // 1D Noise
		{
			if (GetKey(olc::Key::Z).bReleased) // Noise Between 0 and +1
				for (int i = 0; i < nOutputSize; i++) fNoiseSeed1D[i] = (float)rand() / (float)RAND_MAX;

			if (GetKey(olc::Key::X).bReleased) // Noise Between -1 and +1
				for (int i = 0; i < nOutputSize; i++) fNoiseSeed1D[i] = 2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f;

			PerlinNoise1D(nOutputSize, fNoiseSeed1D, nOctaveCount, fScalingBias, fPerlinNoise1D);

			for (int x = 0; x < nOutputSize; x++)
			{
				int y = -(fPerlinNoise1D[x] * (float)ScreenHeight() / 2.0f) + (float)ScreenHeight() / 2.0f;
				if (y < ScreenHeight() / 2)
					for (int f = y; f < ScreenHeight() / 2; f++)
						Draw(x, f, olc::GREEN);
				else
					for (int f = ScreenHeight() / 2; f <= y; f++)
						Draw(x, f, olc::RED);
			}
		}

		if (nMode == 2) // 2D Noise
		{
			if (GetKey(olc::Key::Z).bReleased) // Noise Between 0 and +1
				for (int i = 0; i < nOutputWidth * nOutputHeight; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;


			PerlinNoise2D(nOutputWidth, nOutputHeight, fNoiseSeed2D, nOctaveCount, fScalingBias, fPerlinNoise2D);

			for (int x = 0; x < nOutputWidth; x++)
			{
				for (int y = 0; y < nOutputHeight; y++)
				{
                    olc::Pixel col;
					int pixel_bw = (int)(fPerlinNoise2D[y * nOutputWidth + x] * 255.0f);
                    col = olc::Pixel( pixel_bw, pixel_bw, pixel_bw );
					Draw( x, y, col );
				}
			}
		}

		if (nMode == 3) // 2D Noise - colourised
		{
			if (GetKey(olc::Key::Z).bReleased) // Noise Between 0 and +1
				for (int i = 0; i < nOutputWidth * nOutputHeight; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;


			PerlinNoise2D(nOutputWidth, nOutputHeight, fNoiseSeed2D, nOctaveCount, fScalingBias, fPerlinNoise2D);

			for (int x = 0; x < nOutputWidth; x++)
			{
				for (int y = 0; y < nOutputHeight; y++)
				{
                    olc::Pixel col;
					int pixel_bw = (int)(fPerlinNoise2D[y * nOutputWidth + x] * 11.0f);
					switch (pixel_bw)
					{
                        case  0: col = olc::BLACK;             break;
                        case  1: col = olc::VERY_DARK_BLUE;    break;
                        case  2: col = olc::DARK_BLUE;         break;
                        case  3: col = olc::BLUE;              break;
                        case  4: col = olc::VERY_DARK_GREEN;   break;
                        case  5: col = olc::DARK_GREEN;        break;
                        case  6: col = olc::GREEN;             break;
                        case  7: col = olc::VERY_DARK_YELLOW;  break;
                        case  8: col = olc::DARK_YELLOW;       break;
                        case  9: col = olc::GREY;              break;
                        case 10: col = olc::WHITE;             break;
					}

					Draw(x, y, col);
				}
			}
		}

		return true;
	}


	void PerlinNoise1D(int nCount, float *fSeed, int nOctaves, float fBias, float *fOutput)
	{
		// Used 1D Perlin Noise
		for (int x = 0; x < nCount; x++)
		{
			float fNoise = 0.0f;
			float fScaleAcc = 0.0f;
			float fScale = 1.0f;

			for (int o = 0; o < nOctaves; o++)
			{
				int nPitch = nCount >> o;
				int nSample1 = (x / nPitch) * nPitch;
				int nSample2 = (nSample1 + nPitch) % nCount;

				float fBlend = (float)(x - nSample1) / (float)nPitch;

				float fSample = (1.0f - fBlend) * fSeed[nSample1] + fBlend * fSeed[nSample2];

				fScaleAcc += fScale;
				fNoise += fSample * fScale;
				fScale = fScale / fBias;
			}

			// Scale to seed range
			fOutput[x] = fNoise / fScaleAcc;
		}
	}

	void PerlinNoise2D(int nWidth, int nHeight, float *fSeed, int nOctaves, float fBias, float *fOutput)
	{
		// Used 1D Perlin Noise
		for (int x = 0; x < nWidth; x++)
			for (int y = 0; y < nHeight; y++)
			{
				float fNoise = 0.0f;
				float fScaleAcc = 0.0f;
				float fScale = 1.0f;

				for (int o = 0; o < nOctaves; o++)
				{
					int nPitch = nWidth >> o;
					int nSampleX1 = (x / nPitch) * nPitch;
					int nSampleY1 = (y / nPitch) * nPitch;

					int nSampleX2 = (nSampleX1 + nPitch) % nWidth;
					int nSampleY2 = (nSampleY1 + nPitch) % nWidth;

					float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
					float fBlendY = (float)(x - nSampleY1) / (float)nPitch;

					float fSampleT = (1.0f - fBlendX) * fSeed[nSampleY1 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY1 * nWidth + nSampleX2];
					float fSampleB = (1.0f - fBlendX) * fSeed[nSampleY2 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY2 * nWidth + nSampleX2];

					fScaleAcc += fScale;
					fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
					fScale = fScale / fBias;
				}

				// Scale to seed range
				fOutput[y * nWidth + x] = fNoise / fScaleAcc;
			}

	}


};


int main()
{
	OneLoneCoder_PerlinNoiseDemo game;
	if (game.Construct(256, 256, 3, 3))
        game.Start();
	return 0;
}
