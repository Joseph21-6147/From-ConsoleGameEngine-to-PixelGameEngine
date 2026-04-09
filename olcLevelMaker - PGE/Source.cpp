#define OLC_IMAGE_STB // to make PGE use stb_image.h

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


#include <string>
#include <queue>
#include <sstream>

#include "SpriteSheet.h"
#include "Level.h"

#define FONT_SPRITESHEET "sprites/javidx9_nesfont8x8.png"  // used to be .spr files, now .png versions
#define TILE_SPRITESHEET "sprites/toml_spritesheetdark.png"

#define TILE_WIDTH    16

#define MAP_WIDTH     10
#define MAP_HEIGHT    10

#define DEFAULT_TILE  14

enum class Tool {
	TILES,
	META,
	EXPORT_IMPORT,
	LAST
};

// embeded fill icon meta, should have a better way to store this...
int fillSpriteWidth   = 8;
int fillSpriteHeight  = 8;
std::vector<int> fillSpriteData = {
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0, 15,  0,  0,  0,
    0, 12, 12, 12, 12, 15,  0,  0,
    0, 12, 15, 12, 12, 12, 15,  0,
    0, 12,  0, 15, 12, 12, 12, 15,
    0, 12,  0,  0, 15, 12, 15,  0,
    0, 12,  0,  0,  0, 15,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0
};

olc::Pixel CGE2PGE_colour( int nColValue ) {
    switch (nColValue) {
        case  0: return olc::BLACK	     ;
        case  1: return olc::DARK_BLUE   ;
        case  2: return olc::DARK_GREEN  ;
        case  3: return olc::DARK_CYAN   ;
        case  4: return olc::DARK_RED    ;
        case  5: return olc::DARK_MAGENTA;
        case  6: return olc::DARK_YELLOW ;
        case  7: return olc::GREY		 ;
        case  8: return olc::DARK_GREY   ;
        case  9: return olc::BLUE		 ;
        case 10: return olc::GREEN		 ;
        case 11: return olc::CYAN		 ;
        case 12: return olc::RED		 ;
        case 13: return olc::MAGENTA	 ;
        case 14: return olc::YELLOW		 ;
        case 15: return olc::WHITE		 ;
    };
    return olc::VERY_DARK_YELLOW;
}

enum class Popup {
	NONE,
	NEW_MAP_SIZE,
};

struct popup_t {
	Popup popup = Popup::NONE;
	bool menuActive = false;
	struct {
		std::string width;
		std::string height;
		int field; // 0 - width, 1 - height
	} newMapSize;

	popup_t() {}
	~popup_t() {}
};

class olcLevelMaker : public olc::PixelGameEngine {

public:
	olcLevelMaker() {
		sAppName = "olcLevelMaker - by Itay2805 [port to PGE: Joseph21]";
	}

private:
	float mapMoveX, mapMoveY;
	SpriteSheet font;
	SpriteSheet* tiles;

	olc::Sprite fillIcon;

	Tool tool = Tool::TILES;
	Level level;
	int page = 0;
	int selectedSprite = 0;
	int pageCount = 1;
	int uiBase = 300, tilesPerRow, tilesPerColumn, tilesPerPage, uiWidth;
	float worldOffsetX = 0, worldOffsetY = 0;
	bool grid = false;
	bool floodMode = false;

	bool pickedFirst = false, pickedSecond = false;
	std::pair<int, int> startRec;
	std::pair<int, int> endRex;

	std::string file = "";
	std::string spriteSheetFile = TILE_SPRITESHEET;

	popup_t popup;

	// displays string characters at screen location (x, y)
	void DrawStringFont(int x, int y, const std::string& characters) {
		for (char c : characters) {
			if (c - ' ' >= font.GetTileCount()) {
				continue;
			}
			DrawSprite(x, y, font[c - ' ']);
			x += font.GetTileWidth();
		}
	}

	virtual bool OnUserCreate() {

        // get font sprite sheet - to enable text display
		font.Load(FONT_SPRITESHEET, 8, 8);
		// create and fill fillIcon sprite (a little overflowing can) with pre defined data
		fillIcon = olc::Sprite(fillSpriteWidth, fillSpriteHeight);
		for (int sy = 0; sy < fillIcon.height; sy++) {
			for (int sx = 0; sx < fillIcon.width; sx++) {
				fillIcon.SetPixel( sx, sy, CGE2PGE_colour( fillSpriteData[ sy * fillSpriteWidth + sx ] ));
			}
		}

		// Create a default map with default sizes. Fill level pointer and set default tile
		level.Create(MAP_WIDTH, MAP_HEIGHT);
		for (int i = 0; i < level.GetWidth() * level.GetHeight(); i++) {
			level[i].SetLevel(&level);
			level[i].SetSpriteId(DEFAULT_TILE);
		}

		// search for the default sprite sheet
		std::fstream spriteFile(TILE_SPRITESHEET);
		if (!spriteFile.good()) {
			ImportSpriteSheet();
		}
		else {
			level.LoadSpriteSheet(TILE_SPRITESHEET, TILE_WIDTH);
			tiles = level.GetSpriteSheet();
		}
		if (spriteFile.is_open()) {
			spriteFile.close();
		}

		uiWidth = 400 - uiBase;
		tilesPerRow = uiWidth / tiles->GetTileWidth();
		tilesPerColumn = (200 - 23) / tiles->GetTileHeight();
		tilesPerPage = tilesPerColumn * tilesPerRow;
		pageCount = (tiles->GetTileCount() / tilesPerPage) + 1;

		return true;
	}

	bool moved = false;

	virtual bool OnUserUpdate(float fElapsedTime) {

		float fTileX = GetMouseX() / 16;
		float fTileY = GetMouseY() / 16;
		// simple
		int tileX = (int)(std::round(fTileX - worldOffsetX / 16));
		int tileY = (int)(std::round(fTileY - worldOffsetY / 16));

		// clear screen
		Clear( olc::BLACK );

		int topTileX = -(floor(worldOffsetX / 16)) - 1;
		int topTileY = -(floor(worldOffsetY / 16)) - 1;

		// draw level map
		for (int y = topTileY; y < topTileY + ceil(200.0 / TILE_WIDTH); y++) {
			for (int x = topTileX; x < topTileX + ceil(300.0 / TILE_WIDTH); x++) {
				if (x < 0 || x >= level.GetWidth() || y < 0 || y >= level.GetHeight()) continue;
				int i = x + y * level.GetWidth();
				if (i < 0 || i >= level.GetWidth() * level.GetHeight()) continue;
				int screenX = x * TILE_WIDTH + worldOffsetX;
				int screenY = y * TILE_WIDTH + worldOffsetY;
				if (screenX < -16 || screenX >= 300 || screenY < -16 || screenY >= 200) continue;
				if (screenX < 0) screenX = 0;
				if (screenY < 0) screenY = 0;
				DrawSprite(screenX, screenY, level[i].GetSprite());
				if (grid) {
					DrawLine(screenX             , screenY             , screenX + TILE_WIDTH, screenY             , olc::BLACK);
					DrawLine(screenX             , screenY             , screenX             , screenY + TILE_WIDTH, olc::BLACK);
					DrawLine(screenX + TILE_WIDTH, screenY             , screenX + TILE_WIDTH, screenY + TILE_WIDTH, olc::BLACK);
					DrawLine(screenX             , screenY + TILE_WIDTH, screenX + TILE_WIDTH, screenY + TILE_WIDTH, olc::BLACK);
				}
				if (tool == Tool::META) {
					int offset = 0;
					if (level[i].IsSolid()) {
						FillRect(screenX + offset, screenY + offset, 3, 3, olc::RED);
						offset += 3;
					}
				}
			}
		}

		if (popup.menuActive) {
			switch (popup.popup) {
			    case Popup::NONE: /* Just to prevent warnings */ break;
                case Popup::NEW_MAP_SIZE: {

#pragma region New Map Size Popup
                    FillRect(110, 70, 290-110, 150-70, olc::DARK_GREY);
                    DrawStringFont(150, 75, "New Map Size");
                    DrawStringFont(125, 90, "Width");
                    DrawStringFont(210, 90, "Height");
                    FillRect(170, 130, 240-170, 140-130, olc::GREY);
                    DrawStringFont(180, 131, "Create");
                    switch (popup.newMapSize.field) {
                        case 0:
                            FillRect(125, 100, 200-125, 108-100, olc::GREY);
                            FillRect(210, 100, 280-210, 108-100, olc::BLACK);
                            if (IsFocused()) {
                                if (GetKey(olc::Key::K0  ).bPressed || GetKey(olc::Key::NP0).bPressed) popup.newMapSize.width.append("0");
                                if (GetKey(olc::Key::K1  ).bPressed || GetKey(olc::Key::NP1).bPressed) popup.newMapSize.width.append("1");
                                if (GetKey(olc::Key::K2  ).bPressed || GetKey(olc::Key::NP2).bPressed) popup.newMapSize.width.append("2");
                                if (GetKey(olc::Key::K3  ).bPressed || GetKey(olc::Key::NP3).bPressed) popup.newMapSize.width.append("3");
                                if (GetKey(olc::Key::K4  ).bPressed || GetKey(olc::Key::NP4).bPressed) popup.newMapSize.width.append("4");
                                if (GetKey(olc::Key::K5  ).bPressed || GetKey(olc::Key::NP5).bPressed) popup.newMapSize.width.append("5");
                                if (GetKey(olc::Key::K6  ).bPressed || GetKey(olc::Key::NP6).bPressed) popup.newMapSize.width.append("6");
                                if (GetKey(olc::Key::K7  ).bPressed || GetKey(olc::Key::NP7).bPressed) popup.newMapSize.width.append("7");
                                if (GetKey(olc::Key::K8  ).bPressed || GetKey(olc::Key::NP8).bPressed) popup.newMapSize.width.append("8");
                                if (GetKey(olc::Key::K9  ).bPressed || GetKey(olc::Key::NP9).bPressed) popup.newMapSize.width.append("9");
                                if (GetKey(olc::Key::BACK).bPressed) if (popup.newMapSize.width.length() != 0) popup.newMapSize.width = popup.newMapSize.width.substr(0, popup.newMapSize.width.size() - 1);
                            }
                            break;
                        case 1:
                            FillRect(125, 100, 200-125, 108-100, olc::BLACK);
                            FillRect(210, 100, 280-210, 108-100, olc::GREY);
                            if (IsFocused()) {
                                if (GetKey(olc::Key::K0  ).bPressed || GetKey(olc::Key::NP0).bPressed) popup.newMapSize.height.append("0");
                                if (GetKey(olc::Key::K1  ).bPressed || GetKey(olc::Key::NP1).bPressed) popup.newMapSize.height.append("1");
                                if (GetKey(olc::Key::K2  ).bPressed || GetKey(olc::Key::NP2).bPressed) popup.newMapSize.height.append("2");
                                if (GetKey(olc::Key::K3  ).bPressed || GetKey(olc::Key::NP3).bPressed) popup.newMapSize.height.append("3");
                                if (GetKey(olc::Key::K4  ).bPressed || GetKey(olc::Key::NP4).bPressed) popup.newMapSize.height.append("4");
                                if (GetKey(olc::Key::K5  ).bPressed || GetKey(olc::Key::NP5).bPressed) popup.newMapSize.height.append("5");
                                if (GetKey(olc::Key::K6  ).bPressed || GetKey(olc::Key::NP6).bPressed) popup.newMapSize.height.append("6");
                                if (GetKey(olc::Key::K7  ).bPressed || GetKey(olc::Key::NP7).bPressed) popup.newMapSize.height.append("7");
                                if (GetKey(olc::Key::K8  ).bPressed || GetKey(olc::Key::NP8).bPressed) popup.newMapSize.height.append("8");
                                if (GetKey(olc::Key::K9  ).bPressed || GetKey(olc::Key::NP9).bPressed) popup.newMapSize.height.append("9");
                                if (GetKey(olc::Key::BACK).bPressed) if (popup.newMapSize.height.length() != 0) popup.newMapSize.height = popup.newMapSize.height.substr(0, popup.newMapSize.height.size() - 1);
                            }
                            break;
                        default:
                            popup.newMapSize.field = 0;
                    }
                    DrawStringFont(125, 100, popup.newMapSize.width);
                    DrawStringFont(210, 100, popup.newMapSize.height);
                    if (GetMouse(0).bPressed) {
                        if (GetMouseX() > 125 && GetMouseX() < 200 && GetMouseY() > 100 && GetMouseY() < 108) {
                            popup.newMapSize.field = 0;
                        }
                        if (GetMouseX() > 210 && GetMouseX() < 280 && GetMouseY() > 100 && GetMouseY() < 108) {
                            popup.newMapSize.field = 1;
                        }
                        if (GetMouseX() > 170 && GetMouseX() < 240 && GetMouseY() > 130 && GetMouseY() < 140) {
                            // Create the map
                            popup.popup = Popup::NONE;
                            popup.menuActive = false;
                            int width = stoi(popup.newMapSize.width);
                            int height = stoi(popup.newMapSize.height);
                            if (width != 0 && height != 0) {
                                popup.newMapSize.width = "";
                                popup.newMapSize.height = "";
                                level.Create(width, height);
                                for (int i = 0; i < level.GetWidth() * level.GetHeight(); i++) {
                                    level[i].SetLevel(&level);
                                    level[i].SetSpriteId(DEFAULT_TILE);
                                }
                                level.LoadSpriteSheet(spriteSheetFile, TILE_WIDTH);
                                tiles = level.GetSpriteSheet();
                                file = "";
                            }
                        }
                    }
                }

#pragma endregion

				break;
			}
		}
		else if(IsFocused()) {
			// fill the menu
			FillRect(uiBase, 0, 400 - uiBase, 200 - 0, olc::BLACK);

			if (!GetKey(olc::Key::CTRL).bHeld && ((pickedFirst && pickedSecond) || (!pickedFirst && !pickedSecond))) {
				if (tool == Tool::TILES) tilesTool(tileX, tileY);
				if (tool == Tool::META) metaTool(tileX, tileY);
				if (tool == Tool::EXPORT_IMPORT) exportAndImportTool();
			}

			if (tileX >= 0 && tileY >= 0 && tileX < level.GetWidth() && tileY < level.GetHeight() && GetMouseX() <= 300) {
				// draw coords
				std::string str("<");
				str.append(std::to_string(tileX));
				str.append(", ");
				str.append(std::to_string(tileY));
				str.append(">");
				DrawStringFont(0, 0, str);

				// draw hovered tile rect
				if (pickedFirst) {
					int firstRectTileX = startRec.first;
					int firstRectTileY = startRec.second;
					int secondRectTileX = tileX;
					int secondRectTileY = tileY;
					if (pickedSecond) {
						secondRectTileX = endRex.first;
						secondRectTileY = endRex.second;
					}

					int rectWidth = abs(firstRectTileX - secondRectTileX);
					int rectHeight = abs(firstRectTileY - secondRectTileY);
					rectHeight++;
					rectWidth++;

					DrawLine(firstRectTileX * 16 +                  worldOffsetX, firstRectTileY * 16 +                   worldOffsetY, firstRectTileX * 16 + 16 * rectWidth + worldOffsetX, firstRectTileY * 16 +                   worldOffsetY, olc::GREY);
					DrawLine(firstRectTileX * 16 +                  worldOffsetX, firstRectTileY * 16 +                   worldOffsetY, firstRectTileX * 16 +                  worldOffsetX, firstRectTileY * 16 + 16 * rectHeight + worldOffsetY, olc::GREY);
					DrawLine(firstRectTileX * 16 + 16 * rectWidth + worldOffsetX, firstRectTileY * 16 +                   worldOffsetY, firstRectTileX * 16 + 16 * rectWidth + worldOffsetX, firstRectTileY * 16 + 16 * rectHeight + worldOffsetY, olc::GREY);
					DrawLine(firstRectTileX * 16 +                  worldOffsetX, firstRectTileY * 16 + 16 * rectHeight + worldOffsetY, firstRectTileX * 16 + 16 * rectWidth + worldOffsetX, firstRectTileY * 16 + 16 * rectHeight + worldOffsetY, olc::GREY);
				}
				else {
					DrawLine(tileX * 16 +      worldOffsetX, tileY * 16 +      worldOffsetY, tileX * 16 + 16 + worldOffsetX, tileY * 16 +      worldOffsetY, olc::GREY);
					DrawLine(tileX * 16 +      worldOffsetX, tileY * 16 +      worldOffsetY, tileX * 16 +      worldOffsetX, tileY * 16 + 16 + worldOffsetY, olc::GREY);
					DrawLine(tileX * 16 + 16 + worldOffsetX, tileY * 16 +      worldOffsetY, tileX * 16 + 16 + worldOffsetX, tileY * 16 + 16 + worldOffsetY, olc::GREY);
					DrawLine(tileX * 16 +      worldOffsetX, tileY * 16 + 16 + worldOffsetY, tileX * 16 + 16 + worldOffsetX, tileY * 16 + 16 + worldOffsetY, olc::GREY);
				}

				if (GetKey(olc::Key::CTRL).bHeld && GetMouse(0).bPressed) {
					if (pickedFirst) {
						if (pickedSecond) {
							pickedSecond = false;
							startRec = std::make_pair(tileX, tileY);
						} else {
							pickedSecond = true;
							endRex = std::make_pair(tileX, tileY);
						}
					}
					else {
						pickedFirst = true;
						startRec = std::make_pair(tileX, tileY);
					}
				}

				if (pickedFirst && !pickedSecond && !GetKey(olc::Key::CTRL).bHeld) {
					pickedFirst = false;
				}
			}
			else if(pickedFirst && pickedSecond) {
				int firstRectTileX  = startRec.first;
				int firstRectTileY  = startRec.second;
				int secondRectTileX = endRex.first;
				int secondRectTileY = endRex.second;
				int rectWidth  = abs(firstRectTileX - secondRectTileX);
				int rectHeight = abs(firstRectTileY - secondRectTileY);
				rectHeight++;
				rectWidth++;
				DrawLine(firstRectTileX * 16 +                  worldOffsetX, firstRectTileY * 16 +                   worldOffsetY, firstRectTileX * 16 + 16 * rectWidth + worldOffsetX, firstRectTileY * 16 +                   worldOffsetY, olc::GREY);
				DrawLine(firstRectTileX * 16 +                  worldOffsetX, firstRectTileY * 16 +                   worldOffsetY, firstRectTileX * 16 +                  worldOffsetX, firstRectTileY * 16 + 16 * rectHeight + worldOffsetY, olc::GREY);
				DrawLine(firstRectTileX * 16 + 16 * rectWidth + worldOffsetX, firstRectTileY * 16 +                   worldOffsetY, firstRectTileX * 16 + 16 * rectWidth + worldOffsetX, firstRectTileY * 16 + 16 * rectHeight + worldOffsetY, olc::GREY);
				DrawLine(firstRectTileX * 16 +                  worldOffsetX, firstRectTileY * 16 + 16 * rectHeight + worldOffsetY, firstRectTileX * 16 + 16 * rectWidth + worldOffsetX, firstRectTileY * 16 + 16 * rectHeight + worldOffsetY, olc::GREY);
			}

			int iconOffset = 0;
			if (floodMode || GetKey(olc::Key::SHIFT).bHeld) {
				DrawSprite(2, 190, &fillIcon);
				iconOffset += 10;
			}


#pragma region Controls

			// world movement
#ifdef SMOOTH_WORLD_MOVEMENT
			if (GetKey('W').bHeld) {
				moved = true;
				worldOffsetY += 32 * fElapsedTime * (GetKey(VK_SHIFT).bHeld ? 2 : 1);
			}
			if (GetKey('S').bHeld) {
				moved = true;
				worldOffsetY -= 32 * fElapsedTime * (GetKey(VK_SHIFT).bHeld ? 2 : 1);
			}
			if (GetKey('A').bHeld) {
				moved = true;
				worldOffsetX += 32 * fElapsedTime * (GetKey(VK_SHIFT).bHeld ? 2 : 1);
			}
			if (GetKey('D').bHeld) {
				moved = true;
				worldOffsetX -= 32 * fElapsedTime * (GetKey(VK_SHIFT).bHeld ? 2 : 1);
		}
#else
			if (GetKey(olc::Key::W).bPressed || (GetKey(olc::Key::SHIFT).bHeld && GetKey(olc::Key::W).bHeld)) {
				moved = true;
				worldOffsetY += 16;
			}
			if ((!GetKey(olc::Key::K0).bHeld && GetKey(olc::Key::S).bPressed) || (GetKey(olc::Key::SHIFT).bHeld && GetKey(olc::Key::S).bHeld)) {
				moved = true;
				worldOffsetY -= 16;
			}
			if (GetKey(olc::Key::A).bPressed || (GetKey(olc::Key::SHIFT).bHeld && GetKey(olc::Key::A).bHeld)) {
				moved = true;
				worldOffsetX += 16;
			}
			if (GetKey(olc::Key::D).bPressed || (GetKey(olc::Key::SHIFT).bHeld && GetKey(olc::Key::D).bHeld)) {
				moved = true;
				worldOffsetX -= 16;
			}
#endif

			if (GetKey(olc::Key::ESCAPE).bPressed) {
				pickedFirst  = false;
				pickedSecond = false;
			}
			if (GetKey(olc::Key::CTRL).bHeld && GetKey(olc::Key::S).bPressed) {
				if (file.length() == 0) {
					SaveLevel();
				} else {
					level.Save(file);
				}
			}
			if (GetKey(olc::Key::CTRL).bHeld && GetKey(olc::Key::L).bPressed) {
				LoadLevel();
			}
			if (GetKey(olc::Key::T).bPressed) {
				tool = (Tool)((int)tool + 1);
				if (tool == Tool::LAST) {
					tool = Tool::TILES;
				}
			}
			if (GetKey(olc::Key::G).bPressed) {
				grid = !grid;
			}
			if (GetKey(olc::Key::F).bPressed) {
				floodMode = !floodMode;
			}
			if (GetKey(olc::Key::LEFT).bPressed) {
				if (pageCount != 0) {
					page--;
					if (page < 0) {
						page = 0;
					}
				}
			}
			if (GetKey(olc::Key::RIGHT).bPressed) {
				if (pageCount != 0) {
					page++;
					if (page >= pageCount) {
						page = pageCount - 1;
					}
				}
			}
#pragma endregion
		}

		return true;
	}

#pragma region Meta Tool

	enum class MetaTools {
		SOLID_BRUSH
	};

	MetaTools selectedMetaTool;


	void metaTool(int tileX, int tileY) {
		std::string title("TILE META");
		DrawStringFont(uiBase + 5, 5, title);

		std::string solidBrushText("");
		if (selectedMetaTool == MetaTools::SOLID_BRUSH) {
			solidBrushText.append(" * ");
		}
		solidBrushText.append("SOLID");
		DrawStringFont(uiBase + 10, 18, solidBrushText);
		FillRect(uiBase + 7, 19, 5, 5, olc::RED);


		// are we in the world editor
		if (tileX >= 0 && tileY >= 0 && tileX < level.GetWidth() && tileY < level.GetHeight() && !popup.menuActive) {
			switch (selectedMetaTool) {
			case MetaTools::SOLID_BRUSH:
			{
				// change the tile
				if (GetMouse(0).bHeld) {
					if (floodMode || GetKey(olc::Key::SHIFT).bHeld) {
						FloorFillSolid(tileX, tileY, true);
					}
					else {
						level[tileX + tileY * level.GetWidth()].SetSolid(true);
					}
				}
				else if (GetMouse(1).bHeld) {
					if (floodMode || GetKey(olc::Key::SHIFT).bHeld) {
						FloorFillSolid(tileX, tileY, false);
					}
					else {
						level[tileX + tileY * level.GetWidth()].SetSolid(false);
					}
				}
			}
			break;
			}
		}
	}

#pragma endregion

#pragma region Import Export

    void DrawRect( int x1, int y1, int x2, int y2, olc::Pixel colour ) {
        DrawLine( x1, y1, x1, y2, colour );
        DrawLine( x2, y1, x2, y2, colour );
        DrawLine( x1, y1, x2, y1, colour );
        DrawLine( x1, y2, x2, y2, colour );
    }

	void exportAndImportTool() {

// could also be an enum
#define MENU_OPTION_NEW   0
#define MENU_OPTION_IMP   1
#define MENU_OPTION_ILVL  2
#define MENU_OPTION_ISPR  3
#define MENU_OPTION_EXP   4
#define MENU_OPTION_ELVL  5
#define MENU_OPTION_ESPR  6

        // menu definition: options correspond with coordinates below
        std::vector<std::string> vMenuOptions = {
            "NEW",
            "IMPORT:",
            "LEVEL",
            "SPRITESHEET",
            "EXPORT:",
            "LEVEL",
            "SPRITE"
        };
        // screen coords for displaying options (for x relative to uiBase)
        std::vector<int> vMenuX = {  6,  1,  6,  6,  1,  6,  6 };
        std::vector<int> vMenuY = {  5, 25, 35, 45, 75, 85, 95 };

        auto draw_menu_button = [=]( int x, int y, const std::string &s ) {
            DrawStringFont( x, y, s );

            int lt = x, up = y, dn = up + 8, rt = lt + s.length() * 8;
            int mX = GetMouseX(), mY = GetMouseY();
            if (lt <= mX && mX <= rt && up <= mY && mY <= dn) {
                // mouse hovers over menu option
                DrawRect( lt - 1, up - 1, rt + 1, dn + 1, olc::MAGENTA );
            }
        };

		draw_menu_button( uiBase + vMenuX[0], vMenuY[0], vMenuOptions[0] );
		DrawStringFont(   uiBase + vMenuX[1], vMenuY[1], vMenuOptions[1] );
		draw_menu_button( uiBase + vMenuX[2], vMenuY[2], vMenuOptions[2] );
		draw_menu_button( uiBase + vMenuX[3], vMenuY[3], vMenuOptions[3] );
		DrawStringFont(   uiBase + vMenuX[4], vMenuY[4], vMenuOptions[4] );
		draw_menu_button( uiBase + vMenuX[5], vMenuY[5], vMenuOptions[5] );
		draw_menu_button( uiBase + vMenuX[6], vMenuY[6], vMenuOptions[6] );

		auto is_in_focus = [=]( int index ) -> bool {
		    int lt = uiBase + vMenuX[index];
		    int rt = lt + vMenuOptions[index].length() * 8;
		    int up = vMenuY[index];
		    int dn = up + 8;
		    int mx = GetMouseX(), my = GetMouseY();
		    return (lt <= mx && mx <= rt && up <= my && my <= dn);
		};

		if (popup.menuActive) return;
		if (GetMouse(0).bPressed) {
			// new
			if (is_in_focus( MENU_OPTION_NEW )) {
				popup.menuActive = true;
				popup.popup = Popup::NEW_MAP_SIZE;
			}
			// import level
			if (is_in_focus( MENU_OPTION_ILVL )) {
				LoadLevel();
			}
			// import spritesheet
			if (is_in_focus( MENU_OPTION_ISPR )) {
				ImportSpriteSheet();
			}
			// export level
			if (is_in_focus( MENU_OPTION_ELVL )) {
				SaveLevel();
			}
			// export level as sprite
			if (is_in_focus( MENU_OPTION_ESPR )) {
				ExportAsSprite();
			}
		}
	}

	void ImportSpriteSheet() {
		wchar_t filename[MAX_PATH];
		OPENFILENAME ofn;
		ZeroMemory(&filename, sizeof(filename));
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST;
		ofn.lpstrFilter = L"olcSprite (*.png)\0*.png\0Any File\0*.*\0";
		ofn.lpstrTitle = L"Import Sprite Sheet";
		if (GetOpenFileName(&ofn)) {
			spriteSheetFile = ws2s( filename );
			level.LoadSpriteSheet(spriteSheetFile, 16);
			tiles = level.GetSpriteSheet();
		}
	}

	void LoadLevel() {
		wchar_t filename[MAX_PATH];
		OPENFILENAME ofn;
		ZeroMemory(&filename, sizeof(filename));
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST;
		ofn.lpstrFilter = L"Level File (*.lvl)\0*.lvl\0Any File\0*.*\0";
		ofn.lpstrTitle = L"Load Level";
		if (GetOpenFileName(&ofn)) {
            spriteSheetFile = ws2s( filename );
			level.Load(spriteSheetFile);
		}
	}

	void SaveLevel() {
		wchar_t filename[MAX_PATH];
		OPENFILENAME ofn;
		ZeroMemory(&filename, sizeof(filename));
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = L"Level File (*.lvl)\0*.lvl\0Any File\0*.*\0";
		ofn.lpstrTitle = L"Save Level";
		if (GetSaveFileName(&ofn)) {
			std::string f = ws2s( filename );
			if (!ends_with(f, ".lvl")) {
				f.append(".lvl");
			}
			level.Save(f);
			file = f;
		}
	}


	void ExportAsSprite() {
		wchar_t filename[MAX_PATH];
		OPENFILENAME ofn;
		ZeroMemory(&filename, sizeof(filename));
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = L"olcSprite (*.png)\0*.png\0Any File\0*.*\0";
		ofn.lpstrTitle = L"Export Level As Sprite";
		ofn.lpstrDefExt = L"png";
		if (GetSaveFileName(&ofn)) {
			std::string f = ws2s( filename );
			if (!ends_with(f, ".png")) {
				f.append(".png");
			}
			olc::Sprite exportedSprite(level.GetWidth() * TILE_WIDTH, level.GetHeight() * TILE_WIDTH);
			for (int y = 0; y < level.GetHeight(); y++) {
				for (int x = 0; x < level.GetWidth(); x++) {
					olc::Sprite* sprite = level[x + y * level.GetWidth()].GetSprite();
					for (int sy = 0; sy < sprite->height; sy++) {
						for (int sx = 0; sx < sprite->width; sx++) {
							exportedSprite.SetPixel(x * TILE_WIDTH + sx, y * TILE_WIDTH + sy, sprite->GetPixel(sx, sy));
						}
					}
				}
			}
			exportedSprite.SaveToFile(f);
		}
	}

#pragma endregion

#pragma region Tiles Tool


	void tilesTool(int tileX, int tileY) {

		// draw page
		std::string pageText("TILES:");
		if (pageCount == 0) {
			pageText.append(std::to_string(tiles->GetTileCount()));
		}
		else {
			pageText.append(std::to_string(page + 1));
			pageText.append("/");
			pageText.append(std::to_string(pageCount));
		}
		DrawStringFont(uiBase + 5, 5, pageText);

		// draw sprites in menu
		int drawn = 0;
		int toDraw = std::min(tilesPerPage, tiles->GetTileCount() - tilesPerPage * page);
		for (int row = 0; row < tilesPerColumn; row++) {
			if (drawn >= toDraw) break;
			int y = 23 + row * tiles->GetTileHeight();
			for (int col = 0; col < tilesPerRow; col++) {
				if (drawn >= toDraw) break;
				int x = uiBase + col * tiles->GetTileWidth();
				DrawSprite(x, y, tiles[0][(col + row * tilesPerRow) + tilesPerPage * page]);
				drawn++;
			}
		}

		// draw selected sprite thing
		if (selectedSprite >= tilesPerPage * page && selectedSprite < tilesPerPage * page + tilesPerPage) {
			int col = selectedSprite % tilesPerRow;
			int row = (selectedSprite - col) / tilesPerRow;
			row = row - page * tilesPerColumn;
			int y = 23 + row * tiles->GetTileHeight();
			int x = uiBase + col * tiles->GetTileWidth();
			DrawLine(x, y, x + 16, y, olc::RED);
			DrawLine(x, y, x, y + 16, olc::RED);
			DrawLine(x + 16, y, x + 16, y + 16, olc::RED);
			DrawLine(x, y + 16, x + 16, y + 16, olc::RED);
		}

		// are we in the selection menu
		if (GetMouseX() >= uiBase && GetMouseX() < uiBase + tilesPerRow * tiles->GetTileWidth() && GetMouseY() > 28 && !popup.menuActive) {
			int menuX = GetMouseX() - uiBase;
			int menuY = GetMouseY() - 28;
			int col = menuX / tiles->GetTileWidth();
			int row = menuY / tiles->GetTileHeight();
			int index = (col + row * tilesPerRow) + page * tilesPerPage;
			int y = 23 + row * tiles->GetTileHeight();
			int x = uiBase + col * tiles->GetTileWidth();
			DrawLine(x, y, x + 16, y, olc::DARK_RED);
			DrawLine(x, y, x, y + 16, olc::DARK_RED);
			DrawLine(x + 16, y, x + 16, y + 16, olc::DARK_RED);
			DrawLine(x, y + 16, x + 16, y + 16, olc::DARK_RED);
			if (GetMouse(0).bPressed) {
				selectedSprite = index;
			}
		}

		// are we in the world editor
		if (tileX >= 0 && tileY >= 0 && tileX < level.GetWidth() && tileY < level.GetHeight() && !popup.menuActive && GetMouseX() <= 300) {
			// do we have a selection rect, if so are we in it?
			if ((!pickedFirst && !pickedSecond) || (pickedFirst && pickedSecond && tileX >= startRec.first && tileX < startRec.first + endRex.first - 1 && tileY >= startRec.second && tileY < startRec.second + endRex.second)) {
				// change the tile
				if (GetMouse(0).bHeld) {
					if (floodMode || GetKey(olc::Key::SHIFT).bHeld) {
						FloodFillTile(tileX, tileY);
					}
					else {
						level[tileX + tileY * level.GetWidth()].SetSpriteId(selectedSprite);
					}
				}
				else if (GetMouse(1).bPressed) {
					selectedSprite = level[tileX + tileY * level.GetWidth()].GetSpriteId();
				}
			}
			if (GetKey(olc::Key::BACK).bPressed && pickedFirst && pickedSecond) {
				for (int y = startRec.second; y < endRex.second + startRec.second; y++) {
					for (int x = startRec.first; x < endRex.first + startRec.first - 1; x++) {
						int index = x + y * level.GetWidth();
						level[index].SetSpriteId(selectedSprite);
					}
				}
			}
		}

	}


#pragma endregion

#pragma region Utils

	inline bool ends_with(std::string const & value, std::string const & ending)
	{
		if (ending.size() > value.size()) return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}

	int fillTileOfType = DEFAULT_TILE;
	bool solidStart = false;

	void FloorFillSolid(int x, int y, bool fill) {
		fillTileOfType = level[x + y * level.GetWidth()].GetSpriteId();
		solidStart = fill;
		std::queue<std::pair<int, int>> q;
		q.push(std::pair<int, int>(x, y));
		while (q.size() != 0) {
			std::pair<int, int> xy = q.front();
			q.pop();
			level[xy.first + xy.second * level.GetWidth()].SetSolid(fill);
			if (ShouldFillSolid(xy.first + 1, xy.second    )) q.push(std::pair<int, int>(xy.first + 1, xy.second    ));
			if (ShouldFillSolid(xy.first - 1, xy.second    )) q.push(std::pair<int, int>(xy.first - 1, xy.second    ));
			if (ShouldFillSolid(xy.first    , xy.second + 1)) q.push(std::pair<int, int>(xy.first    , xy.second + 1));
			if (ShouldFillSolid(xy.first    , xy.second - 1)) q.push(std::pair<int, int>(xy.first    , xy.second - 1));
		}
	}

	bool ShouldFillSolid(int x, int y) {
		if (x < 0 || y < 0 || x >= level.GetWidth() || y >= level.GetHeight()) return false;
		return level[x + y * level.GetWidth()].GetSpriteId() == fillTileOfType && level[x + y * level.GetWidth()].IsSolid() != solidStart;
	}

	void FloodFillTile(int x, int y) {
		fillTileOfType = level[x + y * level.GetWidth()].GetSpriteId();
		if (fillTileOfType == selectedSprite) return;
		std::queue<std::pair<int, int>> q;
		q.push(std::pair<int, int>(x, y));
		while (q.size() != 0) {
			std::pair<int, int> xy = q.front();
			q.pop();
			level[xy.first + xy.second * level.GetWidth()].SetSpriteId(selectedSprite);
			if (ShouldFillTile(xy.first + 1, xy.second)) q.push(std::pair<int, int>(xy.first + 1, xy.second));
			if (ShouldFillTile(xy.first - 1, xy.second)) q.push(std::pair<int, int>(xy.first - 1, xy.second));
			if (ShouldFillTile(xy.first, xy.second + 1)) q.push(std::pair<int, int>(xy.first, xy.second + 1));
			if (ShouldFillTile(xy.first, xy.second - 1)) q.push(std::pair<int, int>(xy.first, xy.second - 1));
		}
	}

	bool ShouldFillTile(int x, int y) {
		if (x < 0 || y < 0 || x >= level.GetWidth() || y >= level.GetHeight()) return false;
		return level[x + y * level.GetWidth()].GetSpriteId() == fillTileOfType;
	}

#pragma endregion

};

int main() {
	olcLevelMaker levelMaker;
	if (levelMaker.Construct(800, 400, 2, 2)) {
		levelMaker.Start();
	}
}
