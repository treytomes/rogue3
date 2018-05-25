#pragma once

#include "AI.h"
class MonsterAI : public AI
{
public:
	MonsterAI();
	void load(TCODZip &zip);
	void save(TCODZip &zip);

	int update(Actor *owner);

protected:
	int moveCount;

	int moveOrAttack(Actor *owner, int targetX, int targetY);
};

