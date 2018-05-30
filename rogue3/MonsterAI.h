#pragma once

#include "AI.h"
#include "Scent.h"

class MonsterAI : public AI
{
public:
	MonsterAI();
	~MonsterAI();
	void load(TCODZip &zip);
	void save(TCODZip &zip);

	int update(Actor *owner);

protected:
	int currentTargetX, currentTargetY;
	TCODPath *currentPath;
	int moveCount;

	int moveOrAttack(Actor *owner);
	int moveOrAttack(Actor *owner, int targetX, int targetY);

	// Attempt to sniff out a particular type of actor.
	Scent *findSmell(Actor *owner, int smellsLikeId) const;

	// Monster finds new purpose in a quest for a new position.
	void calculatePath(Actor *owner, Actor *target);
	void calculatePath(Actor *owner, int x, int y);

	// How far to the target position?
	inline float distanceTo(Actor *owner, int x, int y) const;

	bool isInPath(Actor *actor);
	bool isInPath(int x, int y);
	bool canSeePlayer(Actor *owner);
};

