#include "stdafx.h"
#include "Particle.h"

Particle::Particle(float x, float y, int tileIndex, TCODColor foregroundColor, TCODColor backgroundColor, int frameTime)
	: x(x), y(y), tileIndex(tileIndex), foregroundColor(foregroundColor), backgroundColor(backgroundColor), frameTime(frameTime), age(0), lastUpdateTime(0), backgroundAlpha(1.0f)
{
}

//Particle::~Particle()
//{
//}

bool Particle::canUpdate()
{
	int currentTime = TCOD_sys_elapsed_milli(); // TCODSystem::getElapsedMilli();
	if (currentTime - lastUpdateTime < frameTime)
	{
		return false;
	}
	lastUpdateTime = currentTime;
	age += frameTime;
	return true;
}

void Particle::render(TCODConsole *target, int offsetX, int offsetY)
{
	int renderX = (int)(x + offsetX);
	int renderY = (int)(y + offsetY);
	target->setChar(renderX, renderY, tileIndex);
	target->setCharForeground(renderX, renderY, foregroundColor);
	target->setCharBackground(renderX, renderY, backgroundColor, TCOD_BKGND_ALPHA(backgroundAlpha));
}

bool Particle::isAlive()
{
	return true;
}