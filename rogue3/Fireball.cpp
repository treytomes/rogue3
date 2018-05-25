#include "stdafx.h"
#include "Fireball.h"
#include "Engine.h"
#include "RayParticle.h"
#include <math.h>

Fireball::Fireball(float range, int damage)
	: LightningBolt(range, damage)
{
}

void Fireball::save(TCODZip &zip)
{
	zip.putInt(FIREBALL);
	zip.putFloat(range);
	zip.putFloat(damage);
}

bool Fireball::use(Actor *owner, Actor *wearer)
{
	engine.ui->message(TCODColor::cyan, "Left-click a target tile for the fireball,\nor right-click to cancel.");
	int x, y;
	if (!engine.pickATile(&x, &y))
	{
		return false;
	}

	// Burn everything in <range> (including player).
	engine.ui->message(TCODColor::orange, "The fireball explodes, burning everything within %g tiles!", range);
	Stage *currentStage = engine.getCurrentStage();
	for (Actor **iter = currentStage->actors.begin(); iter != currentStage->actors.end(); iter++)
	{
		Actor *actor = *iter;
		if (actor->destructible && !actor->destructible->isDead() && (actor->getDistance(x, y) <= range))
		{
			engine.ui->message(TCODColor::orange, "The %s gets burned for %d hit points.", actor->name, damage);
			actor->destructible->takeDamage(actor, damage);
		}
	}

	//add(new RayParticle(x, y, '*', TCODColor::red, TCODColor::black, 60, -1, -1, 4));
	//add(new RayParticle(x, y, '*', TCODColor::red, TCODColor::black, 60, -1, 0, 4));
	//add(new RayParticle(x, y, '*', TCODColor::red, TCODColor::black, 60, -1, 1, 4));
	//add(new RayParticle(x, y, '*', TCODColor::red, TCODColor::black, 60, 0, 1, 4));
	//add(new RayParticle(x, y, '*', TCODColor::red, TCODColor::black, 60, 0, -1, 4));
	//add(new RayParticle(x, y, '*', TCODColor::red, TCODColor::black, 60, 1, -1, 4));
	//add(new RayParticle(x, y, '*', TCODColor::red, TCODColor::black, 60, 1, 0, 4));
	//add(new RayParticle(x, y, '*', TCODColor::red, TCODColor::black, 60, 1, 1, 4));

	for (float angle = 0; angle < 360; angle += 30)
	{
		float dx = cos(angle * 3.14159 / 180);
		float dy = sin(angle * 3.14159 / 180);
		engine.particles->add(new RayParticle(x, y, '*', TCODColor::orange, TCODColor::darkRed, 120, dx, dy, range));
	}

	return Pickable::use(owner, wearer);
}