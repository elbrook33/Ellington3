#include "UIToolkit/UI.h"
#include "Applets/WindowList.h"

// Globals

char* panelLeftMenu = "*Applications* Windows	> *";
char* panelRightMenu = "* <	> *Exit* >";

bool quit = false;


// Functions

char* updateMarkup(char*);
void openWindowList(uiWindow, uiEvent, char* markup);


// Main

int main(int numArgs, const char** argList)
{	
	uiWindow panel = UI.get(0, 0, UI.fullWidth(), UI.barHeight);
	
	char markup[100];
	panel = UI.draw(panel, updateMarkup(markup));
	
	while(!quit)
	{
		panel = UI.wait(panel);
		switch(panel.event)
		{
			case ButtonPress:
				if(!UI.buttonIs(panel, 1)) { break; }
				if(UI.focusIs(panel, 1, 1, 1)) { system("xfce4-appfinder &"); }
				if(UI.focusIs(event, 1, 1, 2)) { openWindowList(panel, markup); }
				if(UI.focusIs(event, 1, 3, 1)) { quit = true; }
				break;
		}
	}
	
	return 0;
}


// Implementation

char* updateMarkup(char* storage)
{
	char date[20];
	memset(storage, 0, 100);
	
	strcat(storage, panelLeftMenu);
	strcat(storage, dateString(date, 20));
	strcat(storage, panelRightMenu);
	
	return storage;
}

void openWindowList(uiWindow panel, char* markup)
{
	panelLeftMenu = "*Applications* |Windows|	> *";
	UI.draw(panel, updateMarkup(markup));
	
	windowList(UI.popup(panel));
	
	panelLeftMenu = "*Applications* Windows	> *";
	UI.draw(panel, updateMarkup(markup));
}
