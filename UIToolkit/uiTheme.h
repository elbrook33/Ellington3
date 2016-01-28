#ifndef UI_THEME_H
#define UI_THEME_H

#include <time.h>


// Public API

struct Theme {
	NVGcolor	(*bg)	();
	NVGcolor	(*fg)	();
	NVGcolor	(*highlight)	();
	
	float	(*fontSize)	();
	float	(*lineHeight)	();
	
	char*	(*date)	();
};


// Private functions

char* themeDateString(char* storage, int storageSize)
{
	struct tm* tm;
	time_t t;
	
	t = time(NULL);
	tm = localtime(&t);
	
	strftime(storage, storageSize, "%A, %B %-d – %-I.%M%P", tm);
	return storage;
}


// Assign private functions to API

struct Theme Theme = {
	bg,
	fg,
	highlight,
	
	fontSize,
	lineHeight,
	
	themeDateString
};

#endif
