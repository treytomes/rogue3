#include "stdafx.h"
#include "ParticleSystem.h"
#include "RayParticle.h"

ParticleSystem::ParticleSystem()
	: lastUpdateTime(0)
{
}


ParticleSystem::~ParticleSystem()
{
	particles.clearAndDelete();
}

void ParticleSystem::update()
{
	TCODList<Particle *> deadParticles;
	for (Particle **iter = particles.begin(); iter != particles.end(); iter++)
	{
		Particle *particle = *iter;
		if (particle->canUpdate())
		{
			particle->update();
			if (!particle->isAlive())
			{
				//printf("I am dead!\n");
				deadParticles.push(particle);
			}
		}
	}

	for (Particle **iter = deadParticles.begin(); iter != deadParticles.end(); iter++)
	{
		Particle *particle = *iter;
		particles.remove(particle);
	}

	deadParticles.clearAndDelete();
}

void ParticleSystem::render(TCODConsole *target, int offsetX, int offsetY)
{
	for (Particle **iter = particles.begin(); iter != particles.end(); iter++)
	{
		Particle *particle = *iter;
		particle->render(target, offsetX, offsetY);
	}
}

void ParticleSystem::add(Particle *particle)
{
	particles.push(particle);
}

void ParticleSystem::spawnDeathParticles(int x, int y)
{
	add(new RayParticle(x, y, '*', TCODColor::red, TCODColor::black, 60, -1, -1, 4));
	add(new RayParticle(x, y, '*', TCODColor::red, TCODColor::black, 60, -1,  0, 4));
	add(new RayParticle(x, y, '*', TCODColor::red, TCODColor::black, 60, -1,  1, 4));
	add(new RayParticle(x, y, '*', TCODColor::red, TCODColor::black, 60,  0,  1, 4));
	add(new RayParticle(x, y, '*', TCODColor::red, TCODColor::black, 60,  0, -1, 4));
	add(new RayParticle(x, y, '*', TCODColor::red, TCODColor::black, 60,  1, -1, 4));
	add(new RayParticle(x, y, '*', TCODColor::red, TCODColor::black, 60,  1,  0, 4));
	add(new RayParticle(x, y, '*', TCODColor::red, TCODColor::black, 60,  1,  1, 4));
}

void ParticleSystem::spawnNumberParticles(int x, int y, int value)
{
	char numberText[4] = { '\0' };

	TCODColor foregroundColor = TCODColor::white;
	if (value < 0)
	{
		value = -value;
		foregroundColor = TCODColor::red;
	}
	else if (value > 0)
	{
		foregroundColor = TCODColor::green;
	}

	itoa(value, numberText, 10);
	for (int n = 0; n < strlen(numberText); n++)
	{
		add(new RayParticle(x, y, numberText[n], foregroundColor, TCODColor::black, 60, 0, -1, 8));
	}
}