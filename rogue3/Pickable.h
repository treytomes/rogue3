#pragma once

class Actor;

class Pickable : public Persistent
{
public:
	Pickable();
	virtual ~Pickable() {};
	static Pickable *create(TCODZip &zip);

	// "wearer" will almost always be the Player.
	bool pick(Actor *owner, Actor *wearer);
	void drop(Actor *owner, Actor *wearer);
	virtual bool use(Actor *owner, Actor *wearer);

protected:
	enum PickableType { HEALER, LIGHTNING_BOLT, CONFUSER, FIREBALL };
};

