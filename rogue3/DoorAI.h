#pragma once

#include "AI.h"

class Actor;

class DoorAI : public AI
{
public:
	DoorAI(bool isOpen = false);
	~DoorAI();

	void load(TCODZip &zip);
	void save(TCODZip &zip);

	int update(Actor *owner);
	int handleActionKey(Actor *owner, int ascii);

	bool isOpen;

private:
	bool isInitialized;
};

