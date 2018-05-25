#include "stdafx.h"
#include "RayParticle.h"
#include <math.h>

RayParticle::RayParticle(float x, float y, int tileIndex, TCODColor foregroundColor, TCODColor backgroundColor, int frameTime, float dx, float dy, float range)
	: Particle(x, y, tileIndex, foregroundColor, backgroundColor, frameTime),
#ifdef RAYPARTICLE_FADE_COLORS_OVER_TIME
		initialForegroundColor(foregroundColor), initialBackgroundColor(backgroundColor),
#endif
	startX((int)x), startY((int)y), dx(1), dy(dy / dx), range(range)
{
	float slope = sqrtf(dx * dx + dy * dy);
	this->dx = dx / slope;
	this->dy = dy / slope;

	backgroundAlpha = 0.5f;
}

//RayParticle::~RayParticle()
//{
//}

void RayParticle::update()
{
	x += dx;
	y += dy;
	//printf("Ray: %d, %d, %d, %d\n", x, y, dx, dy);

#ifdef FADE_COLORS_OVER_TIME
	float colorRatio = 1.0f - age / (float)lifespan;
	foregroundColor = initialForegroundColor * colorRatio;
	backgroundColor = initialBackgroundColor * colorRatio;
#endif
}

bool RayParticle::isAlive()
{
	int xRange = startX - x;
	int yRange = startY - y;
	return sqrtf((xRange * xRange) + (yRange * yRange)) <= range;
}