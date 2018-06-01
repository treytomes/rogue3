#include "stdafx.h"
#include "LightningBolt.h"
#include "Engine.h"
#include "RayParticle.h"
#include <math.h>

LightningBolt::LightningBolt(float range, int damage)
	: Pickable(), range(range), damage(damage)
{
}

void LightningBolt::load(TCODZip &zip)
{
	range = zip.getFloat();
	damage = zip.getFloat();
}

void LightningBolt::save(TCODZip &zip)
{
	zip.putInt(LIGHTNING_BOLT);
	zip.putFloat(range);
	zip.putFloat(damage);
}

static const int LIGHTNING_LENGTH = 2;
bool LightningBolt::use(Actor *owner, Actor *wearer)
{
	Actor *closestMonster = engine.getClosestMonster(wearer->getX(), wearer->getY(), range);
	if (!closestMonster)
	{
		engine.ui->message(TCODColor::lightGrey, "No enemy is close enough to strike.");
		return false;
	}

	// Hit the closest monster for <damage> hit points.
	int actualDamage = closestMonster->destructible->takeDamage(closestMonster, damage);
	engine.ui->message(TCODColor::lightBlue, "A lightning bolt strikes the %s with a loud thunder!\nThe damage is %d hit points.", closestMonster->name, actualDamage);

	// Create a normalized slope.
	float dx = closestMonster->getX() - wearer->getX();
	float dy = closestMonster->getY() - wearer->getY();
	float slope = sqrtf(dx * dx + dy * dy);
	dx = dx / slope;
	dy = dy / slope;

	for (int n = 0; n < LIGHTNING_LENGTH; n++)
	{
		engine.particles->add(new RayParticle(wearer->getX() + dx * n, wearer->getY() + dy * n, '*', TCODColor::lightBlue, TCODColor::black, 80, dx, dy, slope - n));
	}

	return Pickable::use(owner, wearer);
}
