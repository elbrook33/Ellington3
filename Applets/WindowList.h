#ifndef WINDOWLIST_H
#define WINDOWLIST_H

void windowList(uiWindow menu)
{
	char markup[1000] = 
		"*Desktop 1*\n"
		"Firefox\n"
		"Geany\n"
		"XTerm XYZ\n"
		"\\\n"
		"*Desktop 2*\n"
		"Firefox\n"
		"\\\n"
		"*Desktop 3*\n"
		"Firefox\n"
		"Geany\n"
		"XTerm XYZ\n"
		"\\\n"
		"*Desktop 4*\n"
		"Firefox\n"
		"\\\n"
		"*Desktop 5*\n"
		"Firefox\n"
		"Geany\n"
		"XTerm XYZ\n"
		"\\\n"
		"*Desktop 6*\n"
		"Firefox\n"
		"\\\n"
		"XTerm XYZ";
	
	menu = UI.colour(menu, Theme.highlightFg(), Theme.highlightBg());
	menu = UI.highlightStyle(menu, Theme.highlightLeft(), Theme.fg());
	menu = UI.draw(menu, markup);
	
	while(true)
	{
		menu = UI.wait(menu);
		switch(menu.event)
		{
			case MotionNotify:
				UI.highlight(menu);
				break;
			case ButtonPress:
				if(UI.buttonIs(menu, 4)) { menu = UI.scroll(menu, 1); }
				if(UI.buttonIs(menu, 5)) { menu = UI.scroll(menu, -1); }
				break;
			case LeaveNotify:
				UI.close(menu);
				return;
		}
	}
}

#endif
