#pragma once

#include "AI.h"

class ConfusedMonsterAI : public AI
{
public:
	ConfusedMonsterAI(int numTurns, AI *oldAI);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
	int update(Actor *owner);

protected:
	bool firstUpdate;
	int numTurns;
	AI *oldAI;
	TCODColor oldBackgroundColor;
};

