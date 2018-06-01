#include "stdafx.h"
#include "DoorAI.h"

#include "Actor.h"
#include "Engine.h"

DoorAI::DoorAI(bool isOpen)
	: isOpen(isOpen), isInitialized(false)
{
}

DoorAI::~DoorAI()
{
}

void DoorAI::load(TCODZip &zip)
{
	isOpen = zip.getInt();
}

void DoorAI::save(TCODZip &zip)
{
	zip.putInt(DOOR);
	zip.putInt(isOpen);
}

int DoorAI::update(Actor *owner)
{
	//if (!isInitialized)
	//{
	//	engine.getCurrentStage()->map.setBlocks
	//}
	return 0;
}

int DoorAI::handleActionKey(Actor *owner, int ascii)
{
	//if (!isOpen && (engine.lastKey.c == 'o'))
	//{
	//	engine.gotoPreviousStage(owner->x, owner->y);
	//	return COST_GET_ITEM;
	//}
	//else if (isOpen && (engine.lastKey.c == 'c'))
	//{
	//	engine.gotoNextStage(owner->x, owner->y);
	//	return COST_GET_ITEM;
	//}

	return 0;
}