#include "stdafx.h"
#include "Confuser.h"
#include "Engine.h"
#include "ConfusedMonsterAI.h"

Confuser::Confuser(int numTurns, float range)
	: numTurns(numTurns), range(range)
{
}

void Confuser::load(TCODZip &zip)
{
	numTurns = zip.getInt();
	range = zip.getFloat();
}

void Confuser::save(TCODZip &zip)
{
	zip.putInt(CONFUSER);
	zip.putInt(numTurns);
	zip.putFloat(range);
}

bool Confuser::use(Actor *owner, Actor *wearer)
{
	engine.ui->message(TCODColor::cyan, "Left-click an enemy to confuse it,\nor right-click to cancel.");
	int x, y;
	if (!engine.pickATile(&x, &y, range))
	{
		return false;
	}

	Actor *actor = engine.getActor(x, y);
	if (actor && actor->ai)
	{
		// Confuse the monster for <numTurns> turns.
		AI *confusedAI = new ConfusedMonsterAI(numTurns, actor->ai);
		actor->ai = confusedAI;

		engine.ui->message(TCODColor::lightGreen, "The eyes of the %s look vacant,\nas he starts to stumble around!", actor->name);
		return Pickable::use(owner, wearer);
	}

	return false;
}