#include "stdafx.h"
#include "Scent.h"

#include <math.h>

Scent::Scent(int smellsLikeId, int x, int y, float intensity)
	: smellsLikeId(smellsLikeId), x(x), y(y), intensity(intensity), radius(1)
{
}

void Scent::load(TCODZip &zip)
{
	smellsLikeId = zip.getInt();
	x = zip.getInt();
	y = zip.getInt();
	intensity = zip.getFloat();
	radius = zip.getInt();
}

void Scent::save(TCODZip &zip)
{
	zip.putInt(smellsLikeId);
	zip.putInt(x);
	zip.putInt(y);
	zip.putFloat(intensity);
	zip.putInt(radius);
}

void Scent::update()
{
	radius++;
	intensity *= 0.8f;
	//printf("Stink intensity: %g, radius (%d)\n", intensity, radius);
}

#ifdef RENDER_SCENTS
void Scent::render(int offsetX, int offsetY) const
{
	float scentRatio = intensity / MAX_STINK; // convert to an alpha value

	for (int rx = x - radius; rx <= x + radius; rx++)
	{
		for (int ry = y - radius; ry <= y + radius; ry++)
		{
			if (sqrtf((rx - x) * (rx - x) + (ry - y) * (ry - y)) <= radius)
			{
				int ox = rx + offsetX;
				int oy = ry + offsetY;

				//TCODConsole::root->setChar(ox, oy, '%');
				//TCODConsole::root->setCharForeground(ox, oy, TCODColor::purple);
				TCODColor bgColor = TCODConsole::root->getCharBackground(ox, oy);
				//bgColor = bgColor * (1 - scentRatio) + TCODColor::lightGreen * scentRatio;
				bgColor = (bgColor + TCODColor::lightGreen * scentRatio) * 0.5f;

				//TCODConsole::root->setCharBackground(ox, oy, TCODColor::lightGreen, TCOD_BKGND_ALPHA(scentRatio));
				TCODConsole::root->setCharBackground(ox, oy, bgColor);
			}
		}
	}
}
#endif

bool Scent::isAlive() const
{
	return intensity >= 1.0f;
}