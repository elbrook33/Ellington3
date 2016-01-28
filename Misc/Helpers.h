/*
 * Reserved keywords:
 * 
 * in (for loops)
 * to (for indices loops)
 * where (for filtered loops)
 * either
 * then, done
 * is (eq)
 * or, and
 * 
 */


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

// box
typedef struct box
{
	float x, y, width, height;
} box;

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

bool inBox(box box, float x, float y)
{
	return x >= box.x
		&& x <= box.x + box.width
		&& y >= box.y
		&& y <= box.y + box.height;
}

// forEach
#define in	,
#define forEach(item_in_list, body)	__forEach(item_in_list, body)
#define __forEach(item, items, body)	\
if(items) {	\
	typeof(*(items)) item = *(items);	\
	for(int item##Index = 0;	\
		item; \
		item##Index++,	\
		item = (items)[item##Index])	\
	{ body; }	\
}
	
// forEachToken
#define forEachToken(token_in_string, delimiter, body)	__forEachToken(token_in_string, delimiter, body)
#define __forEachToken(token, string, delimiter, body)	\
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

// forIndices
#define to	,
#define where	,
#define forSomeIndices(var_in_min_to_max_where_condition, body)	\
	__forSomeIndices(var_in_min_to_max_where_condition, body)
#define __forSomeIndices(var, a, b, condition, body)	\
for(int var = a; var <= b; var++) {	\
	if(!(condition)) { continue; }	\
	body;	\
}	

// either-list
#define either	if(false);
#define then NULL,
#define done NULL, NULL
#define o(condition, then_body)	__o(condition, then_body)
#define __o(condition, then, body)	else if(condition) { body; }
#define otherwise	true, NULL

// eq (strcmp)
#define is	,
#define eq(A_is_B)	__eq(A_is_B)
#define __eq(A, B)	(strcmp(A, B) == 0)

// iff (mild assert)
#define orFail	,
#define iff(condition_orFail_value)	__iff(condition_orFail_value)
#define __iff(condition, fail)	if(!(condition)) { return fail; }

// miscellaneous
#define and	&&
#define or	||

#endif
