#pragma once

#include "AI.h"

class Actor;

class StairsAI : public AI
{
public:
	StairsAI(bool goingDown);
	~StairsAI();

	void load(TCODZip &zip);
	void save(TCODZip &zip);

	int update(Actor *owner);
	int handleActionKey(Actor *owner, int ascii);

	bool goingDown;
};

