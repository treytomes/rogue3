#include "stdafx.h"
#include "Container.h"
#include "Actor.h"

Container::Container(int size)
	: size(size)
{
}

Container::~Container()
{
	inventory.clearAndDelete();
}

void Container::load(TCODZip &zip)
{
	size = zip.getInt();
	int numActors = zip.getInt();
	while (numActors > 0) {
		Actor *actor = new Actor(0, 0, 0, NULL, TCODColor::white);
		actor->load(zip);
		inventory.push(actor);
		numActors--;
	}
}

void Container::save(TCODZip &zip)
{
	zip.putInt(size);
	zip.putInt(inventory.size());
	for (Actor **iter = inventory.begin(); iter != inventory.end(); iter++)
	{
		(*iter)->save(zip);
	}
}

bool Container::add(Actor *actor)
{
	if ((size > 0) && (inventory.size() >= size))
	{
		return false;
	}
	inventory.push(actor);
	return true;
}

void Container::remove(Actor *actor)
{
	inventory.remove(actor);
}
