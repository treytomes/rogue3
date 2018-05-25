#include "stdafx.h"
#include "MonsterDestructible.h"

#include "Actor.h"
#include "Engine.h"

MonsterDestructible::MonsterDestructible(int maxHealth, int defense, const char *corpseName)
	: Destructible(maxHealth, defense, corpseName)
{
}

void MonsterDestructible::save(TCODZip &zip)
{
	zip.putInt(MONSTER);
	Destructible::save(zip);
}

void MonsterDestructible::die(Actor *owner)
{
	// Transform it into a nasty corpse!  It doesn't block, can't be attacked and doesn't move.
	engine.ui->message(TCODColor::lightGrey, "%s is dead.\n", owner->name);
	Destructible::die(owner);
}
