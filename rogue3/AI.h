#pragma once

class Actor;

class AI : public Persistent
{
public:
	virtual ~AI() {};
	static AI *create(TCODZip &zip);

	// Perform some action and return the cost of said action.
	virtual int update(Actor *owner) = 0;

	// Allow an AI to respond to player input.  Called from PlayerAI.update.
	virtual int handleActionKey(Actor *owner, int ascii);

protected:
	enum AIType { LUA, MONSTER, CONFUSED_MONSTER, PLAYER, STAIRS };
};
