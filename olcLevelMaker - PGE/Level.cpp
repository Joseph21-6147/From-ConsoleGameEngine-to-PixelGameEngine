#include <iostream>
#include <fstream>

#include "Level.h"

olc::Sprite* Tile::GetSprite() {
	int tileCount = level->GetSpriteSheet()->GetTileCount();
	SpriteSheet* spr = level->GetSpriteSheet();
	if (spriteId >= tileCount) {
		return spr[0][0];
	}
	return spr[0][spriteId];
}


// added Joseph21

#include <locale>
#include <codecvt>

std::string ws2s(const std::wstring& wstr)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}

void Level::Load(const std::string& mapFile) {
	if (tiles != nullptr) {
		delete[] tiles;
	}

	std::string cnvMapFile = mapFile;
	std::ifstream map(cnvMapFile, std::ios::in | std::ios::binary);
	if (map.is_open()) {
		map >> mapWidth >> mapHeight;
		tiles = new Tile[mapWidth * mapHeight];
		for (int i = 0; i < mapWidth * mapHeight; i++) {
			map >> tiles[i];
			tiles[i].SetLevel(this);
		}
		map.close();
	}
}

void Level::Save(const std::string& mapFile) {

	std::string cnvMapFile = mapFile;
	std::ofstream map(cnvMapFile, std::ios::out | std::ios::binary);
	if (map.is_open()) {
		map << mapWidth << " " << mapHeight << "\n";
		for (int i = 0; i < mapWidth * mapHeight; i++) {
			map << tiles[i];
		}
		map.close();
	}
}
