//
// Routines for creating windows and getting their attributes
//

#ifndef X11_INTERFACE_H
#define X11_INTERFACE_H


// Requirements

#include "Misc/Helpers.h"
#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/Xatom.h>	// For WINDOW_TYPE
#include <X11/Xutil.h>	// For titles and app names
#include <stdbool.h>


// Types

typedef struct xWindow
{
	Display* display;
	Window id, root;
	XWindowAttributes attributes;
	XClassHint name;
} xWindow;


// Public API

struct X
{
	xWindow	(*open)	(const char* displayName);
	xWindow	(*window)	(xWindow root, box);
	xWindow	(*panel)	(xWindow root, box);
	void	(*close)	(xWindow);
	
	int	(*x)	(xWindow);
	int	(*y)	(xWindow);
	int	(*width)	(xWindow);
	int	(*height)	(xWindow);
};


// Private functions

// Helpers
xWindow xUpdateAttributes(xWindow window)
{
	XGetWindowAttributes(window.display, window.id, &window.attributes);
	XGetClassHint(window.display, window.id, &window.name);
	return window;
}


// New windows
xWindow xGetRoot(const char* displayID)
{
	xWindow root = {0};

	root.display = XOpenDisplay(displayID);
	root.id = root.root = XDefaultRootWindow(root.display);
	root = xUpdateAttributes(root);

	return root;
}

xWindow xCreate(xWindow root, int x, int y, int width, int height, bool panel)
{
	xWindow newWindow = {0};
	newWindow.display = root.display;
	newWindow.root = root.id;
	
	XSetWindowAttributes attributes = {0};
	
	if(panel) { attributes.override_redirect = true; }
	
	newWindow.id = XCreateWindow(newWindow.display, root.id,
		x, y, width, height, 0,
		root.attributes.depth, CopyFromParent, CopyFromParent,
		panel? CWOverrideRedirect : 0, &attributes
	);
	XMapWindow(newWindow.display, newWindow.id);

	newWindow = xUpdateAttributes(newWindow);	
	return newWindow;
}

xWindow xCreateWindow(xWindow root, box box)
{
	return xCreate(root, box.x, box.y, box.width, box.height, false);
}

xWindow xCreatePanel(xWindow root, box box)
{
	return xCreate(root, box.x, box.y, box.width, box.height, true);
}

xWindow xWrap(xWindow root, Window id)
{
	xWindow newWindow = {0};
	
	newWindow.display = root.display;
	newWindow.id = id;
	newWindow = xUpdateAttributes(newWindow);
	
	return newWindow;
}

void xClose(xWindow window)
{
	if(window.id == window.root)
		{ XCloseDisplay(window.display); }
	else
		{ XDestroyWindow(window.display, window.id); }
}


// Window types (and other Atom-based properties)
Atom xGetWindowType(xWindow window)
{
	Atom flag_windowType =
		XInternAtom(window.display, "_NET_WM_WINDOW_TYPE", false );
	
	Atom typeReturned;
	int formatReturned;
	unsigned long numAtomsReturned, unreadBytes;
	unsigned char* rawData;
	
	XGetWindowProperty(window.display, window.id, flag_windowType,
		0L, 1L, false, XA_ATOM, &typeReturned, &formatReturned,
		&numAtomsReturned, &unreadBytes, &rawData);
	
	Atom type = numAtomsReturned > 0?
		((Atom*)rawData)[0] : 0;
	
	XFree(rawData);
	return type;
}

bool xIsNormal(xWindow window)
{
	Atom windowType = xGetWindowType(window);
	
	Atom flag_normalWindow =
		XInternAtom(window.display, "_NET_WM_WINDOW_TYPE_NORMAL", false);
	Atom flag_dialogWindow =
		XInternAtom(window.display, "_NET_WM_WINDOW_TYPE_DIALOG", false);
	
	return window.attributes.override_redirect == false
		&& (windowType == flag_normalWindow
			|| windowType == flag_dialogWindow
			|| windowType == 0);
}


// Assign private functions to public API

int xX(xWindow window) { return window.attributes.x; }
int xY(xWindow window) { return window.attributes.y; }
int xWidth(xWindow window) { return window.attributes.width; }
int xHeight(xWindow window) { return window.attributes.height; }

struct X X = {
	xGetRoot,
	xCreateWindow,
	xCreatePanel,
	xClose,
	
	xX,
	xY,
	xWidth,
	xHeight
};

#endif
