#pragma once

#include "Pickable.h"

class Actor;

// An actor that can be used to heal another actor.
class Healer : public Pickable
{
public:
	int amount; // how much health to recover

	Healer(int amount);
	bool use(Actor *owner, Actor *wearer);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};

