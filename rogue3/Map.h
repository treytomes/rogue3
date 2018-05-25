#pragma once

#include "Tile.h"

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;
static const int ROOM_MAX_ITEMS = 2;
static const int ROOM_MAX_MONSTERS = 3;

class Map : public Persistent
{
public:
	int width, height;

	Map(int width, int height);
	~Map();

	// sourceX and sourceY are the location of the stairs on the previous level, if there was a previous level.
	void initialize(int sourceX = -1, int sourceY = -1);

	void load(TCODZip &zip);
	void save(TCODZip &zip);

	bool canWalk(int x, int y) const;
	bool isWall(int x, int y) const;
	bool isInFov(int x, int y) const;
	bool isExplored(int x, int y) const;
	void computeFov();
	void render(int centerX, int centerY, int renderX, int renderY, int renderWidth, int renderHeight) const;
	Tile *getTile(int x, int y) const;

	void addMonster(int x, int y);
	void addNPC(int x, int y);
	void addItem(int x, int y);

protected:
	Tile *tiles;
	TCODMap *map;
	friend class BspListener;

	void dig(int x1, int y1, int x2, int y2);
	void createRoom(int x1, int y1, int x2, int y2);
	
	//void setWall(int x, int y);
};
