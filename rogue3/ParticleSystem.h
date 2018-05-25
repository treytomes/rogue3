#pragma once

#include "Particle.h"

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	void update();
	void render(TCODConsole *target, int offsetX, int offsetY);
	void add(Particle *particle);

	void spawnDeathParticles(int x, int y);

	// Create a text particle that rises into the air.  0 = white, <0 = red, >0 = green.
	void spawnNumberParticles(int x, int y, int value);

protected:
	TCODList<Particle *> particles;
	float lastUpdateTime;
};

