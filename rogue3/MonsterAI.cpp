#include "stdafx.h"
#include "MonsterAI.h"
#include "Actor.h"
#include "Engine.h"

#include <math.h>

static const int PERCEPTION = 12; // 16 = fully perceptive, max smell sensitivity.
static const float SMELL_SENSITIVITY = MAX_STINK - PERCEPTION * 16;

MonsterAI::MonsterAI()
	: moveCount(0), currentPath(NULL), currentTargetX(-1), currentTargetY(-1)
{
}

MonsterAI::~MonsterAI()
{
	if (currentPath)
	{
		delete currentPath;
		currentPath = NULL;
	}
}

void MonsterAI::load(TCODZip &zip)
{
	moveCount = zip.getInt();

	if (zip.getInt())
	{
		currentTargetX = zip.getInt();
		currentTargetY = zip.getInt();
	}
}

void MonsterAI::save(TCODZip &zip)
{
	zip.putInt(MONSTER);
	zip.putInt(moveCount);

	if ((currentPath == NULL) || currentPath->isEmpty())
	{
		zip.putInt(false);
	}
	else
	{
		zip.putInt(true);
		zip.putInt(currentTargetX);
		zip.putInt(currentTargetY);
	}
}

int MonsterAI::update(Actor *owner)
{
	if (owner->destructible && owner->destructible->isDead())
	{
		return 0;
	}

	if ((currentPath == NULL) && (currentTargetX != -1) && (currentTargetY != -1))
	{
		calculatePath(owner, currentTargetX, currentTargetY);
	}

	// 1. If the monster is next to the player, then hit the player.
	// 2. Else, if the monster can see the player, then move towards the player.
	// 3. Else, if the monster can smell the player, then move towards the smell.
	// 4. Else, move towards the last known player position.
	// 5. What if the monster doesn't have a path?  Maybe find some friends to hang out with?  Or explore the dungeon?

	if (canSeePlayer(owner)) // monster can see the player
	{
		// TODO: Is it cost-effective to give each actor it's own field-of-view?
		// We can see the player.  Move towards him.
		calculatePath(owner, engine.player);
	}
	else
	{
		Scent *playerScent = findSmell(owner, 0);
		if (playerScent != NULL)
		{
			printf("%s smells the player.\n", owner->name);
			calculatePath(owner, playerScent->x, playerScent->y);
		}
		else
		{
			// Keep moving on the existing path for a while.
			moveCount--;
		}
	}

	if (moveCount > 0)
	{
		return moveOrAttack(owner); // , engine.player->x, engine.player->y);
	}
	else
	{
		// Nothing to do, nowhere to go...
		return COST_WAIT;
	}
}

int MonsterAI::moveOrAttack(Actor *owner)
{
	if ((currentPath == NULL) || currentPath->isEmpty())
	{
		// Nothing to do, nowhere to go...
		return COST_WAIT;
	}

	// 1. Get the next position on the path.
	// 2. If we're not there, then go there.
	int x = -1, y = -1;
	if (currentPath->walk(&x, &y, true))
	{
		// TODO: Make sure the first step is actually going somewhere.
		printf("%s is walking from (%d, %d) to (%d, %d).\n", owner->name, owner->getX(), owner->getY(), x, y);
		Stage *currentStage = engine.getCurrentStage();
		if (currentStage->map->canWalk(x, y))
		{
			owner->moveTo(x, y);
			return COST_MOVE;
		}
		else
		{
			if (owner->attacker && owner->attacker->attack(owner, x, y))
			{
				return COST_ATTACK;
			}
		}
	}

	return COST_WAIT;
}

int MonsterAI::moveOrAttack(Actor *owner, int targetX, int targetY)
{
	int dx = targetX - owner->getX();
	int dy = targetY - owner->getY();
	int stepX = (dx > 0 ? 1 : -1);
	int stepY = (dy > 0 ? 1 : -1);

	float distance = distanceTo(owner, targetX, targetY);
	if (distance >= 2)
	{
		// Normalize the distance.
		dx = (int)round(dx / distance);
		dy = (int)round(dy / distance);

		Stage *currentStage = engine.getCurrentStage();
		if (currentStage->map->canWalk(owner->getX() + dx, owner->getY() + dy))
		{
			owner->moveBy(dx, dy);
			return COST_MOVE;
		}
		else if (currentStage->map->canWalk(owner->getX() + stepX, owner->getY()))
		{
			owner->moveBy(stepX, 0);
			return COST_MOVE;
		}
		else if (currentStage->map->canWalk(owner->getX(), owner->getY() + stepY))
		{
			owner->moveBy(0, stepY);
			return COST_MOVE;
		}
		else
		{
			return COST_MOVE_FAIL;
		}
	}
	else if (owner->attacker)
	{
		// TODO: The target position may not be a player.
		owner->attacker->attack(owner, owner->getX() + dx, owner->getY() + dy);
		return COST_ATTACK;
	}
	else
	{
		return COST_WAIT;
	}
}

Scent *MonsterAI::findSmell(Actor *owner, int smellsLikeId) const
{
	Scent *activeScent = NULL;
	float maxIntensity = 0.0f;
	for (Scent **iter = engine.getCurrentStage()->scents.begin(); iter != engine.getCurrentStage()->scents.end(); iter++)
	{
		Scent *scent = *iter;
		if ((scent->smellsLikeId == smellsLikeId) && (scent->intensity >= SMELL_SENSITIVITY) && (distanceTo(owner, scent->x, scent->y) <= scent->radius))
		{
			if (!activeScent || (scent->intensity > activeScent->intensity))
			{
				activeScent = scent;
			}
		}
	}
	return activeScent;
}

void MonsterAI::calculatePath(Actor *owner, Actor *target)
{
	calculatePath(owner, target->getX(), target->getY());
}

void MonsterAI::calculatePath(Actor *owner, int x, int y)
{
	currentTargetX = x;
	currentTargetY = y;
	currentPath = engine.getCurrentStage()->map->calculatePath(owner->getX(), owner->getY(), currentTargetX, currentTargetY, currentPath);

	// TODO: Base TRACKING_TURNS on actor intelligence.
	// When the monster chooses a new path, it will keep going along that path until it forgets what it's doing.
	moveCount = TRACKING_TURNS;
}

float MonsterAI::distanceTo(Actor *owner, int x, int y) const
{
	int dx = owner->getX() - x;
	int dy = owner->getY() - y;
	return sqrtf(dx * dx + dy * dy);
}

bool MonsterAI::isInPath(Actor *actor)
{
	if (actor == NULL)
	{
		return false;
	}
	return isInPath(actor->getX(), actor->getY());
}

bool MonsterAI::isInPath(int x, int y)
{
	if ((currentPath == NULL) || currentPath->isEmpty())
	{
		return false;
	}

	// More likely than not, the actor we are checking on is the player, located towards the end of the path.
	for (int n = currentPath->size() - 1; n >= 0; n--)
	{
		int pathX = 0, pathY = 0;
		currentPath->get(n, &x, &y);
		if ((pathX == x) && (pathY == y))
		{
			return true;
		}
	}

	return false;
}


bool MonsterAI::canSeePlayer(Actor *owner)
{
	return engine.getCurrentStage()->map->isInFov(owner->getX(), owner->getY());
}