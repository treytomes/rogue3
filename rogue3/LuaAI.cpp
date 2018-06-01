#include "stdafx.h"
#include "LuaAI.h"
#include <lualib.h>
#include <lauxlib.h>
#include "Engine.h"

LuaAI::LuaAI()
{
	// All Lua contexts are held in this structure. We work with it almost all the time.
	L = luaL_newstate();
	luaL_openlibs(L); // load Lua libraries

	//
	// Begin settings constants.
	//
	lua_pushnumber(L, COST_MOVE);
	lua_setglobal(L, "COST_MOVE");

	lua_pushnumber(L, COST_WAIT);
	lua_setglobal(L, "COST_WAIT");

	lua_pushnumber(L, COST_MOVE_FAIL);
	lua_setglobal(L, "COST_MOVE_FAIL");

	lua_pushnumber(L, COST_ATTACK);
	lua_setglobal(L, "COST_ATTACK");

	lua_pushnumber(L, COST_GET_ITEM);
	lua_setglobal(L, "COST_GET_ITEM");

	lua_pushnumber(L, TRACKING_TURNS);
	lua_setglobal(L, "TRACKING_TURNS");
	//
	// Done setting constants.
	//

	//
	// Begin building the LuaAI metatable.
	//
	luaL_newmetatable(L, "LuaAI");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_message);
	lua_setfield(L, -2, "message");
	lua_pushcfunction(L, l_can_walk);
	lua_setfield(L, -2, "can_walk");
	lua_pushcfunction(L, l_is_in_fov);
	lua_setfield(L, -2, "is_in_fov");
	lua_pushcfunction(L, l_get_player_position);
	lua_setfield(L, -2, "get_player_position");
	//
	// Done building the LuaAI metatable.
	//
	
	//
	// Begin building the Actor metatable.
	//
	luaL_newmetatable(L, "Actor");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_is_destructible);
	lua_setfield(L, -2, "is_destructible");
	lua_pushcfunction(L, l_is_dead);
	lua_setfield(L, -2, "is_dead");
	lua_pushcfunction(L, l_move_by);
	lua_setfield(L, -2, "move_by");
	lua_pushcfunction(L, l_is_attacker);
	lua_setfield(L, -2, "is_attacker");
	lua_pushcfunction(L, l_attack);
	lua_setfield(L, -2, "attack");
	lua_pushcfunction(L, l_get_position);
	lua_setfield(L, -2, "get_position");
	//
	// Done building the Actor metatable.
	//

	//
	// Begin building the TCODZip metatable.
	//
	luaL_newmetatable(L, "TCODZip");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_put_int);
	lua_setfield(L, -2, "put_int");
	lua_pushcfunction(L, l_get_int);
	lua_setfield(L, -2, "get_int");
	//
	// Done building the TCODZip metatable.
	//

	LuaAI **ud = static_cast<LuaAI **>(lua_newuserdata(L, sizeof(LuaAI *)));
	*(ud) = this;

	// Equivalent of: luaL_setmetatable(L, "LuaAI");
	luaL_getmetatable(L, "LuaAI"); // set userdata metatable
	lua_setmetatable(L, -2);
	lua_setglobal(L, "this"); // this in lua points to the new userdata

	////lua_settop(L, 0);
	//lua_newtable(L); // We will pass a table to the script.  (I think the table is at the top of the stack now?)

	///*
	// * To put values into the table, we first push the index, then the
	// * value, and then call lua_rawset() with the index of the table in the
	// * stack. Let's see why it's -3: In Lua, the value -1 always refers to
	// * the top of the stack. When you create the table with lua_newtable(),
	// * the table gets pushed into the top of the stack. When you push the
	// * index and then the cell value, the stack looks like:
	// *
	// * <- [stack bottom] -- table, index, value [top]
	// *
	// * So the -1 will refer to the cell value, thus -3 is used to refer to
	// * the table itself. Note that lua_rawset() pops the two last elements
	// * of the stack, so that after it has been called, the table is at the
	// * top of the stack.
	// */
	//for (int i = 1; i <= 5; i++)
	//{
	//	lua_pushnumber(L, i); // Push the table index.
	//	lua_pushnumber(L, i * 2); // Push the cell value.
	//	lua_rawset(L, -3); // Stores the pair in the table.
	//}

	//// By what name is the script going to reference our table?
	//lua_setglobal(L, "foo");

	//// Get the returned value at the top of the stack (index -1).
	//double sum = lua_tonumber(L, -1);
	//printf("Script returned: %.0f\n", sum);

	//lua_pop(L, 1); // Take the returned value out of the stack.
}

LuaAI::~LuaAI()
{
	if (aiScriptPath)
	{
		delete aiScriptPath;
		aiScriptPath = NULL;
	}

	// Destroy the lua context.
	lua_close(L);
}

void LuaAI::load_script(const char *aiScriptPath)
{
	this->aiScriptPath = strdup(aiScriptPath);

	// Load the file containing the script we are going to run.
	int status = luaL_loadfile(L, aiScriptPath);
	if (status)
	{
		// If something went wrong, error message is at the top of the stack.
		fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
	}

	// Ask Lua to run our script.
	status = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (status)
	{
		fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
	}
}

void LuaAI::load(TCODZip &zip)
{
	load_script(zip.getString());

	lua_getglobal(L, "load");

	TCODZip **ud = static_cast<TCODZip **>(lua_newuserdata(L, sizeof(TCODZip *)));
	*(ud) = &zip;
	luaL_getmetatable(L, "TCODZip"); // set userdata metatable
	lua_setmetatable(L, -2);

	int result = lua_pcall(L, 1, 0, 0);
	if (result)
	{
		fprintf(stderr, "Call to LuaAI:load failed: %s\n", lua_tostring(L, -1));
	}
}

void LuaAI::save(TCODZip &zip)
{
	zip.putInt(LUA);
	zip.putString(aiScriptPath);

	lua_getglobal(L, "save");

	TCODZip **ud = static_cast<TCODZip **>(lua_newuserdata(L, sizeof(TCODZip *)));
	*(ud) = &zip;
	luaL_getmetatable(L, "TCODZip"); // set userdata metatable
	lua_setmetatable(L, -2);

	int result = lua_pcall(L, 1, 0, 0);
	if (result)
	{
		fprintf(stderr, "Call to LuaAI:save failed: %s\n", lua_tostring(L, -1));
	}
}

int LuaAI::update(Actor *owner)
{
	lua_getglobal(L, "update");

	Actor **ud = static_cast<Actor **>(lua_newuserdata(L, sizeof(Actor *)));
	*(ud) = owner;

	luaL_getmetatable(L, "Actor"); // set userdata metatable
	lua_setmetatable(L, -2);

	int result = lua_pcall(L, 1, 1, 0);
	if (result)
	{
		fprintf(stderr, "Call to LuaAI:update failed: %s\n", lua_tostring(L, -1));
	}

	int returnCost = lua_tonumber(L, -1);
	lua_pop(L, 1); // Take the returned value out of the stack.

	return returnCost;
}

int LuaAI::l_message(lua_State *L)
{
	bool foundError = false;
	TCODColor color = TCODColor::lightGrey;
	const char *text = NULL;

	// Get the number of arguments.
	int numArguments = lua_gettop(L);

	if (numArguments == 2)
	{
		// Get the "this" pointer.
		LuaAI **ai = static_cast<LuaAI **>(luaL_checkudata(L, 1, "LuaAI"));

		// Get the text.
		text = lua_tostring(L, 2);
	}
	//else if (numArguments == 2)
	//{
	//	// Get the color.
	//	lua_checktype(L, 1, LUA_TABLE);
	//	if (lua_isnumber(L, 1))
	//	{
	//		text = lua_tostring(L, 1);
	//	}
	//	else
	//	{
	//		foundError = true;
	//	}

	//	// Get the text.
	//	if (lua_isnumber(L, 2))
	//	{
	//		text = lua_tostring(L, 2);
	//	}
	//	else
	//	{
	//		foundError = true;
	//	}
	//}
	else
	{
		foundError = true;
	}

	if (foundError)
	{
		engine.ui->message(TCODColor::orange, "Syntax error: LuaAI:message([color], text)");
	}

	engine.ui->message(color, text);

	return 0;
}

int LuaAI::l_can_walk(lua_State *L)
{
	int numArguments = lua_gettop(L);
	if (numArguments == 3)
	{
		// Get the "this" pointer.
		LuaAI **ai = static_cast<LuaAI **>(luaL_checkudata(L, 1, "LuaAI"));

		// Get the position.
		int x = lua_tointeger(L, 2);
		int y = lua_tointeger(L, 3);
		bool canWalk = engine.getCurrentStage()->map->canWalk(x, y);
		lua_pushboolean(L, canWalk);
		return 1;
	}
	else
	{
		engine.ui->message(TCODColor::orange, "Syntax error: LuaAI:can_walk(x, y)");
		return 0;
	}
}

int LuaAI::l_is_in_fov(lua_State *L)
{
	int numArguments = lua_gettop(L);
	if (numArguments == 3)
	{
		// Get the "this" pointer.
		LuaAI **ai = static_cast<LuaAI **>(luaL_checkudata(L, 1, "LuaAI"));

		// Get the position.
		int x = lua_tointeger(L, 2);
		int y = lua_tointeger(L, 3);
		bool isInFov = engine.getCurrentStage()->map->isInFov(x, y);
		lua_pushboolean(L, isInFov);
		return 1;
	}
	else
	{
		engine.ui->message(TCODColor::orange, "Syntax error: LuaAI:is_in_fov(x, y)");
		return 0;
	}
}

int LuaAI::l_get_player_position(lua_State *L)
{
	int numArguments = lua_gettop(L);
	if (numArguments == 1)
	{
		// Get the "this" pointer.
		LuaAI **ai = static_cast<LuaAI **>(luaL_checkudata(L, 1, "LuaAI"));

		// Return the player's position.
		lua_pushinteger(L, engine.player->getX());
		lua_pushinteger(L, engine.player->getY());
		return 2;
	}
	else
	{
		engine.ui->message(TCODColor::orange, "Syntax error: LuaAI:get_player_position()");
		return 0;
	}
}

int LuaAI::l_is_destructible(lua_State *L)
{
	int numArguments = lua_gettop(L);
	if (numArguments == 1)
	{
		// Get the "owner" pointer.
		Actor **owner = static_cast<Actor **>(luaL_checkudata(L, 1, "Actor"));
		bool isDestructible = (*owner)->destructible != NULL;
		lua_pushboolean(L, isDestructible);
		return 1;
	}
	else
	{
		engine.ui->message(TCODColor::orange, "Syntax error: Actor:is_destructible()");
		return 0;
	}
}

int LuaAI::l_is_dead(lua_State *L)
{
	int numArguments = lua_gettop(L);
	if (numArguments == 1)
	{
		// Get the "owner" pointer.
		Actor **owner = static_cast<Actor **>(luaL_checkudata(L, 1, "Actor"));
		bool isDead = (*owner)->destructible->isDead();
		lua_pushboolean(L, isDead);
		return 1;
	}
	else
	{
		engine.ui->message(TCODColor::orange, "Syntax error: Actor:is_dead()");
		return 0;
	}
}

int LuaAI::l_move_by(lua_State *L)
{
	int numArguments = lua_gettop(L);
	if (numArguments == 3)
	{
		// Get the "owner" pointer.
		Actor **owner = static_cast<Actor **>(luaL_checkudata(L, 1, "Actor"));

		// Get the position.
		int dx = lua_tointeger(L, 2);
		int dy = lua_tointeger(L, 3);
		(*owner)->moveBy(dx, dy);
		return 0;
	}
	else
	{
		engine.ui->message(TCODColor::orange, "Syntax error: Actor:move_by(dx, dy)");
		return 0;
	}
}

int LuaAI::l_is_attacker(lua_State *L)
{
	int numArguments = lua_gettop(L);
	if (numArguments == 1)
	{
		// Get the "owner" pointer.
		Actor **owner = static_cast<Actor **>(luaL_checkudata(L, 1, "Actor"));
		bool isAttacker = (*owner)->attacker != NULL;
		lua_pushboolean(L, isAttacker);
		return 1;
	}
	else
	{
		engine.ui->message(TCODColor::orange, "Syntax error: Actor:is_attacker()");
		return 0;
	}
}

int LuaAI::l_attack(lua_State *L)
{
	int numArguments = lua_gettop(L);
	if (numArguments == 3)
	{
		// Get the "owner" pointer.
		Actor **owner = static_cast<Actor **>(luaL_checkudata(L, 1, "Actor"));
		
		// Get the position.
		int targetX = lua_tointeger(L, 2);
		int targetY = lua_tointeger(L, 3);

		(*owner)->attacker->attack(*owner, targetX, targetY);
		return 0;
	}
	else
	{
		engine.ui->message(TCODColor::orange, "Syntax error: Actor:attack(target_x, target_y)");
		return 0;
	}
}

int LuaAI::l_get_position(lua_State *L)
{
	int numArguments = lua_gettop(L);
	if (numArguments == 1)
	{
		// Get the "owner" pointer.
		Actor **owner = static_cast<Actor **>(luaL_checkudata(L, 1, "Actor"));

		// Return the owner's position.
		lua_pushinteger(L, (*owner)->getX());
		lua_pushinteger(L, (*owner)->getY());
		return 2;
	}
	else
	{
		engine.ui->message(TCODColor::orange, "Syntax error: Actor:get_position()");
		return 0;
	}
}

int LuaAI::l_put_int(lua_State *L)
{
	int numArguments = lua_gettop(L);
	if (numArguments == 2)
	{
		TCODZip **zip = static_cast<TCODZip **>(luaL_checkudata(L, 1, "TCODZip"));
		(*zip)->putInt(lua_tointeger(L, 2));
		return 0;
	}
	else
	{
		engine.ui->message(TCODColor::orange, "Syntax error: TCODZip:put_int()");
		return 0;
	}
}

int LuaAI::l_get_int(lua_State *L)
{
	int numArguments = lua_gettop(L);
	if (numArguments == 1)
	{
		TCODZip **zip = static_cast<TCODZip **>(luaL_checkudata(L, 1, "TCODZip"));
		lua_pushinteger(L, (*zip)->getInt());
		return 1;
	}
	else
	{
		engine.ui->message(TCODColor::orange, "Syntax error: TCODZip:get_int()");
		return 0;
	}
}
