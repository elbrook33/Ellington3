//
// wmSession
// Set up session and handle map, configure, unmap and destroy.
//

#ifndef WM_SESSION_H
#define WM_SESSION_H

// Start session
wmSession wmOpen(const char* displayID)
{
	xWindow root = xGetRoot(displayID);
	
	// Input
	XSelectInput(root.display, root.id,
		SubstructureRedirectMask|SubstructureNotifyMask
	);
	// Mod4 + left/right → scroll
	// Mod4 + up/down → switch
	// Mod4 + h → show/hide home screen
	
	// Create components
	wmSession session = {0};
	session.root = root;
	session.desktop = wmGetDesktop(session);
	
	XSetErrorHandler(wmErrorHandler);
	return session;
}

void wmClose(wmSession session)
{
	UI.close(session.desktop);
	UI.close(session.home);
	X.close(session.root);
}

// Layout desktop
wmSession wmLayoutWorkspace(wmSession session)
{	
	if(session.hasWindow[0])
	{
		XMoveResizeWindow(session.root.display, session.windows[0].id,
			wmX(session, 0), 0,
			wmWidth(session), wmHeight(session)
		);
	}
	if(session.hasWindow[1])
	{
		XMoveResizeWindow(session.root.display, session.windows[1].id,
			wmX(session, 1), 0,
			wmWidth(session), wmHeight(session)
		);
	}
	
	return wmRedrawDesktop(session);
}

// Switch windows
wmSession wmSwitchToWindow(wmSession session, int id)
{
	if(id == session.windows[0].id) { return session; }
	
	// Hide all currently active
	if(session.hasWindow[0]) { XUnmapWindow(session.root.display, session.windows[0].id); }
	if(session.hasWindow[1]) { XUnmapWindow(session.root.display, session.windows[1].id); }
	
	// Layout window
	session.windows[0] = X.wrap(session.root, id);
	session.hasWindow[0] = true;
	session.hasWindow[1] = false;
	session = wmLayoutWorkspace(session);
	
	// Show all newly active
	XMapWindow(session.root.display, session.windows[0].id);
	
	return session;
}

// Map, configure, unmap and destroy
wmSession wmOnMap(wmSession session, XMapRequestEvent event)
{
	xWindow target = X.wrap(session.root, event.window);
	
	either
		o (X.isNormal(target), then	// Update panel
				XSetWindowBorderWidth(session.root.display, target.id, 0);
				session = wmRedrawDesktop(session))

		o (X.isDialog(target), then	// Show dialog
				XSetWindowBorderWidth(session.root.display, target.id, 20);
				XMapWindow(session.root.display, target.id))

		o (otherwise,	// Show tooltip, menu, etc
				XMapWindow(session.root.display, target.id))
	
	return session;
}

wmSession wmOnConfigure(wmSession, XConfigureRequestEvent event)
{
	// Boilerplate: sign-off on all changes
	XWindowChanges changes = {0};
	changes.x = event.x;
	changes.y = event.y;
	changes.width = event.width;
	changes.height = event.height;
	changes.border_width = event.border_width;
	changes.sibling = event.above;
	changes.stack_mode = event.detail;
	XConfigureWindow(session.root.display, event.window, event.value_mask, &changes);
	
	// Don't let normal and dialog windows resize or move
	
	return session;
}

#endif
