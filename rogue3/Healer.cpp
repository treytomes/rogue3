#include "stdafx.h"
#include "Healer.h"
#include "Actor.h"
#include "Engine.h"

Healer::Healer(int amount)
	: Pickable(), amount(amount)
{
}

void Healer::load(TCODZip &zip)
{
	amount = zip.getFloat();
}

void Healer::save(TCODZip &zip)
{
	zip.putInt(HEALER);
	zip.putFloat(amount);
}

bool Healer::use(Actor *owner, Actor *wearer)
{
	if (wearer->destructible)
	{
		int amountHealed = wearer->destructible->heal(amount);
		if (amountHealed > 0)
		{
			engine.particles->spawnNumberParticles(wearer->x, wearer->y, amount);
			return Pickable::use(owner, wearer);
		}
	}
	return false;
}