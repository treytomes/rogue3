#pragma once

#define WINDOW_TITLE "rogue3"
#define TERMINAL_WIDTH 80
#define TERMINAL_HEIGHT 50
#define IS_FULLSCREEN false

#define MAP_WIDTH	64
#define MAP_HEIGHT	64

// Setting INITIAL_FOV to 0 will make the field-of-view infinite.
#define INITIAL_FOV 10

#define MAX_NAME_LENGTH 32

// Action Costs
static const int COST_MOVE = 256;
static const int COST_WAIT = COST_MOVE / 2;
static const int COST_MOVE_FAIL = COST_MOVE + COST_WAIT;
static const int COST_ATTACK = COST_MOVE / 2;
static const int COST_GET_ITEM = COST_MOVE * 2;

// How many turns the monster chases the player after losing sight of him.
#define TRACKING_TURNS 3