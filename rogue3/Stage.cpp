#include "stdafx.h"
#include "Stage.h"
#include "Actor.h"
#include "Map.h"
#include "Engine.h"
#include "StairsAI.h"

Stage::Stage()
{
	map = NULL;
	actors.clear();
}

Stage::~Stage()
{
	terminate();
}

void Stage::initialize(int sourceX, int sourceY)
{
	if (actors.size() > 0)
	{
		actors.clearAndDelete();
	}

	map = new Map(MAP_WIDTH, MAP_HEIGHT);
	map->initialize(sourceX, sourceY);
}

void Stage::load(TCODZip &zip)
{
	// Load the map.
	int width = zip.getInt();
	int height = zip.getInt();
	map = new Map(width, height);
	map->load(zip);

	// Then all other actors.
	int numActors = zip.getInt();
	while (numActors > 0)
	{
		Actor *actor = new Actor(0, 0, 0, NULL, TCODColor::white);
		actor->load(zip);
		if (!actor->blocks)
		{
			actors.insertBefore(actor, 0);
		}
		else
		{
			actors.push(actor);
		}
		numActors--;
	}
}

void Stage::save(TCODZip &zip)
{
	// Save the map first.
	zip.putInt(map->width);
	zip.putInt(map->height);
	map->save(zip);

	// Then all the actors.
	zip.putInt(actors.size());
	for (Actor **iter = actors.begin(); iter != actors.end(); iter++)
	{
		(*iter)->save(zip);
	}
}

void Stage::terminate()
{
	actors.clearAndDelete();
	if (map)
	{
		delete map;
		map = NULL;
	}
}

void Stage::update()
{
	for (Actor **iterator = actors.begin(); iterator != actors.end(); iterator++)
	{
		Actor *actor = *iterator;
		if (actor != engine.player) // Not really happy with having an engine reference here, but it will work.
		{
			actor->update();
		}
	}
}