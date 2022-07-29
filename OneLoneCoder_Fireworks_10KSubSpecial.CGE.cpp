/*
	Live 10K Sub Special - Fireworks

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018-2019 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Relevant Video: https://www.youtube.com/watch?v=OmTD5IoqHJU

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
				https://www.youtube.com/javidx9extra
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Patreon:	https://www.patreon.com/javidx9
	Homepage:	https://www.onelonecoder.com

	Author
	~~~~~~
	David Barr, aka javidx9, ©OneLoneCoder 2019
*/

// NOTE: THIS CODE IS PROVIDED AS IS FROM THE END OF THE
// LIVE STREAM. SO ITS A MESS. ENJOY!


/* Alterations Joseph21 - 2022-07-29
 *   - implemented the code following the video
 *   - outcommented using namespace std; and compensated scope resolution where necessary
 */

//using namespace std;

#include "olcConsoleGameEngine.h"

class Celebration : public olcConsoleGameEngine
{
public:

    Celebration()
    {
        m_sAppName = L"Celebrate 10K";
    }

private:

    struct sParticle
    {
        float x = 0;
        float y = 0;
        float vx = 0;
        float vy = 0;
        float fuse = 0.0f;
        float lifetime = 0.0f;
        short col = FG_WHITE;
    };

    class cFirework : public sParticle
    {
    public:
        cFirework(float position, float ground, int size)
        {
            x = position;
            y = ground;
            vy = -100.0f;
            vx = RandomFloat(100.0f) - 50.0f;
            fuse = RandomFloat(2.0f) + 1.0f;
            starcount = size;
        }

        void Update(float fElapsedTime)
        {
            float fGravity = 25.0f;
            float fDrag = 0.999f;

            lifetime += fElapsedTime;
            if (lifetime <= fuse)
            {
                // We're launching
                vx *= fDrag;
                x += vx * fElapsedTime;
                y += (vy + fGravity) * fElapsedTime;
            }
            else
            {
                if (!bHasExploded)
                {
                    bHasExploded = true;
                    // Explode
                    short star_col = rand() % 8 + 8;
                    for (int i = 0; i < starcount; i++)
                    {
                        sParticle s;
                        s.x = x;
                        s.y = y;
                        float angle = RandomFloat(2.0f * 3.14159f);
                        float power = RandomFloat(50.0f);
                        s.fuse = RandomFloat(4.0f) + 1.0f;

                        s.vx = cosf(angle) * power;
                        s.vy = sinf(angle) * power;
                        s.col = star_col;
                        vecStars.push_back(s);
                    }
                }
                else
                {
                    for (auto &s : vecStars)
                    {
                        s.lifetime += fElapsedTime;
                        s.vx *= fDrag;
                        s.x += s.vx * fElapsedTime;
                        s.y += (s.vy + fGravity) * fElapsedTime;
                    }
                }
            }
        }

        float RandomFloat(float fMax)
        {
            return ((float)rand() / (float)RAND_MAX) * fMax;
        }

        void DrawSelf(olcConsoleGameEngine *gfx)
        {
            if (lifetime < fuse)
                gfx->Draw(x, y, PIXEL_SOLID, FG_WHITE);
            else
            {
                bExpired = true;
                for (auto &s : vecStars)
                {
                    if (s.lifetime <= s.fuse)
                    {
                        bExpired = false;
                        gfx->Draw(s.x, s.y, PIXEL_SOLID, s.col - ((s.lifetime >= s.fuse * 0.75f) ? 8 : 0));
                    }
                }
            }
        }

        std::vector<sParticle> vecStars;
        int starcount = 0;
        bool bHasExploded = false;
        bool bExpired = false;
    };

    std::list<cFirework> listFireworks;
    float fDelay = 1.0f;

protected:

    virtual bool OnUserCreate()
    {
        return true;
    }

    virtual bool OnUserUpdate( float fElapsedTime )
    {
        Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

        // Generate a new firework on mouse click
//        if (GetMouse(0).bReleased)
//        {
//            listFireworks.push_back({128.0f, (float)ScreenHeight(), 100});
//        }

        fDelay -= fElapsedTime;
        if (fDelay <= 0.0f)
        {
            fDelay = ((float)rand() / (float)RAND_MAX) * 2.0f + 0.1f;
            listFireworks.push_back({ 128.0f, (float)ScreenHeight(), rand() % 800 + 100 });
        }

        for (auto &f : listFireworks)
        {
            f.Update(fElapsedTime);
            f.DrawSelf(this);
        }

        listFireworks.remove_if([](const cFirework &f){ return f.bExpired; });

        return true;
    }
};

int main()
{
    // use olcConsoleGameEngine derived app
    Celebration game;
    if (game.ConstructConsole( 256, 240, 4, 4 ))
        game.Start();
    return 0;
}
