#include "UIToolkit/UI.h"
#include <stdio.h>
#include <unistd.h>

int main()
{
	xWindow root = X.open(":0");
	xWindow win = X.window(root, xywhBox(100, 100, 200, 300));
	
	printf("Window ID %lui (%i) on root %lui\n", win.id, win.attributes.map_state, root.id);
	
	sleep(1);
	X.close(win);

	xWindow panel = X.panel(root, xywhBox(100, 100, 200, 300));
	printf("Panel ID %lui (%i) on root %lui\n", win.id, win.attributes.map_state, root.id);
	
	sleep(1);
	X.close(panel);	
	X.close(root);
	
	uiWindow ui = UI.open(300, 300, 300, 200);
	UI.draw(ui, "Hello!");
	
	sleep(1);
	UI.close(ui);
	
	return 0;
}
