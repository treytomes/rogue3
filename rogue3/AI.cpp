#include "stdafx.h"
#include "AI.h"
#include "PlayerAI.h"
#include "MonsterAI.h"
#include "ConfusedMonsterAI.h"
#include "LuaAI.h"
#include "StairsAI.h"

AI *AI::create(TCODZip &zip) {
	AIType type = (AIType)zip.getInt();
	AI *ai = NULL;
	switch (type) {
	case LUA:
		ai = new LuaAI();
		break;
	case PLAYER:
		ai = new PlayerAI();
		break;
	case MONSTER:
		ai = new MonsterAI();
		break;
	case CONFUSED_MONSTER:
		ai = new ConfusedMonsterAI(0, NULL);
		break;
	case STAIRS:
		ai = new StairsAI(0);
		break;
	}
	ai->load(zip);
	return ai;
}

int AI::handleActionKey(Actor *owner, int ascii)
{
	return 0;
}