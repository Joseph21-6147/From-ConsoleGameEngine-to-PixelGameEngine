#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "olcPixelGameEngine.h"

// This objects create separate tiles for a tile sheet, one sprite per tile, which can then be
// accessed using indexing on the object
class SpriteSheet {
private:
	std::vector<olc::Sprite *> sprites;

	int spriteCount;
	int tileWidth, tileHeight;

public:


	SpriteSheet() {}

	SpriteSheet(const std::string& filename, int tileWidth, int tileHeight = -1) {
		Load(filename, tileWidth, tileHeight);
	}

	~SpriteSheet() { sprites.clear(); }

	void Load(const std::string& filename, int tileWidth, int tileHeight);

	olc::Sprite* operator[](int index) const {
		return sprites[index];
	}

	inline int GetTileWidth( ) const { return this->tileWidth;   }
	inline int GetTileHeight() const { return this->tileHeight;  }
	inline int GetTileCount( ) const { return this->spriteCount; }
};

