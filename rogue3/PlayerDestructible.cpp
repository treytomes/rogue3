#include "stdafx.h"
#include "PlayerDestructible.h"
#include "Engine.h"

PlayerDestructible::PlayerDestructible(int maxHealth, int defense, const char *corpseName)
	: Destructible(maxHealth, defense, corpseName, 0)
{
}

void PlayerDestructible::save(TCODZip &zip)
{
	zip.putInt(PLAYER);
	Destructible::save(zip);
}

void PlayerDestructible::die(Actor *owner)
{
	engine.ui->message(TCODColor::red, "You died!\n");
	Destructible::die(owner);
	engine.gameStatus = Engine::DEFEAT;
}