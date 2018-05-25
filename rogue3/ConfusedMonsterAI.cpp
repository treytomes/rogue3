#include "stdafx.h"
#include "ConfusedMonsterAI.h"
#include "Engine.h"

ConfusedMonsterAI::ConfusedMonsterAI(int numTurns, AI *oldAI)
	: numTurns(numTurns), oldAI(oldAI), firstUpdate(true)
{
}

void ConfusedMonsterAI::load(TCODZip &zip)
{
	numTurns = zip.getInt();
	firstUpdate = zip.getInt();
	oldBackgroundColor = zip.getColor();
	oldAI = AI::create(zip);
}

void ConfusedMonsterAI::save(TCODZip &zip)
{
	zip.putInt(CONFUSED_MONSTER);
	zip.putInt(numTurns);
	zip.putInt(firstUpdate);
	zip.putColor(&oldBackgroundColor);
	oldAI->save(zip);
}

int ConfusedMonsterAI::update(Actor *owner)
{
	if (firstUpdate)
	{
		// TODO: If the actor dies, this AI gets deleted and the color never reverts back.
		oldBackgroundColor = owner->backgroundColor;
		owner->backgroundColor = TCODColor::green;
		firstUpdate = false;
	}

	TCODRandom *rng = TCODRandom::getInstance();
	int dx = rng->getInt(-1, 1);
	int dy = rng->getInt(-1, 1);
	int cost = COST_WAIT;

	if ((dx != 0) || (dy != 0))
	{
		int destX = owner->x + dx;
		int destY = owner->y + dy;
		if (engine.getCurrentStage()->map->canWalk(destX, destY))
		{
			owner->x = destX;
			owner->y = destY;
			cost = COST_MOVE;
		}
		else
		{
			Actor *actor = engine.getActor(destX, destY);
			if (actor && actor->destructible && !actor->destructible->isDead())
			{
				owner->attacker->attack(owner, destX, destY);
				cost = COST_ATTACK;
			}
			else
			{
				cost = COST_MOVE_FAIL;
			}
		}
	}

	numTurns--;
	if (numTurns == 0)
	{
		owner->backgroundColor = oldBackgroundColor;
		engine.ui->message(TCODColor::lightGreen, "The %s has come back to it's senses.", owner->name);
		owner->ai = oldAI;
		delete this;
	}

	return cost;
}
