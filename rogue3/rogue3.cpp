// rogue3.cpp : Defines the entry point for the program.
//

#include "stdafx.h"

#include "Engine.h"

// Defined in Engine.h; this is the state of game while the user is playing.
Engine engine(TERMINAL_WIDTH, TERMINAL_HEIGHT);

int main(int argc, char *argv[])
{
	engine.load();
	while (!TCODConsole::isWindowClosed() && engine.isPlaying)
	{
		engine.update();
		engine.render();
		TCODConsole::flush();
	}
	engine.save();
	return 0;
}
