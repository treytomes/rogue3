#pragma once

#include "AI.h"
#include <lua.hpp>

class LuaAI : public AI
{
public:
	LuaAI();
	~LuaAI();

	void load_script(const char *aiScriptPath);
	void load(TCODZip &zip);
	void save(TCODZip &zip);

	int update(Actor *owner);

protected:
	char *aiScriptPath;
	lua_State * L;

	// Functions for "this" (LuaAI).
	static int l_message(lua_State *L);
	static int l_can_walk(lua_State *L);
	static int l_is_in_fov(lua_State *L);
	static int l_get_player_position(lua_State *L);

	// Functions for "owner" (Actor).
	static int l_is_destructible(lua_State *L);
	static int l_is_dead(lua_State *L);
	static int l_move_by(lua_State *L);
	static int l_is_attacker(lua_State *L);
	static int l_attack(lua_State *L);
	static int l_get_position(lua_State *L);

	// Functions for "zip" (TCODZip).
	static int l_put_int(lua_State *L);
	static int l_get_int(lua_State *L);
};

