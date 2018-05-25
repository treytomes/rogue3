#pragma once

#include "LightningBolt.h"

class Fireball : public LightningBolt
{
public:
	Fireball(float range, int damage);
	bool use(Actor *owner, Actor *wearer);
	void save(TCODZip &zip);
};

