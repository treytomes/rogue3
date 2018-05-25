#include "stdafx.h"
#include "UIManager.h"

#include "Engine.h"
#include <stdarg.h>

Menu::~Menu()
{
	clear();
}

void Menu::clear()
{
	items.clearAndDelete();
}

void Menu::addItem(int code, const char *label)
{
	MenuItem *item = new MenuItem();
	item->code = code;
	item->label = label;
	items.push(item);
}

int Menu::pick(int defaultCode) {
	static TCODImage img("menu_background1.png");
	int selectedItem = 0;
	while (!TCODConsole::isWindowClosed())
	{
		img.blit2x(TCODConsole::root, 0, 0);

		int currentItem = 0;
		for (MenuItem **iter = items.begin(); iter != items.end(); iter++)
		{
			if (currentItem == selectedItem)
			{
				TCODConsole::root->setDefaultForeground(TCODColor::lighterOrange);
			}
			else
			{
				TCODConsole::root->setDefaultForeground(TCODColor::lightGrey);
			}
			TCODConsole::root->print(10, 10 + currentItem * 3, (*iter)->label);
			currentItem++;
		}

		TCODConsole::flush();

		// check key presses
		TCOD_key_t key;
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL);
		switch (key.vk)
		{
		case TCODK_UP:
			selectedItem--;
			if (selectedItem < 0)
			{
				selectedItem = items.size() - 1;
			}
			break;
		case TCODK_DOWN:
			selectedItem = (selectedItem + 1) % items.size();
			break;
		case TCODK_ENTER:
			return items.get(selectedItem)->code;
		case TCODK_ESCAPE:
			return defaultCode;
		default:
			break;
		}
	}

	return defaultCode;
}

UIManager::UIManager()
{
	hudPanel = new TCODConsole(engine.screenWidth, HUDPANEL_HEIGHT);
	sidePanel = new TCODConsole(SIDEPANEL_WIDTH, engine.screenHeight - HUDPANEL_HEIGHT);
}

UIManager::~UIManager()
{
	delete hudPanel;
	delete sidePanel;
	clear();
}

void UIManager::clear()
{
	log.clearAndDelete();
}

void UIManager::load(TCODZip &zip)
{
	int numMessages = zip.getInt();
	while (numMessages > 0)
	{
		// Text pointer will be deleted once <zip> is destroyed, but the message function will make a copy for us.
		const char *text = zip.getString();
		TCODColor color = zip.getColor();

		Message *msg = new Message(text, color);
		log.push(msg);

		numMessages--;
	}
}

void UIManager::save(TCODZip &zip)
{
	zip.putInt(log.size());
	for (Message **iter = log.begin(); iter != log.end(); iter++)
	{
		zip.putString((*iter)->text);
		zip.putColor(&(*iter)->color);
	}
}

void UIManager::render()
{
	// Clear the HUD console.
	hudPanel->setDefaultBackground(TCODColor::black);
	hudPanel->clear();

	// Draw the health bar.
	renderBar(hudPanel, 1, 1, BAR_WIDTH, "HP", engine.player->destructible->health, engine.player->destructible->maxHealth, TCODColor::lightRed, TCODColor::darkerRed);

	// Draw the number of turns.
	hudPanel->print(0, HUDPANEL_HEIGHT - 1, "# turns: %d", engine.gameTimeInTurns);

	// Draw the current dungeon level.
	hudPanel->print(3, 3, "Dungeon level: %d", engine.currentStageIndex); // Stage isn't necessarily the same as difficulty level.

	// Draw the message log.
	int y = 1;
	float colorCoef = 1.0f;
	for (Message **iter = log.begin(); iter != log.end(); iter++)
	{
		Message *message = *iter;
		hudPanel->setDefaultForeground(message->color * colorCoef);
		hudPanel->print(MSG_X, y, message->text);
		y++;
		if (colorCoef > 0.0f)
		{
			colorCoef -= .05f;
		}
	}

	renderMouseLook();

	// Blit the HUD panel on the root console.
	TCODConsole::blit(hudPanel, 0, 0, engine.screenWidth, HUDPANEL_HEIGHT, TCODConsole::root, 0, engine.screenHeight - HUDPANEL_HEIGHT);

	// Clear the side console.
	sidePanel->setDefaultBackground(TCODColor::black);
	sidePanel->clear();

	// Draw the enemy health bars.
	Stage *currentStage = engine.getCurrentStage();
	y = 1;
	for (Actor **iter = currentStage->actors.begin(); iter != currentStage->actors.end(); iter++)
	{
		Actor *actor = *iter;
		if (actor->destructible && !actor->destructible->isDead() && (actor != engine.player) && currentStage->map->isInFov(actor->x, actor->y))
		{
			renderBar(sidePanel, 1, y, BAR_WIDTH, actor->name, actor->destructible->health, actor->destructible->maxHealth, TCODColor::lightRed, TCODColor::darkerRed);
			y += 2;
		}
	}

	// Blit the side panel on the root console.
	TCODConsole::blit(sidePanel, 0, 0, SIDEPANEL_WIDTH, engine.screenHeight - HUDPANEL_HEIGHT, TCODConsole::root, engine.screenWidth - SIDEPANEL_WIDTH, 0);
}

void UIManager::renderBar(TCODConsole *target, int x, int y, int width, const char *name, int value, int maxValue, const TCODColor &barColor, const TCODColor &backColor)
{
	// Fill the background.
	target->setDefaultBackground(backColor);
	target->rect(x, y, width, 1, false, TCOD_BKGND_SET);

	int barWidth = (int)(value / (float)maxValue * width);
	if (barWidth > 0)
	{
		// Draw the bar.
		target->setDefaultBackground(barColor);
		target->rect(x, y, barWidth, 1, false, TCOD_BKGND_SET);
	}

	// Print text on top of the bar.
	target->setDefaultForeground(TCODColor::white);
	target->printEx(x + width / 2, y, TCOD_BKGND_NONE, TCOD_CENTER, "%s: %d/%d", name, value, maxValue);
}

void UIManager::renderMouseLook()
{
	int mapX = 0;
	int mapY = 0;
	int mapWidth = engine.screenWidth;
	int mapHeight = engine.screenHeight - HUDPANEL_HEIGHT;

	int offsetX = mapX + mapWidth / 2 - engine.player->x;
	int offsetY = mapY + mapHeight / 2 - engine.player->y;

	int mouseX = engine.mouse.cx - offsetX;
	int mouseY = engine.mouse.cy - offsetY;

	Stage *currentStage = engine.getCurrentStage();
	if (!currentStage->map->isInFov(mouseX, mouseY))
	{
		// Mouse is out of the player's field-of-view.
		return;
	}

	// Collect a comma-separated list of actor names.
	char buf[128] = "";
	bool first = true;
	for (Actor **iter = currentStage->actors.begin(); iter != currentStage->actors.end(); iter++)
	{
		Actor *actor = *iter;
		if ((actor->x == mouseX) && (actor->y == mouseY))
		{
			if (!first)
			{
				strcat(buf, ", ");
			}
			else
			{
				first = false;
			}
			strcat(buf, actor->name);
		}
	}

	Tile *mouseTile = currentStage->map->getTile(mouseX, mouseY);
	if (mouseTile != NULL)
	{
		if (!first)
		{
			strcat(buf, ", ");
		}
		strcat(buf, mouseTile->name);
	}

	// Display the list of actors under the mouse cursor.
	hudPanel->setDefaultForeground(TCODColor::lightGrey);
	hudPanel->print(1, 0, buf);
}

void UIManager::message(const TCODColor &color, const char *text, ...)
{
	// Build the text.
	va_list ap;
	char buf[128];
	va_start(ap, text);
	vsprintf(buf, text, ap);
	va_end(ap);

	char *lineBegin = buf;
	char *lineEnd;
	int lineNumber = 0;
	do
	{
		// Make room for the new message.
		if (log.size() == MSG_HEIGHT)
		{
			Message *toRemove = log.get(MSG_HEIGHT - 1);
			log.remove(toRemove);
			delete toRemove;
		}

		// Detect end of the line.
		lineEnd = strchr(lineBegin, '\n');
		if (lineEnd)
		{
			*lineEnd = '\0';
		}

		// Add a new message to the log.
		if (strlen(lineBegin) > 0)
		{
			Message *msg = new Message(lineBegin, color);
			log.insertBefore(msg, lineNumber);
			lineNumber++;
			printf("%s\n", lineBegin);
			//log.push(msg);
		}

		// Go to the next line;
		lineBegin = lineEnd + 1;
	} while (lineEnd);
}

UIManager::Message::Message(const char *text, const TCODColor &color)
	: text(strdup(text)), color(color)
{
}

UIManager::Message::~Message()
{
	free(text);
	text = NULL;
}