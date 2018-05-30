#include "stdafx.h"
#include "MonsterDestructible.h"

#include "Actor.h"
#include "Engine.h"

MonsterDestructible::MonsterDestructible(int maxHealth, int defense, const char *corpseName, int xp)
	: Destructible(maxHealth, defense, corpseName, xp)
{
}

void MonsterDestructible::save(TCODZip &zip)
{
	zip.putInt(MONSTER);
	Destructible::save(zip);
}

void MonsterDestructible::die(Actor *owner)
{
	engine.ui->message(TCODColor::lightGrey, "%s is dead.  You gain %d xp.", owner->name, xp);
	engine.player->destructible->xp += xp;

	// Transform it into a nasty corpse!  It doesn't block, can't be attacked and doesn't move.
	Destructible::die(owner);
}
