# From-ConsoleGameEngine-to-PixelGameEngine
Contains code that is ported from most of Javidx9s olcConsoleGameEngine videos to olcPixelGameEngine 

Being a fan of Javidx9 I implemented most of his videos. In 2017 he developed the olcConsoleGameEngine (CGE for short) as a Windows console based platform for game, concept and algorithm development. In 2018 the olcPixelGameEngine (PGE) v. 1.0 was introduced, and the emphasis in the videos shifted to development on the PGE. 

In January 2022 the CGE was deprecated and is no longer supported. It is still possible to follow the CGE based videos and doing the implementation on the deprecated CGE platform, but you can also do the implementation on the newer, much more feature rich, cross platform and supported PGE. 

I implemented most of the CGE videos on the CGE platform, and ported these implementations to the newer PGE platform afterwards. To help anyone who wants to follow these CGE based videos I decided to upload all these ports. I also put a cheat sheet that describes how to port from CGE to PGE yourself if you want. And all the sprite files are converted from .spr to .png format, using Gorbit's sprite converter code (and a bit of my own hackery).

The original code is there as well, but I took the liberty to outcomment the "using namespace std;" lines wherever I found them, and correct with scope resolution operators where necessary. You can recognize the code based on the CGE platform by the CGE in the file name. The file having the PGE in the name is the ported version of the code. I find it convenient to have them together to enable easy comparison of both versions.

Note: There are also a few programs that were developed for the bare Windows console originally (Snake, CaveDiver, Tetris and the first version of the First Person Shooter). This code is also included in this repo: I ported these programs to the ConsoleGameEngine first, and from there to the PixelGameEngine. The original (Windows console) versions can be identified by the CON in the file names.

I'm having no licence or copyright on this code whatsoever - the original GNU GPLv3 license of the original creator applies and is there in each of the code files as they were in the originals. 

In the header comment I added a little summary of the adaptations I did per file. 

Have fun with it!

Joseph21

#update june 2, 2024 - code also available via PGEtinker

Recently Moros1138 released his PGEtinker, a tool for realtime compiling and running PGE based code using Emscriptem for the translation to WASM. He also ported all the CGE to PGE ported code to PGEtinker.

Here's a list of links, courtesy of Moros1138!

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
