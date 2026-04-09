#include "SpriteSheet.h"

void SpriteSheet::Load(const std::string& file, int tileWidth, int tileHeight) {

    std::cout << "SpriteSheet::Load() entered with filename: " << file << " and WxH: " << tileWidth << "x" << tileHeight << std::endl;

	this->tileWidth  = tileWidth;
	this->tileHeight = tileHeight;

	// read the input sprite file
	olc::Sprite* spritesheet = new olc::Sprite(file);
    // determine the number of tiles it contains
	int tileCountX = spritesheet->width  / tileWidth;
	int tileCountY = spritesheet->height / tileHeight;
	spriteCount = tileCountX * tileCountY;

	sprites.clear();

	for (int i = 0; i < spriteCount; i++) {
        olc::Sprite *pNewTileSprite = new olc::Sprite( tileWidth, tileHeight );
		int baseX =  (i          % tileCountX);
		int baseY = ((i - baseX) / tileCountX);
		baseX *= tileWidth;
		baseY *= tileHeight;
		// this could be done easier using Sprite::Duplicate() calls ig
		for (int y = 0; y < tileHeight; y++) {
			for (int x = 0; x < tileWidth; x++) {
				pNewTileSprite->SetPixel(x, y, spritesheet->GetPixel(baseX + x, baseY + y));
			}
		}
        sprites.push_back( pNewTileSprite );
	}
    // now all tiles have their own sprite, the input sprite sheet is no longer needed
	delete spritesheet;

    std::cout << "SpriteSheet::Load() exited" << std::endl;
}
