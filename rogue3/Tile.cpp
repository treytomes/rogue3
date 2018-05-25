#include "stdafx.h"
#include "Tile.h"

Tile::Tile()
	: tileIndex('#'), foregroundColor(TCODColor::lightGrey), backgroundColor(TCODColor::lightGrey * 0.5f), explored(false)
{
	strcpy(name, "wall");
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
