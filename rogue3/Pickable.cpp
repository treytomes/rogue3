#include "stdafx.h"
#include "Pickable.h"
#include "Actor.h"
#include "Engine.h"
#include "Healer.h"
#include "LightningBolt.h"
#include "Confuser.h"
#include "Fireball.h"

Pickable::Pickable()
{
}

Pickable *Pickable::create(TCODZip &zip)
{
	PickableType type = (PickableType)zip.getInt();
	Pickable *pickable = NULL;
	switch (type) {
	case HEALER:
		pickable = new Healer(0);
		break;
	case LIGHTNING_BOLT:
		pickable = new LightningBolt(0, 0);
		break;
	case CONFUSER:
		pickable = new Confuser(0, 0);
		break;
	case FIREBALL:
		pickable = new Fireball(0, 0);
		break;
	}
	pickable->load(zip);
	return pickable;
}

bool Pickable::pick(Actor *owner, Actor *wearer)
{
	if (wearer->container && wearer->container->add(owner))
	{
		engine.getCurrentStage()->actors.remove(owner);
		return true;
	}
	return false;
}

void Pickable::drop(Actor *owner, Actor *wearer)
{
	if (wearer->container)
	{
		wearer->container->remove(owner);
		if (owner->blocks)
		{
			engine.getCurrentStage()->actors.push(owner);
		}
		else
		{
			engine.getCurrentStage()->actors.insertBefore(owner, 0);
		}

		owner->x = wearer->x;
		owner->y = wearer->y;
		engine.ui->message(TCODColor::lightGrey, "%s drops a %s.", wearer->name, owner->name);
	}
}

bool Pickable::use(Actor *owner, Actor *wearer)
{
	if (wearer->container)
	{
		// TODO: What if the owner is used, but not consumed?
		wearer->container->remove(owner);
		delete owner;
		return true;
	}
	return false;
}
