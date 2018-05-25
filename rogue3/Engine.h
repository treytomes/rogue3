#pragma once

#include "Constants.h"

#include "Actor.h"
#include "Map.h"
#include "UIManager.h"
#include "ParticleSystem.h"
#include "Stage.h"

class Engine
{
public:
	enum GameStatus {
		STARTUP,	// First frame of the game
		IDLE,		// No new turn.  Redraw the same screen.
		NEW_TURN,	// Update the monsters positions.
		VICTORY,	// The player won.
		DEFEAT		// The player was killed.
	} gameStatus;

	ParticleSystem *particles;
	Actor *player;
	TCODList<Stage *> stages;
	int currentStageIndex;

	bool isPlaying;
	int fovRadius;
	int gameTimeInTurns;
	int screenWidth;
	int screenHeight;
	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;

	UIManager *ui;

	Engine(int screenWidth, int screenHeight);
	~Engine();

	void initialize();
	void terminate(); // end the current game
	void load();
	void save();

	// Handle player movement, etc.
	void update();
	void render();
	void sendToBack(Actor *actor);
	Actor *getClosestMonster(int x, int y, float range = 0) const; // get the closest monster to a position, up to a certain range
	Actor *getActor(int x, int y) const; // get the first actor at this position
	Actor *getDestructibleActor(int x, int y) const; // get a destructible actor at this position
	bool pickATile(int *x, int *y, float maxRange = 0.0f);

	// Count the number of items (actor->pickable != NULL) at a position.
	int countItems(int x, int y) const;

	inline Stage *getCurrentStage() const;
	void gotoNextStage(int targetX, int targetY);
	void gotoPreviousStage(int targetX, int targetY);

private:
	//bool computerFov;
};

extern Engine engine;