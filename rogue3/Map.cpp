#include "stdafx.h"

#include "Map.h"

#include "ColorUtil.h"
#include "Actor.h"
#include "Engine.h"
#include "MonsterDestructible.h"
#include "MonsterAI.h"
#include "Healer.h"
#include "LightningBolt.h"
#include "Fireball.h"
#include "Confuser.h"
#include "LuaAI.h"
#include "StairsAI.h"

class BspListener : public ITCODBspCallback
{
private:
	// A map to dig.
	Map & map;

	// Room number.
	int roomNum;

	// Center of the last room.
	int lastX, lastY;

	Actor *stairsUp;
	Actor *stairsDown;

public:
	BspListener(Map &map, int sourceX, int sourceY) : map(map), roomNum(0)
	{
		// I don't need to delete these in BspListener because they are added to (and delete from) Stage.

		stairsUp = new Actor(0, 0, '<', "stairs up", TCODColor::white);
		stairsUp->blocksMovement = false;
		stairsUp->fovOnly = false;
		stairsUp->ai = new StairsAI(false);
		engine.getCurrentStage()->actors.insertBefore(stairsUp, 0);

		stairsDown = new Actor(0, 0, '>', "stairs down", TCODColor::white);
		stairsDown->blocksMovement = false;
		stairsDown->fovOnly = false;
		stairsDown->ai = new StairsAI(true);
		engine.getCurrentStage()->actors.insertBefore(stairsDown, 0);

		if ((sourceX >= 0) && (sourceY >= 0) && (sourceX < map.width) && (sourceY < map.height))
		{
			// Stairs up are in the same position as the previous stage's stairs down.
			createRoom(sourceX - ROOM_MAX_SIZE / 2, sourceY - ROOM_MAX_SIZE / 2, ROOM_MAX_SIZE, ROOM_MAX_SIZE);
			stairsUp->moveTo(sourceX, sourceY);
		}
	}

	bool visitNode(TCODBsp *node, void *userData)
	{
		if (node->isLeaf())
		{
			createRoom(node->x, node->y, node->w, node->h);
		}
		return true;
	}

	void createRoom(int minX, int minY, int maxWidth, int maxHeight)
	{
		int x, y, w, h;
		// Dig a room.
		TCODRandom *rng = TCODRandom::getInstance();
		w = rng->getInt(ROOM_MIN_SIZE, maxWidth - 2);
		h = rng->getInt(ROOM_MIN_SIZE, maxHeight - 2);
		x = rng->getInt(minX + 1, minX + maxWidth - w - 1);
		y = rng->getInt(minY + 1, minY + maxHeight - h - 1);

		if (x < 1)
		{
			x = 1;
		}
		if (y < 1)
		{
			y = 1;
		}
		if (x + w >= map.width - 1)
		{
			w = map.width - 1 - x;
		}
		if (y + h >= map.height - 1)
		{
			h = map.height - 1 - y;
		}

		map.createRoom(x, y, x + w - 1, y + h - 1);

		if (roomNum != 0)
		{
			// dig a corridor from last room
			map.dig(lastX, lastY, x + w / 2, lastY);
			map.dig(x + w / 2, lastY, x + w / 2, y + h / 2);

			// Put the stairs down in the last room.  Setting this on every room like this will cause it to move to the last room.
			stairsDown->moveTo(x + w / 2, y + h / 2);
		}
		else
		{
			// Put the stairs up (and the player) in the first room.
			engine.player->moveTo(x + w / 2, y + h / 2);
			stairsUp->moveTo(x + w / 2, y + h / 2);
		}

		lastX = x + w / 2;
		lastY = y + h / 2;

		roomNum++;
	}
};

//class Viewable

Map::Map(int width, int height)
	: width(width), height(height)
{
	tiles = new Tile[width * height];
	map = new TCODMap(width, height);
}

Map::~Map()
{
	if (tiles)
	{
		delete[] tiles;
		tiles = NULL;
	}

	if (map)
	{
		delete map;
		map = NULL;
	}
}

void Map::initialize(int sourceX, int sourceY)
{
	TCODBsp bsp(0, 0, width, height);
	bsp.splitRecursive(NULL, 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
	BspListener listener(*this, sourceX, sourceY);
	bsp.traverseInvertedLevelOrder(&listener, NULL);
}

void Map::load(TCODZip &zip)
{
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			bool isTransparent = zip.getInt();
			bool isWalkable = zip.getInt();
			map->setProperties(x, y, isTransparent, isWalkable);
			tiles[x + y * width].load(zip);
		}
	}
}

void Map::save(TCODZip &zip)
{
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			zip.putInt(map->isTransparent(x, y));
			zip.putInt(map->isWalkable(x, y));
			tiles[x + y * width].save(zip);
		}
	}
}

static const TCODColor multipleItemsBackground = TCODColor::darkerBlue;

void Map::render(int centerX, int centerY, int renderTop, int renderLeft, int renderWidth, int renderHeight) const
{
	//static const TCODColor colorForegroundWall = TCODColor::lightGrey;
	//static const TCODColor colorForegroundGround = TCODColor::darkGrey;

	for (int renderX = renderLeft, mapX = centerX - renderWidth / 2; (renderX < renderLeft + renderWidth - 1) && (mapX < width); renderX++, mapX++)
	{
		if (mapX < 0)
		{
			continue;
		}

		for (int renderY = renderTop, mapY = centerY - renderHeight / 2; (renderY < renderTop + renderHeight - 1) && (mapY < height); renderY++, mapY++)
		{
			if (mapY < 0)
			{
				continue;
			}

			bool wall = isWall(mapX, mapY);

			int tilesOffset = mapX + mapY * width;
			if (isInFov(mapX, mapY))
			{
				TCODConsole::root->setCharForeground(renderX, renderY, tiles[tilesOffset].foregroundColor);

				if (engine.countItems(mapX, mapY) > 1)
				{
					TCODConsole::root->setCharBackground(renderX, renderY, multipleItemsBackground);
				}
				else
				{
					TCODConsole::root->setCharBackground(renderX, renderY, tiles[tilesOffset].backgroundColor);
				}

				TCODConsole::root->setChar(renderX, renderY, tiles[tilesOffset].tileIndex);
			}
			else if (isExplored(mapX, mapY))
			{
				TCODConsole::root->setCharForeground(renderX, renderY, tiles[tilesOffset].foregroundColor * 0.5f);
				TCODConsole::root->setCharBackground(renderX, renderY, tiles[tilesOffset].backgroundColor * 0.5f);
				TCODConsole::root->setChar(renderX, renderY, tiles[tilesOffset].tileIndex);
			}
		}
	}
}

bool Map::canWalk(int x, int y) const
{
	if (isWall(x, y))
	{
		return false;
	}

	Stage *currentStage = engine.getCurrentStage();
	for (Actor **iterator = currentStage->actors.begin(); iterator != currentStage->actors.end(); iterator++)
	{
		Actor *actor = *iterator;
		if ((actor->getX() == x) && (actor->getY() == y) && actor->blocksMovement)
		{
			// There is an actor here.  Cannot walk.
			return false;
		}
	}

	return true;
}

bool Map::isWall(int x, int y) const
{
	//return !tiles[x + y * width].canWalk;
	return !map->isWalkable(x, y);
}

bool Map::isInFov(int x, int y) const
{
	if ((x < 0) || (x >= width) || (y < 0) || (y >= height))
	{
		return false;
	}
	
	if (map->isInFov(x, y))
	{
		tiles[x + y * width].explored = true;
		return true;
	}
	return false;
}

bool Map::isExplored(int x, int y) const
{
	return tiles[x + y * width].explored;
}

void Map::setBlocksVision(int x, int y, int value)
{
	map->setProperties(x, y, !value, map->isWalkable(x, y));
}

void Map::computeFov()
{
	map->computeFov(engine.player->getX(), engine.player->getY(), engine.fovRadius);
}

void Map::dig(int x1, int y1, int x2, int y2)
{
	if (x2 < x1)
	{
		int tmp = x2;
		x2 = x1;
		x1 = tmp;
	}
	if (y2 < y1)
	{
		int tmp = y2;
		y2 = y1;
		y1 = tmp;
	}
	for (int tileX = x1; tileX <= x2; tileX++)
	{
		for (int tileY = y1; tileY <= y2; tileY++)
		{
			map->setProperties(tileX, tileY, true, true);
			int tilesOffset = tileX + tileY * width;

			strcpy(tiles[tilesOffset].name, "floor");
			tiles[tilesOffset].tileIndex = '.';
			tiles[tilesOffset].setColor(TCODColor::darkGrey);
		}
	}
}

void Map::createRoom(int x1, int y1, int x2, int y2)
{
	dig(x1, y1, x2, y2);

	TCODRandom *rng = TCODRandom::getInstance();
	if (rng->getInt(0, 3) == 0)
	{
		// 25% chance of creating a dumb NPC.
		int x = rng->getInt(x1, x2);
		int y = rng->getInt(y1, y2);
		addNPC(x, y);
	}

	int nbMonsters = rng->getInt(0, ROOM_MAX_MONSTERS);
	while (nbMonsters > 0)
	{
		int x = rng->getInt(x1, x2);
		int y = rng->getInt(y1, y2);
		if (canWalk(x, y))
		{
			addMonster(x, y);
		}
		nbMonsters--;
	}

	int nbItems = rng->getInt(0, ROOM_MAX_ITEMS);
	while (nbItems > 0)
	{
		int x = rng->getInt(x1, x2);
		int y = rng->getInt(y1, y2);
		if (canWalk(x, y))
		{
			addItem(x, y);
		}
		nbItems--;
	}
}

void Map::addMonster(int x, int y)
{
	TCODRandom *rng = TCODRandom::getInstance();
	Actor *actor;
	if (rng->getInt(0, 100) < 80)
	{
		// Create an orc (80%).
		actor = new Actor(x, y, 'o', "orc", TCODColor::desaturatedGreen);
		actor->speed = 4;
		actor->attacker = new Attacker(3);
		actor->destructible = new MonsterDestructible(10, 0, "dead orc", 35);
	}
	else
	{
		// Create a troll (20%).
		actor = new Actor(x, y, 'T', "troll", TCODColor::darkerGreen);
		actor->blocksVision = true; // trolls are big
		actor->speed = 2;
		actor->attacker = new Attacker(8);
		actor->destructible = new MonsterDestructible(16, 1, "troll carcass", 100);
	}
	actor->ai = new MonsterAI();
	//LuaAI *ai = new LuaAI();
	//ai->load_script("monster_ai.lua");
	//actor->ai = ai;

	engine.getCurrentStage()->actors.push(actor);
}

void Map::addNPC(int x, int y)
{
	TCODRandom *rng = TCODRandom::getInstance();
	Actor *actor = new Actor(x, y, '@', "npc", TCODColor::yellow);
	actor->speed = 10;
	//actor->attacker = new Attacker(3);
	//actor->destructible = new MonsterDestructible(10, 0, "dead npc");
	engine.getCurrentStage()->actors.push(actor);
}

void Map::addItem(int x, int y)
{
	TCODRandom *rng = TCODRandom::getInstance();
	int dice = rng->getInt(0, 100);
	if (dice < 70)
	{
		// Create a health potion.
		Actor *healthPotion = new Actor(x, y, '!', "health potion", TCODColor::violet);
		healthPotion->blocksMovement = false;
		healthPotion->pickable = new Healer(4);
		engine.getCurrentStage()->actors.insertBefore(healthPotion, 0); // it's not blocking, so draw it beneath other actors
	}
	else if (dice < 70 + 10)
	{
		// Create a scroll of lightning bolt.
		Actor *scrollOfLightningBolt = new Actor(x, y, '#', "scroll of lightning bolt", TCODColor::lightYellow);
		scrollOfLightningBolt->blocksMovement = false;
		scrollOfLightningBolt->pickable = new LightningBolt(5, 20);
		engine.getCurrentStage()->actors.insertBefore(scrollOfLightningBolt, 0);
	}
	else if (dice < 70 + 10 + 10)
	{
		// Create a scroll of fireball.
		Actor *scrollOfFireball = new Actor(x, y, '#', "scroll of fireball", TCODColor::lightYellow);
		scrollOfFireball->blocksMovement = false;
		scrollOfFireball->pickable = new Fireball(3, 12);
		engine.getCurrentStage()->actors.insertBefore(scrollOfFireball, 0);
	}
	else
	{
		// Create a scroll of confusion.
		Actor *scrollOfConfusion = new Actor(x, y, '#', "scroll of confusion", TCODColor::lightYellow);
		scrollOfConfusion->blocksMovement = false;
		scrollOfConfusion->pickable = new Confuser(10, 8);
		engine.getCurrentStage()->actors.insertBefore(scrollOfConfusion, 0);
	}
}

Tile *Map::getTile(int x, int y) const
{
	if ((x < 0) || (x >= width) || (y < 0) || (y >= height))
	{
		return NULL;
	}
	return &tiles[x + y * width];
}


TCODPath *Map::calculatePath(int startX, int startY, int endX, int endY, TCODPath *path)
{
	if (!path)
	{
		path = new TCODPath(map);
	}
	path->compute(startX, startY, endX, endY);
	return path;
}