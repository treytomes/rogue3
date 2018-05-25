#pragma once

#include "Destructible.h"

class PlayerDestructible : public Destructible
{
public:
	PlayerDestructible(int maxHealth, int defense, const char *corpseName);
	void save(TCODZip &zip);
	void die(Actor *owner);
};

