#include "stdafx.h"
#include "Engine.h"
#include "PlayerDestructible.h"
#include "PlayerAI.h"
#include "Attacker.h"
#include "LuaAI.h"
#include "Scent.h"

bool query_destructible(Actor *actor)
{
	return actor->destructible && !actor->destructible->isDead();
}

bool query_destructible(Actor *actor, int x, int y)
{
	return (actor->getX() == x) && (actor->getY() == y) && query_destructible(actor);
}

Engine::Engine(int screenWidth, int screenHeight)
	: screenWidth(screenWidth), screenHeight(screenHeight), fovRadius(INITIAL_FOV), gameStatus(STARTUP), gameTimeInTurns(0) //, computerFov(true)
{
	TCODConsole::initRoot(TERMINAL_WIDTH, TERMINAL_HEIGHT, WINDOW_TITLE, IS_FULLSCREEN);
	ui = new UIManager();
	particles = new ParticleSystem();

	//TCOD_sys_set_fps(60);
}

Engine::~Engine()
{
	terminate();
	delete particles;
	delete ui;
}

void Engine::initialize()
{
	player = new Actor(TERMINAL_WIDTH / 2, TERMINAL_HEIGHT / 2, '@', "player", TCODColor::white);
	player->speed = 4;
	player->destructible = new PlayerDestructible(30, 2, "your cadaver");
	player->ai = new PlayerAI();
	player->attacker = new Attacker(2); // TODO: Might reduce this to 1 after leveling is introduced.
	player->container = new Container(26);

	currentStageIndex = 0;
	Stage *stage = new Stage();
	stages.push(stage); // Need to push before initialize, otherwise the map builder won't know what the current stage is when placing actors.
	stage->initialize();
	// TODO: Maybe actors should be placed on stage outside of the map builder?  The map would need to contain more semantic information about it's internal structure.

	// Every time the stage changes, I will need to remove the player from the old stage and add it to the new one.
	// This will also be an issue when saving and loading.
	stage->actors.push(player);

	isPlaying = true;
	gameStatus = STARTUP;

	ui->message(TCODColor::lightRed, "Welcome stranger!\nPrepare to perish in the Tombs of the Ancient Kings.");
}

void Engine::terminate()
{
	lastKey = TCOD_key_t();

	stages.clearAndDelete();

	ui->clear();
}

enum MainMenuCode {
	NEW_GAME,
	CONTINUE,
	EXIT
};

void Engine::load()
{
	ui->menu.clear();
	ui->menu.addItem(MainMenuCode::NEW_GAME, "New game");
	if (TCODSystem::fileExists("game.sav"))
	{
		ui->menu.addItem(MainMenuCode::CONTINUE, "Continue");
	}
	ui->menu.addItem(MainMenuCode::EXIT, "Exit");
	MainMenuCode menuItem = (MainMenuCode)ui->menu.pick(Menu::MAIN, EXIT);

	if (menuItem == MainMenuCode::EXIT)
	{
		// Exit or window closed
		exit(0);
	}
	else if (menuItem == MainMenuCode::NEW_GAME)
	{
		// New game
		terminate();
		initialize();
	}
	else
	{
		terminate(); // clear out the old game

		TCODZip zip;
		zip.loadFromFile("game.sav");

		// First entry is the player.
		player = new Actor(0, 0, 0, NULL, TCODColor::white);
		player->load(zip);

		currentStageIndex = zip.getInt();
		int numStages = zip.getInt();
		while (numStages > 0)
		{
			Stage *stage = new Stage();
			stage->load(zip);
			stages.push(stage);
			numStages--;
		}

		// Add the player to the stage after the stage has been loaded.
		getCurrentStage()->actors.push(player);

		// Finally the message log.
		ui->load(zip);

		gameStatus = STARTUP;
		isPlaying = true;
	}
}

void Engine::save()
{
	if (player->destructible->isDead())
	{
		TCODSystem::deleteFile("game.sav");
	}
	else
	{
		TCODZip zip;

		player->save(zip);

		Stage *currentStage = getCurrentStage();
		// Be careful not to save the player to the stage.
		currentStage->actors.remove(player);

		zip.putInt(currentStageIndex);
		zip.putInt(stages.size());
		for (Stage **iter = stages.begin(); iter != stages.end(); iter++)
		{
			Stage *stage = *iter;
			stage->save(zip);
		}

		// Put the player back on the stage.
		currentStage->actors.push(player);
		
		// Finally the message log.
		ui->save(zip);
		zip.saveToFile("game.sav");
	}
}

void Engine::update()
{
	if (!isPlaying)
	{
		return;
	}

	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse);
	if (lastKey.vk == TCODK_ESCAPE)
	{
		save();
		load();
	}

	if (gameStatus == STARTUP)
	{
		gameStatus = NEW_TURN;
	}
	else if (gameStatus == IDLE)
	{
		getCurrentStage()->update();

		if (player->energy < 0)
		{
			player->energy += player->speed * 8;
		}
		if (player->energy >= 0)
		{
			gameStatus = NEW_TURN;
		}
	}
	else if (gameStatus == NEW_TURN)
	{
		player->update();
		if (player->energy < 0)
		{
			// The player made a move; we can stop waiting!
			gameTimeInTurns++;
			gameStatus = IDLE;
		}
	}
	else if (gameStatus == DEFEAT)
	{
		TCOD_key_t key;
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL);
		if (key.vk == TCODK_ESCAPE)
		{
			isPlaying = false;
		}
	}

	getCurrentStage()->map->computeFov();
	particles->update();
}

void Engine::render()
{
	TCODConsole::root->clear();

	int mapX = 0;
	int mapY = 0;
	int mapWidth = screenWidth;
	int mapHeight = screenHeight - HUDPANEL_HEIGHT;

	Stage *currentStage = getCurrentStage();

	// Draw the map.
	currentStage->map->render(player->getX(), player->getY(), mapX, mapY, mapWidth, mapHeight); // screenWidth, screenHeight - PANEL_HEIGHT);

																					  // Draw the actors.
	int offsetX = mapX + mapWidth / 2 - player->getX();
	int offsetY = mapY + mapHeight / 2 - player->getY();
	for (Actor **iter = currentStage->actors.begin(); iter != currentStage->actors.end(); iter++)
	{
		Actor *actor = *iter;
		if ((!actor->fovOnly && currentStage->map->isExplored(actor->getX(), actor->getY())) || currentStage->map->isInFov(actor->getX(), actor->getY()))
		{
			actor->render(offsetX, offsetY);
		}
	}

#ifdef RENDER_SCENTS
	// Draw the scents.
	for (Scent **iter = currentStage->scents.begin(); iter != currentStage->scents.end(); iter++)
	{
		Scent *scent = *iter;
		scent->render(offsetX, offsetY);
	}
#endif

	particles->render(TCODConsole::root, offsetX, offsetY);

	ui->render();
}

void Engine::sendToBack(Actor *actor)
{
	Stage *currentStage = getCurrentStage();
	currentStage->actors.remove(actor);
	currentStage->actors.insertBefore(actor, 0);
}

Actor *Engine::getClosestMonster(int x, int y, float range) const
{
	Actor *closest = NULL;
	float bestDistance = 1E6f; // will a map ever be bigger than this???
	Stage *currentStage = getCurrentStage();
	for (Actor **iter = currentStage->actors.begin(); iter != currentStage->actors.end(); iter++)
	{
		Actor *actor = *iter;
		if ((actor != player) && query_destructible(actor))
		{
			float distance = actor->getDistance(x, y);
			if ((distance < bestDistance) && ((range == 0.0f) || (distance <= range)))
			{
				bestDistance = distance;
				closest = actor;
			}
		}
	}
	return closest;
}

Actor *Engine::getActor(int x, int y) const
{
	Stage *currentStage = getCurrentStage();
	for (Actor **iter = currentStage->actors.begin(); iter != currentStage->actors.end(); iter++)
	{
		Actor *actor = *iter;
		if ((actor->getX() == x) && (actor->getY() == y))
		{
			return actor;
		}
	}
	return NULL;
}

Actor *Engine::getDestructibleActor(int x, int y) const
{
	Stage *currentStage = getCurrentStage();
	for (Actor **iter = currentStage->actors.begin(); iter != currentStage->actors.end(); iter++)
	{
		Actor *actor = *iter;
		if (query_destructible(actor, x, y))
		{
			return actor;
		}
	}
	return NULL;
}

int Engine::countItems(int x, int y) const
{
	int numItems = 0;
	Stage *currentStage = getCurrentStage();
	for (Actor **iter = currentStage->actors.begin(); iter != currentStage->actors.end(); iter++)
	{
		Actor *actor = *iter;
		if ((actor->getX() == x) && (actor->getY() == y) && actor->pickable)
		{
			numItems++;
		}
	}
	return numItems;
}

bool Engine::pickATile(int *x, int *y, float maxRange)
{
	int mapX = 0;
	int mapY = 0;
	int mapWidth = screenWidth;
	int mapHeight = screenHeight - HUDPANEL_HEIGHT;
	int offsetX = mapX + mapWidth / 2 - player->getX();
	int offsetY = mapY + mapHeight / 2 - player->getY();
	Stage *currentStage = getCurrentStage();

	while (!TCODConsole::isWindowClosed())
	{
		render();

		// Highlight the possible range.
		for (int cx = 0; cx < currentStage->map->width; cx++)
		{
			for (int cy = 0; cy < currentStage->map->height; cy++)
			{
				if (currentStage->map->isInFov(cx, cy) && ((maxRange == 0) || (player->getDistance(cx, cy) <= maxRange)))
				{
					int renderX = cx + offsetX;
					int renderY = cy + offsetY;
					TCODConsole::root->setCharBackground(renderX, renderY, TCODConsole::root->getCharForeground(renderX, renderY) * 1.2);
					TCODConsole::root->setCharForeground(renderX, renderY, TCODConsole::root->getCharBackground(renderX, renderY) * 1.2);
				}
			}
		}

		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse);

		int mapMouseX = mouse.cx - offsetX;
		int mapMouseY = mouse.cy - offsetY;
		if (currentStage->map->isInFov(mapMouseX, mapMouseY) && (maxRange == 0 || player->getDistance(mapMouseX, mapMouseY) <= maxRange))
		{
			TCODConsole::root->setCharBackground(mouse.cx, mouse.cy, TCODColor::white);
		}

		if (mouse.lbutton_pressed) {
			*x = mapMouseX;
			*y = mapMouseY;
			return true;
		}

		if (mouse.rbutton_pressed || lastKey.vk != TCODK_NONE) {
			return false;
		}

		TCODConsole::flush();
	}

	return false;
}

Stage *Engine::getCurrentStage() const
{
	return stages.get(currentStageIndex);
}

void Engine::gotoNextStage(int targetX, int targetY)
{
	getCurrentStage()->actors.remove(player);

	engine.ui->message(TCODColor::lightGrey, "Trying to go down...");
	currentStageIndex++;
	if (currentStageIndex >= stages.size())
	{
		// Mostly duplicated from Engine::initialize.  Probably need a central factory method for this.
		Stage *stage = new Stage();
		stages.push(stage); // Need to push before initialize, otherwise the map builder won't know what the current stage is when placing actors.
		stage->initialize(targetX, targetY);
		// TODO: Maybe actors should be placed on stage outside of the map builder?  The map would need to contain more semantic information about it's internal structure.

		ui->message(TCODColor::lightViolet, "You take a moment to rest, and recover your strength.");
		player->destructible->heal(player->destructible->maxHealth / 2);
	}
	// Else, the stage has already been defined.

	// Every time the stage changes, I will need to remove the player from the old stage and add it to the new one.
	// This will also be an issue when saving and loading.
	getCurrentStage()->actors.push(player);
	// But where do I put the player?
	player->moveTo(targetX, targetY);
	//getCurrentStage()->map->computeFov();
	ui->message(TCODColor::red, "After a rare moment of peace, you descent\ndeeper into the heart of the dungeon...");
}

void Engine::gotoPreviousStage(int targetX, int targetY)
{
	ui->message(TCODColor::lightGrey, "Trying to go up...");
	if (currentStageIndex == 0)
	{
		ui->message(TCODColor::lightGrey, "You're already at the top!");
	}
	else
	{
		getCurrentStage()->actors.remove(player);

		currentStageIndex--;

		getCurrentStage()->actors.push(player);
		player->moveTo(targetX, targetY);
		//getCurrentStage()->map->computeFov();
		ui->message(TCODColor::orange, "Poking your head up from the stairs you wonder to yourself,\nhave you seen this place before?");
	}
}
