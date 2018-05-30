#include "stdafx.h"
#include "Destructible.h"

#include "Actor.h"
#include "Engine.h"
#include "RayParticle.h"
#include "MonsterDestructible.h"
#include "PlayerDestructible.h"

Destructible::Destructible(int maxHealth, int defense, const char *corpseName, int xp)
	: maxHealth(maxHealth), health(maxHealth), defense(defense), xp(xp)
{
	if (corpseName != NULL)
	{
		if (strlen(corpseName) > MAX_NAME_LENGTH)
		{
			engine.ui->message(TCODColor::yellow, "This name is too long: %s", corpseName);
			memcpy(this->corpseName, corpseName, MAX_NAME_LENGTH);
		}
		else
		{
			strcpy(this->corpseName, corpseName);
		}
	}
}

void Destructible::load(TCODZip &zip)
{
	maxHealth = zip.getFloat();
	health = zip.getFloat();
	defense = zip.getFloat();
	strcpy(corpseName, zip.getString());
	xp = zip.getInt();
}

void Destructible::save(TCODZip &zip)
{
	zip.putFloat(maxHealth);
	zip.putFloat(health);
	zip.putFloat(defense);
	zip.putString(corpseName);
	zip.putInt(xp);
}

Destructible *Destructible::create(TCODZip &zip) {
	DestructibleType type = (DestructibleType)zip.getInt();
	Destructible *destructible = NULL;
	switch (type) {
	case MONSTER:
		destructible = new MonsterDestructible(0, 0, NULL, 0);
		break;
	case PLAYER:
		destructible = new PlayerDestructible(0, 0, NULL);
		break;
	}
	destructible->load(zip);
	return destructible;
}

int Destructible::takeDamage(Actor *owner, int damage)
{
	damage -= defense;
	if (damage > 0)
	{
		health -= damage;

		engine.particles->spawnNumberParticles(owner->x, owner->y, -damage);

		if (health <= 0)
		{
			die(owner);
		}
	}
	else
	{
		damage = 0;
	}
	return damage;
}

int Destructible::heal(int amount)
{
	health += amount;
	if (health > maxHealth)
	{
		amount -= health - maxHealth;
		health = maxHealth;
	}
	return amount;
}

void Destructible::die(Actor *owner)
{
	// Transform the actor in a corpse!
	owner->tileIndex = '%';
	owner->foregroundColor = TCODColor::darkRed;
	strcpy(owner->name, corpseName);
	owner->blocks = false;

	// Do dead actors need AI?  Might be more efficient to delete it.  Are resurrecting monsters a thing?
	if (owner->ai)
	{
		delete owner->ai;
		owner->ai = NULL;
	}

	// Make sure corpses are drawn before living actors.
	engine.sendToBack(owner);

	engine.particles->spawnDeathParticles(owner->x, owner->y);
}