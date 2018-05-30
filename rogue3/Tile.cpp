#include "stdafx.h"
#include "Tile.h"


Tile::Tile()
	: tileIndex('#'), explored(false)
	//, foregroundColor(TCODColor::lightGrey), backgroundColor(TCODColor::lightGrey * 0.5f)
{
	strcpy(name, "wall");
	setColor(TCODColor::lightGrey);
}

void Tile::load(TCODZip &zip)
{
	tileIndex = zip.getInt();
	foregroundColor = zip.getColor();
	backgroundColor = zip.getColor();
	explored = zip.getInt();

	const char *name = zip.getString();
	strcpy(this->name, name);
}

void Tile::save(TCODZip &zip)
{
	zip.putInt(tileIndex);
	zip.putColor(&foregroundColor);
	zip.putColor(&backgroundColor);
	zip.putInt(explored);
	zip.putString(name);
}

void Tile::setColor(TCODColor color, float deviation)
{
	TCODRandom *rng = TCODRandom::getInstance();

	int r = (int)(TCODColor::lightGrey.r * rng->getFloat(1.0f - deviation, 1.0f + deviation));
	int g = (int)(TCODColor::lightGrey.g * rng->getFloat(1.0f - deviation, 1.0f + deviation));
	int b = (int)(TCODColor::lightGrey.b * rng->getFloat(1.0f - deviation, 1.0f + deviation));

	foregroundColor = TCODColor(r, g, b);
	backgroundColor = foregroundColor * 0.3f;
}