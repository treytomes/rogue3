#include "stdafx.h"

#include "Actor.h"
#include "Engine.h"
#include <math.h>

Actor::Actor(int x, int y, int tileIndex, const char *name, const TCODColor &foregroundColor, const TCODColor &backgroundColor)
	: x(x), y(y), tileIndex(tileIndex), foregroundColor(foregroundColor), energy(0), speed(1), attacker(NULL), destructible(NULL), ai(NULL), container(NULL), pickable(NULL),
	blocks(true), fovOnly(true)
{
	if (name != NULL)
	{
		if (strlen(name) > MAX_NAME_LENGTH)
		{
			engine.ui->message(TCODColor::yellow, "This name is too long: %s", name);
			memcpy(this->name, name, MAX_NAME_LENGTH);
		}
		else
		{
			strcpy(this->name, name);
		}
	}
}

Actor::~Actor()
{
	if (destructible != NULL)
	{
		delete destructible;
		destructible = NULL;
	}
	if (attacker != NULL)
	{
		delete attacker;
		attacker = NULL;
	}
	if (ai != NULL)
	{
		delete ai;
		ai = NULL;
	}
	if (container != NULL)
	{
		delete container;
		container = NULL;
	}
	if (pickable != NULL)
	{
		delete pickable;
		pickable = NULL;
	}
}

void Actor::load(TCODZip &zip)
{
	speed = zip.getInt();
	energy = zip.getInt();
	x = zip.getInt();
	y = zip.getInt();
	tileIndex = zip.getInt();
	strcpy(name, zip.getString());
	foregroundColor = zip.getColor();
	backgroundColor = zip.getColor();
	blocks = zip.getInt();
	fovOnly = zip.getInt();

	bool hasAttacker = zip.getInt();
	bool hasDestructible = zip.getInt();
	bool hasAi = zip.getInt();
	bool hasPickable = zip.getInt();
	bool hasContainer = zip.getInt();

	if (hasAttacker)
	{
		attacker = new Attacker(0.0f);
		attacker->load(zip);
	}
	if (hasDestructible)
	{
		destructible = Destructible::create(zip);
	}
	if (hasAi)
	{
		ai = AI::create(zip);
	}
	if (hasPickable)
	{
		pickable = Pickable::create(zip);
	}
	if (hasContainer)
	{
		container = new Container(0);
		container->load(zip);
	}
}

void Actor::save(TCODZip &zip)
{
	zip.putInt(speed);
	zip.putInt(energy);
	zip.putInt(x);
	zip.putInt(y);
	zip.putInt(tileIndex);
	zip.putString(name);
	zip.putColor(&foregroundColor);
	zip.putColor(&backgroundColor);
	zip.putInt(blocks);
	zip.putInt(fovOnly);

	zip.putInt(attacker != NULL);
	zip.putInt(destructible != NULL);
	zip.putInt(ai != NULL);
	zip.putInt(pickable != NULL);
	zip.putInt(container != NULL);

	if (attacker)
	{
		attacker->save(zip);
	}
	if (destructible)
	{
		destructible->save(zip);
	}
	if (ai)
	{
		ai->save(zip);
	}
	if (pickable)
	{
		pickable->save(zip);
	}
	if (container)
	{
		container->save(zip);
	}
}

void Actor::update()
{
	if (energy < 0)
	{
		energy += speed * 8;
		return;
	}

	if (ai)
	{
		energy -= ai->update(this);
	}
	//else
	//{
	//	engine.ui->message(TCODColor::yellow, "The %s growls!\n", name);
	//	energy -= MOVE_ATTEMPT_COST;
	//}
}

void Actor::render(int offsetX, int offsetY) const
{
	TCODConsole::root->setChar(x + offsetX, y + offsetY, tileIndex);
	if (destructible)
	{
		// This should make a destructible actor turn redder as it approaches death.
		float healthRatio = destructible->health / (float)destructible->maxHealth;
		TCODColor modifiedForegroundColor = TCODColor::darkRed * (1 - healthRatio) + foregroundColor * healthRatio;
		TCODConsole::root->setCharForeground(x + offsetX, y + offsetY, modifiedForegroundColor);
	}
	else
	{
		TCODConsole::root->setCharForeground(x + offsetX, y + offsetY, foregroundColor);
	}
	if (backgroundColor != TCODColor::black)
	{
		TCODConsole::root->setCharBackground(x + offsetX, y + offsetY, backgroundColor);
	}
}

float Actor::getDistance(int cx, int cy) const
{
	int dx = x - cx;
	int dy = y - cy;
	return sqrtf((dx * dx) + (dy * dy));
}