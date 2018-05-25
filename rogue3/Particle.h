#pragma once

class Particle
{
public:
	Particle(float x, float y, int tileIndex, TCODColor foregroundColor, TCODColor backgroundColor, int frameTime);
	//~Particle();

	bool canUpdate();
	virtual void update() = 0;
	void render(TCODConsole *target, int offsetX, int offsetY);

	// A particle will be removed from the system once it dies.
	virtual bool isAlive();

protected:
	int tileIndex, frameTime, age, lastUpdateTime;
	float x, y, backgroundAlpha;
	TCODColor foregroundColor, backgroundColor;
};
