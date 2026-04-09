#pragma once

#include <iostream>
#include <fstream>

#include "SpriteSheet.h"

// TODO: maybe have some custom file instead of using this

std::string ws2s(const std::wstring& wstr);   // convenience, to convert std::wstring type to std::string

class Level;

class Tile {
private:
	Level *level = nullptr;
	int spriteId = 0;
	bool solid = false;

public:
	Tile() {}

	Tile(int _spriteId)
		: spriteId(_spriteId)
	{

	}

	void SetSpriteId(int nSpriteId) { spriteId = nSpriteId; }
	int  GetSpriteId() const { return spriteId; }
	void SetLevel(Level* pLevel) { level = pLevel; }
	Level* GetLevel() const { return level; }
	void SetSolid(bool bSolid) { solid = bSolid; }
	bool IsSolid() const { return solid; }

	olc::Sprite* GetSprite();

	friend std::istream& operator>>(std::istream& input, Tile& tile) {
		input >> tile.spriteId >> tile.solid;
		return input;
	}

	friend std::ostream& operator<<(std::ostream& output, const Tile& tile) {
		output << tile.spriteId << " " << tile.solid << " ";
		return output;
	}

};

class Level {
private:
	SpriteSheet spritesheet;
	Tile* tiles = nullptr;
	int mapWidth, mapHeight;

public:

	~Level() {
		delete[] tiles;
	}

	Level()
	{
	}

	Level(int mapWidth, int mapHeight)
		:	mapWidth(mapWidth),
			mapHeight(mapHeight)
	{
		tiles = new Tile[mapWidth * mapHeight];
		for (int i = 0; i < mapWidth * mapHeight; i++) {
			tiles[i].SetLevel(this);
		}
	}

	Level(const std::string& map)
	{
		Load(map);
	}

	Level(const std::string& map, const std::string& spriteSheet, int tileSize)
	{
		Load(map);
		LoadSpriteSheet(spriteSheet, tileSize);
	}

	void LoadSpriteSheet(const std::string& map, int tileSize) {
		spritesheet.Load(map, tileSize, tileSize);
	}

	void Create(int mapWidth, int mapHeight) {
		this->mapWidth  = mapWidth;
		this->mapHeight = mapHeight;
		delete[] tiles;
		tiles = new Tile[mapWidth * mapHeight];
	}

	void Load(const std::string& mapFile);
	void Save(const std::string& mapFile);

	inline Tile& operator[](int index) const { return tiles[index]; }

	inline int GetWidth( ) const { return this->mapWidth;  }
	inline int GetHeight() const { return this->mapHeight; }
	inline SpriteSheet* GetSpriteSheet() { return &this->spritesheet; }
};
