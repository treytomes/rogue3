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
	void tryGetItem(Actor *owner, int x, int y);
	Actor *chooseFromInventory(Actor *owner);

	// Wait for an arrow / numpad key to be pressed.  Returns false if no direction was chosen.
	// The chosen direction will be added to the input coordinates.
	bool tryChooseDirection(int *x, int *y);
};

