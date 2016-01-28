#ifndef WALLPAPER_H
#define WALLPAPER_H

#include "UIToolkit/UIToolkit.h"


// Set up desktop
uiWindow wmGetDesktop(wmSession session)
{
	uiWindow desktop = {0};
	desktop = UI.getOn(session.root, X.box(session.root));
	desktop = UI.template(desktop,
	
		// Window switcher
		"Home\n"
		"\n"
		"FOR list=windows\n"
		"<name>\n"
		"ROF\n"
		
		// Shadow 1
		"PANEL x=0.1875 shadow\n"

		// Gap
		"PANEL x=0.8125\n"

		// Shadow 2
		"PANEL x=0.828125 shadow\n"
		
	);
	
	XSelectInput(session.root.display, desktop.window.id,
		PointerMotionMask|ButtonPressMask|ButtonReleaseMask);
	
	return desktop;
}


// Draw desktop
wmSession wmRedrawDesktop(wmSession session)
{
	char markup[1000] = {0};
	
	// Shadows
	forSomeIndices(i in 0 to 1 where session.hasWindow[i],
		sprintf(markup, "%s <shadow box=%i,%i,%i,%i>", markup,
			wmX(session, i) - session.scrollX, 0, wmWidth(session), wmHeight(session)
		);
	);
	UI.draw(ui, markup);
	
	return session;
}


// Clicks. Triggered on desktop background
wmSession wmOnPress(wmSession session, XButtonPressEvent event)
{
	session.drag = true;
	session.dragX = event.x;
	
	return session;
}

// Motion. Focus on to titlebars and, mostly, drag
wmSession wmOnMotion(wmSession session, XMotionNotifyEvent event)
{
	if(!session.dragEvent.active) { return session; }

	// Handle drag
	session.scrollX += event.x - session.dragX;
	session.dragX = event.x;
	
	// Limit drag
	
	// Move windows and redraw
	if(session.hasWindow[0]) { XMoveWindow(session.root.display, session.windows[0], wmX(session, 0), 0);
	if(session.hasWindow[1]) { XMoveWindow(session.root.display, session.windows[1], wmX(session, 1), 0);
	XMoveWindow(session.root.display, session.desktop.window.id, session.scrollX);
	
	return session;
}

wmSession wmOnRelease(wmSession session, XButtonReleaseEvent event)
{
	session.drag = false;
	return session;
}

#endif
