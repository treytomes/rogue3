#pragma once

#include "Pickable.h"

class Actor;

class LightningBolt : public Pickable
{
public:
	float range;
	int damage;

	LightningBolt(float range, int damage);
	bool use(Actor *owner, Actor *wearer);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};

