#ifndef UI_H
#define UI_H


// Requirements

#include <GL/glew.h>
#include <GL/glx.h>
#include <GL/gl.h>
#define NANOVG_GLEW
#define NANOVG_GL2_IMPLEMENTATION
#include "nanovg.h"
#include "nanovg_gl.h"
#include <X11/Xlib.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "UIToolkit/X.h"
#include "Misc/List.h"


// Types

typedef enum { uiLeft, uiCentre, uiRight } uiAlign;
typedef struct uiIndices { int par, tab, word; } uiIndices;

typedef struct uiCanvas
{
	GLXContext glx;
	NVGcontext* nano;
	
	float left, right, top, bottom, margin,
		fontSize, lineHeight, spaceWidth,
		scrollY;
	
	NVGcolor fgColour, bgColour, highlight;
	int bgImage;
	NVGpaint bgImagePattern;
} uiCanvas;

typedef struct uiEvent
{
	int type;
	XEvent xEvent;
	float x, y;
	uiIndices target;
} uiEvent;

typedef struct uiWindow
{
	xWindow window;
	uiCanvas canvas;
	uiEvent event;
	list doc;
} uiWindow;


// Public API

struct UI
{
	// Create and delete
	uiWindow	(*open)	(int, int, int, int);
	void	(*close)	(uiWindow);
	
	// Set template and data
	uiWindow	(*template)	(uiWindow, text template);
	uiWindow	(*set)	(uiWindow, list data);
	
	// Events
	uiWindow	(*wait)	(uiWindow);
};


// Private functions

// Set up X, GL and NanoVG
#include "uiWindow.h"
void uiBeginFrame(uiWindow ui)
{
	glXMakeCurrent(ui.window.display, ui.window.id, ui.canvas.glx);
	nvgBeginFrame(ui.canvas.nano, ui.window.attributes.width, ui.window.attributes.height, 1/1);
}
void uiEndFrame(uiWindow ui)
{
	nvgEndFrame(ui.canvas.nano);
	glFlush();
}

// Parse markup and draw
#include "uiDraw.h"
#include "uiParse.h"
uiWindow uiDrawMarkup(uiWindow ui, text markup)
{
	ui.doc = uiParse(markup);
	return uiDraw(ui, ui.doc);
}
uiWindow uiDrawFrame(uiWindow ui, text markup)
{
	uiBeginFrame(ui);
	ui = uiDrawMarkup(ui, markup);
	uiEndFrame(ui);
	return ui;
}

// Handle events
uiWindow uiGetEvent(uiWindow ui)
{
	return ui;
}
uiWindow uiHighlight(uiWindow ui)
{
	return ui;
}
bool uiFocusIs(uiWindow ui, int par, int tab, int word) { return true; }
bool uiButtonIs(uiWindow ui, int button) { return button == ui.event.xEvent.xbutton.button; }


// Assign private functions to public API

const struct UI UI = {
	uiGetWindow,
	uiGetPanel,
	uiGetWindowOn,
	uiGetPanelOn,
	
	uiClose,
	
	uiDrawFrame,
	uiDrawMarkup,
	
	uiBeginFrame,
	uiEndFrame,
	
	uiGetEvent,

	uiFocusIs,
	uiButtonIs,

	uiHighlight,
	
	uiSetColours,
	uiSetImage
};

#endif
