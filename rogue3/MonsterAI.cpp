#include "stdafx.h"
#include "MonsterAI.h"
#include "Actor.h"
#include "Engine.h"
#include <math.h>

MonsterAI::MonsterAI()
	: moveCount(0)
{
}

void MonsterAI::load(TCODZip &zip)
{
	moveCount = zip.getInt();
}

void MonsterAI::save(TCODZip &zip)
{
	zip.putInt(MONSTER);
	zip.putInt(moveCount);
}

int MonsterAI::update(Actor *owner)
{
	if (owner->destructible && owner->destructible->isDead())
	{
		return 0;
	}

	if (engine.getCurrentStage()->map->isInFov(owner->x, owner->y))
	{
		// We can see the player.  Move towards him.
		moveCount = TRACKING_TURNS;
	}
	else
	{
		moveCount--;
	}

	if (moveCount > 0)
	{
		return moveOrAttack(owner, engine.player->x, engine.player->y);
	}

	return COST_WAIT;
}

int MonsterAI::moveOrAttack(Actor *owner, int targetX, int targetY)
{
	int dx = targetX - owner->x;
	int dy = targetY - owner->y;
	int stepX = (dx > 0 ? 1 : -1);
	int stepY = (dy > 0 ? 1 : -1);

	float distance = sqrtf(dx * dx + dy * dy);
	if (distance >= 2)
	{
		// Normalize the distance.
		dx = (int)round(dx / distance);
		dy = (int)round(dy / distance);

		Stage *currentStage = engine.getCurrentStage();
		if (currentStage->map->canWalk(owner->x + dx, owner->y + dy))
		{
			owner->x += dx;
			owner->y += dy;
			return COST_MOVE;
		}
		else if (currentStage->map->canWalk(owner->x + stepX, owner->y))
		{
			owner->x += stepX;
			return COST_MOVE;
		}
		else if (currentStage->map->canWalk(owner->x, owner->y + stepY))
		{
			owner->y += stepY;
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
		owner->attacker->attack(owner, owner->x + dx, owner->y + dy);
		return COST_ATTACK;
	}
	else
	{
		return COST_WAIT;
	}
}
