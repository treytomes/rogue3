#pragma once

#include "Constants.h"

#include "Attacker.h"
#include "Destructible.h"
#include "AI.h"
#include "Pickable.h"
#include "Container.h"

class Actor : public Persistent
{
public:
	// Setting speed to 0 will effectively disable this actor.
	int speed; // number of energy units recovered on each turn
	int energy; // energy must be >= 0 to do anything

	int tileIndex; // ascii code
	char name[MAX_NAME_LENGTH]; // the actor's name
	TCODColor foregroundColor;
	TCODColor backgroundColor;

	bool blocksMovement; // can we walk on this actor?
	bool blocksVision; // can we see through this actor?
	bool fovOnly; // only display when in field-of-view

	Attacker *attacker; // something that deals damage
	Destructible *destructible; // something that can be damaged
	AI *ai; // something self-updating
	Pickable *pickable; // something that can be picked and used
	Container *container; // something that can contain actors

	Actor(int x, int y, int tileIndex, const char *name, const TCODColor &foregroundColor, const TCODColor &backgroundColor = TCODColor::black);
	~Actor();
	void load(TCODZip &zip);
	void save(TCODZip &zip);
	virtual void update();
	void render(int offsetX, int offsetY) const;
	float getDistance(int cx, int cy) const; // distance from actor to position

	inline int getX() const { return x; }
	inline void setX(int x) { moveTo(x, y); }
	inline int getY() const { return y; }
	inline void setY(int y) { moveTo(x, y); }
	inline void moveBy(int dx, int dy) { moveTo(x + dx, y + dy); }
	void moveTo(int targetX, int targetY);

private:
	int x, y; // position on map

};