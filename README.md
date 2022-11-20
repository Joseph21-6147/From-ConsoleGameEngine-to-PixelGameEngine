# From-ConsoleGameEngine-to-PixelGameEngine
Contains code that is ported to olcPixelGameEngine from most of Javidx9's olcConsoleGameEngine video's

Being a fan of Javidx9 I implemented most of his video's. In 2017 he developed the olcConsoleGameEngine (CGE for short) as a console based platform for game, concept and algorithm developement. In 2018 the olcPixelGameEngine (PGE) v. 1.0 was introduced, and the emphasis in the video's shifted to development on the PGE. 

Recently the CGE was deprecated and is no longer supported. It is still possible to follow the CGE based video's and doing the implementation on the deprecated CGE platform, but you can also do the implementation on the newer, much more feature rich, cross platform and supported PGE. 

I implemented most of the CGE video's on the CGE platform, and ported most of them to the newer PGE platform afterwards. To help anyone who wants to follow these CGE based video's I decided to upload all these ports. I also put a cheat sheet that describes how to port from CGE to PGE yourself if you want.

The original code is there as well, but I took the liberty to outcomment the "using namespace std;" lines wherever I found them, and correct with scope resolution operators where necessary. You can recognize the code based on the CGE platform by the CGE in the file name. The file having the PGE in the name is the ported version of the code. It's convenient to have them together to enable easy comparison of both versions.

Note: There are a few programs that were developed for the console originally (Snake, CaveDiver, Tetris and version 1 of the First Person Shooter). I ported these programs to the ConsoleGameEngine and from there to the PixelGameEngine. The original (console) versions can be identified by the CON in the file names.

I'm having no licence or copyright on this code whatsoever - the original GNU GPLv3 license of the original creator applies and is there in each of the code files as they were in the originals. 

In the header comment I added a little summary of the adaptations I did per file. 

Have fun with it!
Joseph21
