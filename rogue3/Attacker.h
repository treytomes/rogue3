#pragma once

class Actor;

class Attacker : public Persistent
{
public:
	int power; // hit points given

	Attacker(int power);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
	bool can_attack(Actor *owner, int targetX, int targetY);
	bool attack(Actor *owner, int targetX, int targetY);
};

