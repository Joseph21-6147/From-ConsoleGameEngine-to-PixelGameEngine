# From-ConsoleGameEngine-to-PixelGameEngine

_July 19, 2022 - Inital posting_

This repo contains code that is ported from most of Javidx9s olcConsoleGameEngine videos to olcPixelGameEngine.

Being a fan of Javidx9 I implemented most of his videos. In 2017 he developed the olcConsoleGameEngine (CGE for short) as a Windows console based platform for game, concept and algorithm development. In 2018 the olcPixelGameEngine (PGE) v. 1.0 was introduced, and the emphasis in the videos shifted to development on the PGE.

As of January 2022 the CGE was deprecated and is no longer supported. For some time it was still possible to run CGE-based code, but with the upgrade to Windows 11 the CGE no longer functioned correctly. Fortunately you can also do the implementation on the newer, much more feature rich, cross platform and supported PGE.

Initially I implemented most of the CGE videos on the CGE platform, and ported these implementations to the newer PGE platform afterwards. To help anyone who wants to follow these CGE based videos I decided to upload all these ports. I also put a cheat sheet that describes how to port from CGE to PGE yourself if you want. And all the sprite files are converted from .spr to .png format, using Gorbit's sprite converter code (and a bit of my own hackery).

The original CGE code is there as well, but I took the liberty to outcomment the "using namespace std;" lines wherever I found them, and correct with scope resolution operators where necessary. You can recognize the code based on the CGE platform by the CGE in the file name. The file having the PGE in the name is the ported version of the code (I find it convenient to have them together to enable easy comparison of both versions).

Note: There is also a handful of programs that were developed for the bare Windows console originally (Snake, CaveDiver, Tetris and the first version of the First Person Shooter). This code pre dates the CGE, but is also included in this repo: I ported these programs to the ConsoleGameEngine first, and from there to the PixelGameEngine. The original (Windows console) versions can be identified by the CON in the file names.

I'm having no licence or copyright on this code whatsoever - the original GNU GPLv3 license of the original creator applies and is there in each of the code files as they were in the originals.

In the header comment I added a little summary of the adaptations I did per file.

_Update june 2, 2024 - code also available via PGEtinker_

In may 2024 Moros1138 released his PGEtinker, a tool for realtime compiling and running PGE based code in the browser (!) using Emscriptem for the translation to WASM. (It probably contains much more very smart tricks and techniques, which I'm not able to explain :) ) He also ported all the CGE to PGE ported code to PGEtinker. Below in this readme is a list of links where you can run these programs directly from your browser, courtesy of Moros1138!

_Update may 1, 2025 - SGE versions in the repo (not completed)_

A couple of years ago I wrote a port of the Pixel Game Engine (PGE) by Javidx9, using C++ and SDL2, and called it SDL-based Game Engine (SGE for short). The code can be found elsewhere in my github. As of april 2025 I started adding SGE versions of PGE programs to this repo (but never completed it). If you are not interested in the SDL-based Game Engine, just ignore these SGE files. 

_Update april 2, 2026 - CGE code is executable again!_

On the olc discord server voidstar-dev published an SDL2/OpenGL based port of the ConsoleGameEngine header file. I tested this header file on most of the CGE code in this repo, and everything appeared to work fine! Thanks to the efforts of this fellow coder it has become possible again to run CGE based code without porting it to PGE, cheers voidstar-dev! The header file and libs can be found here: https://gitlab.com/voidstar-dev/olc-console-game-engine-port

Have fun with it!

Joseph21


_Special thanks to:_

* **Javidx9** - for the CGE, the PGE, the console based code and all the inspiration you provided

* **Gorbit** - for the spr to png converter code

* **Moros1138** - for the PGEtinker 

* **voidstar-dev** - for the SDL2/OpenGL based port of the Console Game Engine include file



_PGEtinker links_

OneLoneCoder_Asteroids.PGE.cpp                   - https://beta.pgetinker.com/s/98dA5VR2lMU

OneLoneCoder_Balls1.PGE.cpp                      - https://beta.pgetinker.com/s/r2qCfSOO3HU

OneLoneCoder_Balls2.PGE.cpp                      - https://beta.pgetinker.com/s/T6KnEfy4M-

OneLoneCoder_CaveDiver.PGE.cpp                   - https://beta.pgetinker.com/s/Iw8MqeZWjz5

OneLoneCoder_CommandLineFPS.PGE.cpp              - https://beta.pgetinker.com/s/kp20NorMY3C

OneLoneCoder_CommandLineFPS_2.PGE.cpp            - https://beta.pgetinker.com/s/Sg5Q2flDNna

OneLoneCoder_Fireworks_10KSubSpecial.PGE.cpp     - https://beta.pgetinker.com/s/3lfLR6elx7q

OneLoneCoder_FlappyBird.PGE.cpp                  - https://beta.pgetinker.com/s/3eYwcEz311

OneLoneCoder_Frogger.PGE.cpp                     - https://beta.pgetinker.com/s/d1WUYCMAhKS

OneLoneCoder_GameOfLife.PGE.cpp                  - https://beta.pgetinker.com/s/obiToOT98y6

OneLoneCoder_LudumDare42.PGE.cpp                 - https://beta.pgetinker.com/s/rmUZohYfS89

OneLoneCoder_MatchingGems_50KSubSpecial.PGE.cpp  - https://beta.pgetinker.com/s/cJXN5Fxons

OneLoneCoder_Matrix.PGE.cpp                      - https://beta.pgetinker.com/s/ceC3MIPakJF

OneLoneCoder_Mazes.PGE.cpp                       - https://beta.pgetinker.com/s/jm34y5fFGc

OneLoneCoder_olcEngine3D_Part1.PGE.cpp           - https://beta.pgetinker.com/s/FuNihLD9MG

OneLoneCoder_olcEngine3D_Part2.PGE.cpp           - https://beta.pgetinker.com/s/JJuMnMDWdcu

OneLoneCoder_olcEngine3D_Part3.PGE.cpp           - https://beta.pgetinker.com/s/hnLeVLiCNj

OneLoneCoder_olcEngine3D_Part4.PGE.cpp           - https://beta.pgetinker.com/s/TEkNFkol3c

OneLoneCoder_PanAndZoom.PGE.cpp                  - https://beta.pgetinker.com/s/jJhBVmxHafZ

OneLoneCoder_PathFinding_AStar.PGE.cpp           - https://beta.pgetinker.com/s/N2ki6Ip3XXW

OneLoneCoder_PerlinNoise.PGE.cpp                 - https://beta.pgetinker.com/s/U89runhyg1

OneLoneCoder_PlatformGame1.PGE.cpp               - https://beta.pgetinker.com/s/rMSJZETWEFy

OneLoneCoder_Pseudo3DPlanesMode7.PGE.cpp         - https://beta.pgetinker.com/s/KNdCipg0fuB

OneLoneCoder_RacingLines.PGE.cpp                 - https://beta.pgetinker.com/s/eTwmKacI1RO

OneLoneCoder_RetroArcadeRacer.PGE.cpp            - https://beta.pgetinker.com/s/J5OVmuW1Kd5

OneLoneCoder_Snake.PGE.cpp                       - https://beta.pgetinker.com/s/aPSfbnEMct

OneLoneCoder_Splines1.PGE.cpp                    - https://beta.pgetinker.com/s/8AYfnVW9G8F

OneLoneCoder_Splines2.PGE.cpp                    - https://beta.pgetinker.com/s/-CI0KKN3AIh

OneLoneCoder_Tetris.PGE.cpp                      - https://beta.pgetinker.com/s/yNkUjCXqhIp

OneLoneCoder_Worms1.PGE.cpp                      - https://beta.pgetinker.com/s/EhIklVCtthV

OneLoneCoder_Worms2.PGE.cpp                      - https://beta.pgetinker.com/s/YR0KVrjkure

OneLoneCoder_Worms3.PGE.cpp                      - https://beta.pgetinker.com/s/cPLcxz0SEMT



