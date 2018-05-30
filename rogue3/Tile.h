#pragma once

#include "Constants.h"

static const float DEFAULT_COLOR_DEVIATION = 0.1f;

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

	// Calculate a random-ish background and foreground color based on the deviation value.
	void setColor(TCODColor color, float deviation = DEFAULT_COLOR_DEVIATION);
};
