#pragma once
#include "Particle.h"

//#define RAYPARTICLE_FADE_COLORS_OVER_TIME

// A particle that shoots off at a vector for a given time.
class RayParticle : public Particle
{
public:
	RayParticle(float x, float y, int tileIndex, TCODColor foregroundColor, TCODColor backgroundColor, int frameTime, float dx, float dy, float range);
	//~RayParticle();

	void update();
	bool isAlive();

protected:
	int startX, startY;
	float dx, dy, range;

#ifdef RAYPARTICLE_FADE_COLORS_OVER_TIME
	TCODColor initialForegroundColor, initialBackgroundColor;
#endif
};
