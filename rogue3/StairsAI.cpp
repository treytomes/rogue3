#include "stdafx.h"
#include "StairsAI.h"

#include "Actor.h"
#include "Engine.h"

StairsAI::StairsAI(bool goingDown)
	: goingDown(goingDown)
{
}

StairsAI::~StairsAI()
{
}

void StairsAI::load(TCODZip &zip)
{
	goingDown = zip.getInt();
}

void StairsAI::save(TCODZip &zip)
{
	zip.putInt(STAIRS);
	zip.putInt(goingDown);
}

int StairsAI::update(Actor *owner)
{
	return 0;
}

int StairsAI::handleActionKey(Actor *owner, int ascii)
{
	if ((engine.player->x == owner->x) && (engine.player->y == owner->y))
	{
		if (!goingDown && (engine.lastKey.c == '<'))
		{
			engine.gotoPreviousStage(owner->x, owner->y);
		}
		else if (goingDown && (engine.lastKey.c == '>'))
		{
			engine.gotoNextStage(owner->x, owner->y);
		}
	}

	return 0;
}