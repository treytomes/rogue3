#pragma once
#include "AI.h"

class PlayerAI : public AI
{
public:
	int xpLevel;

	PlayerAI();
	void load(TCODZip &zip);
	void save(TCODZip &zip);
	int update(Actor *owner);

	int getNextLevelXP() const;

private:
	int moveOrAttack(Actor *owner, int targetX, int targetY);
	int handleActionKey(Actor *owner, int ascii);

	void tryGetItem(Actor *owner);
	Actor *chooseFromInventory(Actor *owner);
};

