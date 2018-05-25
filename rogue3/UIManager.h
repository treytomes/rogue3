#pragma once

#define HUDPANEL_HEIGHT	7
#define BAR_WIDTH		20
#define SIDEPANEL_WIDTH	22

#define MSG_X		(BAR_WIDTH + 2)
#define MSG_HEIGHT	(HUDPANEL_HEIGHT - 1)

class Menu
{
public:
	~Menu();
	void clear();
	void addItem(int code, const char *label);
	int pick(int defaultCode = -1); // defaultCode will be returned if Escape is pressed.
protected:
	struct MenuItem
	{
		int code;
		const char *label;
	};
	TCODList<MenuItem *> items;
};

class UIManager : public Persistent
{
public:
	Menu menu;
	
	UIManager();
	~UIManager();

	void clear();
	void render();
	void message(const TCODColor &col, const char *text, ...);
	void load(TCODZip &zip);
	void save(TCODZip &zip);

protected:
	TCODConsole *hudPanel;
	TCODConsole *sidePanel;

	struct Message
	{
		char *text;
		TCODColor color;
		Message(const char *text, const TCODColor &color);
		~Message();
	};
	TCODList<Message *> log;

	void renderBar(TCODConsole *target, int x, int y, int width, const char *name, int value, int maxValue, const TCODColor &barColor, const TCODColor &backColor);
	void renderMouseLook();
};
