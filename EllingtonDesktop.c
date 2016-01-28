/*
 * Ellington Desktop
 * =================
 * 
 * To do
 * -----
 * - Split up parser. Handle tags, especially images.
 * - Panel applets: applications menu, windows list/workspace switcher, clock, status tray, log out.
 * - Full-screen.
 * - Extra workspace actions (e.g. copy and paste).
 * - Problem cases: Firefox tooltips, xfce4-taskmanager...
 */

#include <stdbool.h>

bool globalQuit = false;

#include "WindowManager/WM.h"

int main(int numArgs, const char** argList)
{
	// Parse command-line arguments
	const char* displayID = numArgs > 1? argList[1] : ":0";
	
	// Run the window manager
	wmSession session = WM.open(displayID);
	
	while(!globalQuit)
		{ session = WM.wait(session); }
	
	WM.close(session);
	return 0;
}
