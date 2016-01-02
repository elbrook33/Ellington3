#ifndef HELPERS_H
#define HELPERS_H

#include <stdbool.h>
#include <stdio.h>


// min/max

#define min(a,b)	\
({	\
	typeof (a) _a = (a);	\
	typeof (b) _b = (b);	\
	_a < _b ? _a : _b;	\
})

#define max(a,b)	\
({	\
	typeof (a) _a = (a);	\
	typeof (b) _b = (b);	\
	_a > _b ? _a : _b;	\
})


// strcount

int strcount(const char* text, char c)
{
	int count;
	for(count = 0; *text; text++)
		{ count += (*text == c); }
	return count;
}


// box

typedef struct box
{
	float x, y, width, height;
} box;

bool inBox(box box, float x, float y)
{
	return x >= box.x
		&& x <= box.x + box.width
		&& y >= box.y
		&& y <= box.y + box.height;
}

struct box fitBox(box inner, struct box outer)
{
}

struct box xywhBox(float x, float y, float width, float height)
{
	struct box box;
	box.x = x;
	box.y = y;
	box.width = width;
	box.height = height;
	return box;
}

struct box xyxyBox(float* bounds)
{
	struct box box;
	box.x = bounds[0];
	box.y = bounds[1];
	box.width = bounds[2] - bounds[0];
	box.height = bounds[3] - bounds[1];
	return box;
}


// forEach, forEachToken

#define forEach(item_in_list, body)	forEach2(item_in_list, body)
#define forEach2(item, items, body)	\
if(items) {	\
	typeof(*(items)) item = *(items);	\
	for(int item##Index = 0;	\
		item; \
		item##Index++,	\
		item = (items)[item##Index])	\
	{ body; }	\
}
	

#define in	,

#define forEachToken(token_in_string, delimiter, body)	forEachToken2(token_in_string, delimiter, body)
#define forEachToken2(token, string, delimiter, body)	\
{	\
	text token##Tail,	\
		token##Storage = Item.text("tokens", string),	\
		token = strtok_r(token##Storage, delimiter, &token##Tail);	\
	\
	for(int token##Index = 1; token != NULL;	\
		token = strtok_r(NULL, delimiter, &token##Tail),	\
		token##Index += 1)	\
	{	\
		body;	\
	}	\
	Item.delete(token##Tail);	\
}
	


// maybe, ifeq

#define maybe(optional)	optional = optional? optional
#define onlyIf(optional)	if(optional) optional = false? optional

#define eq(A_is_B) eq2(A_is_B)
#define eq2(A, B) (strcmp(A, B) == 0)
#define ifeq(A_is_B, body) if(eq2(A_is_B)) { body; }
#define is ,

#define either	if(false);
#define o(condition)	else if(condition)
#define otherwise true


// assert

#define iff(condition_orFail_value) iff2(condition_orFail_value)
#define iff2(condition, fail)	if(!(condition)) { return fail; }
#define orFail	,

#define and &&
#define or ||


// for chaining

#define then NULL,
#define done NULL, NULL

#endif
