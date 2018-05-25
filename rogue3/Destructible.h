#pragma once

#include "Constants.h"

class Actor;

class Destructible : public Persistent
{
public:
	int maxHealth; // maximum health points
	int health; // current health points
	int defense; // hit points deflected
	char corpseName[MAX_NAME_LENGTH]; // the actor's name once dead/destroyed

	Destructible(int maxHealth, int defense, const char *corpseName);
	virtual ~Destructible() {};
	void load(TCODZip &zip);
	virtual void save(TCODZip &zip);
	static Destructible *create(TCODZip &zip);

	inline bool isDead() { return health <= 0; }
	int takeDamage(Actor *owner, int damage); // returns the amount of damage actually taken (damage - defense?)
	int heal(int amount);
	virtual void die(Actor *owner);

protected:
	enum DestructibleType { MONSTER, PLAYER };
};

