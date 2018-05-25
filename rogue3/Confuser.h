#pragma once

#include "Pickable.h"

class Actor;

class Confuser : public Pickable
{
public:
	int numTurns;
	float range;

	Confuser(int numTurns, float range);
	bool use(Actor *owner, Actor *wearer);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};

