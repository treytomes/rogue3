#include "stdafx.h"
#include "PlayerAI.h"

#include "Actor.h"
#include "Engine.h"
#include "Scent.h"

const int LEVEL_UP_BASE = 200;
const int LEVEL_UP_FACTOR = 150;
const int PLAYER_STINK = MAX_STINK;

PlayerAI::PlayerAI()
	: xpLevel(1)
{
}

void PlayerAI::load(TCODZip &zip)
{
	xpLevel = zip.getInt();
}

void PlayerAI::save(TCODZip &zip)
{
	zip.putInt(PLAYER);
	zip.putInt(xpLevel);
}

enum LevelUpMenuCode {
	CONSTITUTION,
	STRENGTH,
	AGILITY
};

int PlayerAI::update(Actor *owner)
{
	int levelUpXP = getNextLevelXP();
	if (owner->destructible->xp >= levelUpXP)
	{
		xpLevel++;
		owner->destructible->xp -= levelUpXP;
		engine.ui->message(TCODColor::yellow, "Your battle skills grow stronger!  You reached level %d.", xpLevel);

		engine.ui->menu.clear();
		engine.ui->menu.addItem(LevelUpMenuCode::CONSTITUTION, "Constitution (+20 HP)");
		engine.ui->menu.addItem(LevelUpMenuCode::STRENGTH, "Strength (+1 attack)");
		engine.ui->menu.addItem(LevelUpMenuCode::AGILITY, "Agility (+1 defense)");
		LevelUpMenuCode selectedItem = (LevelUpMenuCode)engine.ui->menu.pick(Menu::PAUSE	);

		switch (selectedItem)
		{
		case CONSTITUTION:
			owner->destructible->maxHealth += 20;
			owner->destructible->health += 20;
			break;
		case STRENGTH:
			owner->attacker->power++;
			break;
		case AGILITY:
			owner->destructible->defense++;
			break;
		}
	}

	if (owner->destructible && owner->destructible->isDead())
	{
		return 0;
	}

	int cost = 0;
	int playerX = owner->x;
	int playerY = owner->y;

	switch (engine.lastKey.vk)
	{
	case TCODK_KP1:
		playerX--;
		playerY++;
		break;

	case TCODK_KP2:
	case TCODK_DOWN:
		playerY++;
		break;

	case TCODK_KP3:
		playerX++;
		playerY++;
		break;

	case TCODK_KP4:
	case TCODK_LEFT:
		playerX--;
		break;

	case TCODK_KP5:
		engine.ui->message(TCODColor::lightGrey, "Waiting...\n");
		cost += COST_WAIT;
		break;

	case TCODK_KP6:
	case TCODK_RIGHT:
		playerX++;
		break;

	case TCODK_KP7:
		playerX--;
		playerY--;
		break;

	case TCODK_KP8:
	case TCODK_UP:
		playerY--;
		break;

	case TCODK_KP9:
		playerX++;
		playerY--;
		break;

	case TCODK_ESCAPE:
		engine.isPlaying = false;
		break;

	case TCODK_CHAR:
		cost += handleActionKey(owner, engine.lastKey.c);
		break;

	default:
		break;
	}

	if ((owner->x != playerX) || (owner->y != playerY))
	{
		cost += moveOrAttack(owner, playerX, playerY);
	}

	if (cost != 0)
	{
		// Player might have moved.
		engine.getCurrentStage()->map->computeFov();

		// Add some player scent to the current tiles.
		engine.getCurrentStage()->scents.push(new Scent(0, owner->x, owner->y, PLAYER_STINK));
	}

	return cost;
}

int PlayerAI::handleActionKey(Actor *owner, int ascii)
{
	int cost = 0;
	Actor *itemActor = NULL;

	switch (ascii)
	{
	case 'g': // pickup item under owner
		tryGetItem(owner);
		cost = COST_GET_ITEM;
		break;
	case 'i': // view the inventory
		itemActor = chooseFromInventory(owner);
		if (itemActor != NULL)
		{
			itemActor->pickable->use(itemActor, owner);
			cost = COST_GET_ITEM;
		}
		break;
	case 'd':
		itemActor = chooseFromInventory(owner);
		if (itemActor)
		{
			itemActor->pickable->drop(itemActor, owner);
			cost = COST_GET_ITEM;
		}
		break;
	}

	Stage *currentStage = engine.getCurrentStage();
	for (Actor **iter = currentStage->actors.begin(); iter != currentStage->actors.end(); iter++)
	{
		Actor *actor = *iter;
		if ((actor != owner) && (actor->ai) && (actor->x == owner->x) && (actor->y == owner->y))
		{
			cost += actor->ai->handleActionKey(actor, ascii);
		}
	}

	return cost;
}

void PlayerAI::tryGetItem(Actor *owner)
{
	Stage *currentStage = engine.getCurrentStage();
	bool found = false;
	for (Actor **iter = currentStage->actors.begin(); iter != currentStage->actors.end(); iter++)
	{
		Actor *actor = *iter;
		if (actor->pickable && (actor->x == owner->x) && (actor->y == owner->y))
		{
			if (actor->pickable->pick(actor, owner))
			{
				found = true;
				engine.ui->message(TCODColor::lightGrey, "You pick up the %s.", actor->name);
				break;
			}
			else if (!found)
			{
				found = true;
				engine.ui->message(TCODColor::red, "Your inventory is full.");
				break;
			}
		}
	}
	if (!found)
	{
		engine.ui->message(TCODColor::lightGrey, "There's nothing here that you can pick up.");
	}
}

Actor *PlayerAI::chooseFromInventory(Actor *owner)
{
	static const int INVENTORY_WIDTH = 50;
	static const int INVENTORY_HEIGHT = 28;
	static TCODConsole con(INVENTORY_WIDTH, INVENTORY_HEIGHT);
	
	// Display the inventory frame.
	con.setDefaultForeground(TCODColor(200, 180, 50));
	con.printFrame(0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, "inventory");

	// Display the items with their keyboard shortcut.
	con.setDefaultForeground(TCODColor::white);
	int shortcut = 'a';
	int y = 1;
	for (Actor **iter = owner->container->inventory.begin(); iter != owner->container->inventory.end(); iter++)
	{
		Actor *actor = *iter;
		con.print(2, y, "(%c) %s", shortcut, actor->name);
		y++;
		shortcut++;
	}

	// Blit the inventory console on the root console.
	TCODConsole::blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, TCODConsole::root, (engine.screenWidth - INVENTORY_WIDTH) / 2, (engine.screenHeight - INVENTORY_HEIGHT) / 2, 1.0f, 0.5f);
	TCODConsole::flush();

	// Wait for a key press.
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
	if (key.vk == TCODK_CHAR)
	{
		int actorIndex = key.c - 'a';
		if ((actorIndex >= 0) && (actorIndex < owner->container->inventory.size()))
		{
			return owner->container->inventory.get(actorIndex);
		}
	}
	return NULL;
}

int PlayerAI::moveOrAttack(Actor *owner, int targetX, int targetY)
{
	Stage *currentStage = engine.getCurrentStage();
	if (currentStage->map->isWall(targetX, targetY))
	{
		return COST_MOVE_FAIL;
	}

	for (Actor **iterator = currentStage->actors.begin(); iterator != currentStage->actors.end(); iterator++)
	{
		Actor *actor = *iterator;
		if ((actor->x == targetX) && (actor->y == targetY))
		{
			if (actor->destructible && !actor->destructible->isDead())
			{
				owner->attacker->attack(owner, targetX, targetY);
				return COST_ATTACK;
			}
			else if (actor->blocks)
			{
				// TODO: actor->interacts?
				return COST_MOVE_FAIL;
			}
		}
	}

	for (Actor **iterator = currentStage->actors.begin(); iterator != currentStage->actors.end(); iterator++)
	{
		Actor *actor = *iterator;
		if (((actor->destructible && actor->destructible->isDead()) || actor->pickable) && (actor->x == targetX) && (actor->y == targetY))
		{
			engine.ui->message(TCODColor::lightGrey, "There's a %s here.\n", actor->name);
		}
	}

	owner->x = targetX;
	owner->y = targetY;
	return COST_MOVE;
}

int PlayerAI::getNextLevelXP() const
{
	return LEVEL_UP_BASE + xpLevel * LEVEL_UP_FACTOR;
}