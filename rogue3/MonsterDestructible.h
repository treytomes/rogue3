#pragma once

#include "Destructible.h"

class MonsterDestructible : public Destructible
{
public:
	MonsterDestructible(int maxHealth, int defense, const char *corpseName);
	void save(TCODZip &zip);
	void die(Actor *owner);
};

