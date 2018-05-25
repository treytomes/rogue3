#include "stdafx.h"
#include "Attacker.h"
#include "Actor.h"
#include "Engine.h"

Attacker::Attacker(int power)
	: power(power)
{
}

void Attacker::load(TCODZip &zip)
{
	power = zip.getInt();
}

void Attacker::save(TCODZip &zip)
{
	zip.putInt(power);
}

bool Attacker::can_attack(Actor *owner, int targetX, int targetY)
{
	if (owner->getDistance(targetX, targetY) < 2)
	{
		Actor *target = engine.getActor(targetX, targetY);
		if (target->destructible && !target->destructible->isDead())
		{
			return true;
		}
	}
	return false;
}

void Attacker::attack(Actor *owner, int targetX, int targetY)
{
	Actor *target = engine.getDestructibleActor(targetX, targetY);
	if (target == NULL)
	{
		engine.ui->message(TCODColor::lightGrey, "%s tries to attack, but couldn't find anything to hit.");
	}

	if (target->destructible && !target->destructible->isDead())
	{
		if (power - target->destructible->defense > 0)
		{
			engine.ui->message(owner == engine.player ? TCODColor::red : TCODColor::lightGrey, "%s attacks %s for %d hit points.", owner->name, target->name, power - target->destructible->defense);
		}
		else
		{
			engine.ui->message(TCODColor::lightGrey, "%s attacks %s, but it has no effect!", owner->name, target->name);
		}
		target->destructible->takeDamage(target, power);
	}
	else
	{
		engine.ui->message(TCODColor::lightGrey, "%s attacks %s in vain.", owner->name, target->name);
	}
}