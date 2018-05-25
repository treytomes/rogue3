#pragma once

class Actor;
class Map;

// A stage consists of both the map, and the actors playing on the map.
class Stage : public Persistent
{
public:
	Stage();
	~Stage();

	// sourceX and sourceY are the location of the stairs on the previous level, if there was a previous level.
	void initialize(int sourceX = -1, int sourceY = -1);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
	void terminate();

	void update();

	TCODList<Actor *> actors;
	//Actor *stairsUp;
	//Actor *stairsDown;
	Map *map;
};

