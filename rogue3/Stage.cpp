#include "stdafx.h"
#include "Stage.h"
#include "Actor.h"
#include "Map.h"
#include "Engine.h"
#include "StairsAI.h"
#include "Scent.h"

Stage::Stage()
{
	map = NULL;
	actors.clear();
	scents.clear();
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

	if (scents.size() > 0)
	{
		scents.clearAndDelete();
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

	// Then all actors.
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

	// Then all scents.
	int numScents = zip.getInt();
	while (numScents > 0)
	{
		Scent *scent = new Scent(0, 0, 0, 0.0f);
		scent->load(zip);
		scents.push(scent);
		numScents--;
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

	// Then all the scents.
	zip.putInt(scents.size());
	for (Scent **iter = scents.begin(); iter != scents.end(); iter++)
	{
		(*iter)->save(zip);
	}
}

void Stage::terminate()
{
	actors.clearAndDelete();
	scents.clearAndDelete();
	if (map)
	{
		delete map;
		map = NULL;
	}
}

void Stage::update()
{
	for (Actor **iter = actors.begin(); iter != actors.end(); iter++)
	{
		Actor *actor = *iter;
		if (actor != engine.player) // Not really happy with having an engine reference here, but it will work.
		{
			actor->update();
		}
	}

	TCODList<Scent *> deadScents;
	for (Scent **iter = scents.begin(); iter != scents.end(); iter++)
	{
		Scent *scent = *iter;
		scent->update();
		if (!scent->isAlive())
		{
			deadScents.push(scent);
		}
	}

	for (Scent **iter = deadScents.begin(); iter != deadScents.end(); iter++)
	{
		Scent *scent = *iter;
		scents.remove(scent);
	}

	deadScents.clearAndDelete();
}