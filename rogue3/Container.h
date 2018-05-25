#pragma once

class Actor;

class Container : Persistent
{
public:
	int size; // maximum number of actors. 0=unlimited
	TCODList<Actor *> inventory;

	Container(int size);
	~Container();
	void load(TCODZip &zip);
	void save(TCODZip &zip);

	bool add(Actor *actor);
	void remove(Actor *actor);
};

