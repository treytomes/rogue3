#pragma once

#include "Constants.h"

class Tile : public Persistent
{
public:
	// Has the player already seen this tile?
	bool explored;

	char name[MAX_NAME_LENGTH];
	int tileIndex;
	TCODColor foregroundColor, backgroundColor;

	Tile();
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};
