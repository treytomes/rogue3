#pragma once

#include "libtcod.hpp"

// This will darken the color for percentages < 0, or brighten it for percentages > 0.
inline TCODColor modify_color(TCODColor sourceColor, float percentage)
{
	//return TCODColor((int)(sourceColor.r * percentage), (int)(sourceColor.g * percentage), (int)(sourceColor.b * percentage));
	return sourceColor * percentage;
}