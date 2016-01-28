#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

// Requirements

#include "UIToolkit/X.h"
#include <X11/Xlib.h>


// Types

typedef struct wmSession
{
	xWindow root;
	uiWindow home, desktop;
	
	xWindow windows[2];
	bool hasWindow[2];
	
	bool drag;
	int dragX, scrollX;
} wmSession;


// Public API

struct WM
{
	wmSession	(*open)	(const char* displayName);
	wmSession	(*session)	();
	wmSession	(*wait)	(wmSession);
	wmSession	(*close)	(wmSession);
};


// Private functions

// Dimensions
int wmHeight(wmSession session) { return session.root.attributes.height; }
int wmMargin(wmSession session) { return session.root.attributes.width / 64; }
int wmPanelWidth(wmSession session) { return (3 * session.root.attributes.width) / 16; }
int wmWidth(wmSession session)
{
	return session.hasWindow[1]? 
		((5 * session.root.attributes.width) / 8) : (session.root.attributes.width - wmPanelWidth(session));
}
int wmX(wmSession session, int i)
{
	return session.scrollX + wmPanelWidth(session) + (i > 0? wmWidth(session) + wmMargin(session) : 0);
}

// Replacement X11 error handler - look out for BadWindows
int wmNumRemovals = 0;
Window wmRemovals[50];

int wmErrorHandler(Display* display, XErrorEvent* error) {
	printf("X11 error. Request %i, code %i.%i, resource %li\n",
		error->request_code, error->error_code, error->minor_code, error->resourceid);
	
	char errorText[100];
	
	XGetErrorText(display, error->error_code, errorText, 100);
	printf("Error code: %s\n", errorText);
	
	XGetErrorText(display, error->minor_code, errorText, 100);
	printf("Minor code: %s\n", errorText);
	
	if(error->error_code == BadWindow && wmNumRemovals < 50)
	{
		wmRemovals[wmNumRemovals] = error->resourceid;
		wmNumRemovals += 1;
	}
	
	return error->error_code;
}

// Desktop (shadows, dragging and panel)
#include "WindowManager/wmDesktop.h"

// Session (mapping and resizing windows)
#include "WindowManager/wmSession.h"

// Events handler
wmSession wmEvents(wmSession session)
{
	XEvent event = {0};
	XNextEvent(session.root.display, &event);

	xWindow targetedWindow;
	switch(event.type)
	{
		// Session events
		case MapRequest: session = wmOnMap(session, event.xmaprequest); break;
		case ConfigureRequest: session = wmOnConfigure(session, event.xconfigurerequest); break;
		
		case UnmapNotify:
		case DestroyNotify: session = wmOnRemoved(session, event.xunmap.window); break;
		
		// Desktop events
		case ButtonPress: session = wmOnPress(session, event.xbutton); break;
		case MotionNotify: session = wmOnMotion(session, event.xmotion); break;
		case ButtonRelease: session = wmOnRelease(session, event.xbutton); break;
		
		// Shortcuts
		case KeyPress: break;
	}
	
	// Clear orphans
	
	return session;
}

// Assign private functions to public API

struct WM WM = {
	wmOpen,
	wmEvents,
	wmClose
};

#endif
