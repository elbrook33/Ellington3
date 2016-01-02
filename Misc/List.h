#ifndef LIST_H
#define LIST_H


// Requirements

#include "Helpers.h"
#include <inttypes.h>
#include <stdarg.h>
#include <stdlib.h>


// Data structures

typedef char* text;
typedef float* vec;
typedef void* listItem;
typedef listItem* list;

typedef struct listMetadata
{
	text type, key;
	int size, length, index;
	list parent;
	list info;
	void *storage,	// Original allocation. Used for free
		*bookend;	// First negative item is always set to zero: null-terminated in reverse.
} listMetadata;


// Public API

struct List
{
	list	(*new)	(text key);
	void	(*delete)	(list);
	
	int	(*length)	(list);
	
	list	(*add)	(list, listItem);
	list	(*text)	(list, text key, text val);
	list	(*vec)	(list, text key, vec val, int length);
	
	list	(*join)	(list, list);
	list (*chain)	(list, ...);
	
	void (*json)	(list);
};

struct Item
{
	text	(*type)	(listItem);
	text	(*name)	(listItem);
	
	text	(*info)	(listItem, text infoKey);
	listItem	(*set)	(listItem, text infoKey, text val);
	listItem	(*unset)	(listItem, text infoKey);
	
	text	(*text)	(text key, text val);
	
	list	(*wrap)	(listItem, text key);
	list	(*unwrap)	(list);
	list	(*delete)	(listItem);
};


// Private functions


// Fat

list listHideFat(char* fat)
{
	return (list)(fat + sizeof(listMetadata));
}

#define RoundTo	0x100
#define RoundMask	~0xff
#define listRoundDown(ptr)	 ((list)((uintptr_t)ptr & RoundMask))
#define listRoundUp(ptr)	 ( (list) (((uintptr_t)ptr & RoundMask) + RoundTo) )

listMetadata* listGetFat(list noFat)
{
	noFat = listRoundDown(noFat);
	while(noFat[-1]) { noFat--; }	// Find NULL at -1
	return (listMetadata*)(((char*)noFat) - sizeof(listMetadata));
}


// Print

void listJsonWorker(list items, int* indent)
{
	printf("\"%s\": {", listGetFat(items)->key);
	*indent += 2;
	
	forEach(item in items,
		printf("\n");
		for(int i = 0; i < *indent; i++) { printf(" "); }

		ifeq(listGetFat(item)->type is "list", listJsonWorker(item, indent))
		ifeq(listGetFat(item)->type is "text",
			printf("\"%s\" (", listGetFat(item)->key);
			forEach(attribute in listGetFat(item)->info,
				printf("%s, ", listGetFat(attribute)->key);
			)
			if(listGetFat(item)->info) { printf("\b\b"); }
			printf("): \"%s\",", (text)item);
		)

	)
	printf("\b \n");
	*indent -= 2;

	for(int i = 0; i < *indent; i++) { printf(" "); }
	printf("},");
}

void listJson(list items)
{
	int indent = 0;
	listJsonWorker(items, &indent);
	printf("\b \n");
}


// Allocate, grow, delete

int listDefaultSize = 20;

list listAllocate(list oldList, int N)
{
	int size = sizeof(listMetadata) + N*(sizeof(listItem)) + RoundTo;
	
	char* storage = oldList? realloc(listGetFat(oldList)->storage, size) : malloc(size);
	iff(storage orFail NULL);

	memset(storage, 0, size);
	
	list L = listRoundUp(storage + sizeof(listMetadata));
	listMetadata* metadata = listGetFat(L);
	metadata->storage = storage;
	metadata->size = N;
	metadata->type = "list";
	
	return L;	
}

list listNew(text key)
{
	list newList = listAllocate(NULL, listDefaultSize);
	listGetFat(newList)->key = key;	// Should strdup?
	return newList;
}

list listGrow(list oldList)
{
	int newSize = listGetFat(oldList)->size + listDefaultSize;
	return listAllocate(oldList, newSize);
}


// Set, add

list listSetItem(list items, int index, listItem item)
{
	while(index + 1 >= listGetFat(items)->size)
		{ listGrow(items); }
	iff(items orFail NULL); // In case realloc fails
	
	items[index] = item;
	
	iff(item orFail items);
	listGetFat(item)->parent = items;
	listGetFat(item)->index = index;

	return items;
}

list listAddItem(list items, listItem item)
{
	iff(item orFail items);
	
	int index = listGetFat(items)->length;
	items = listSetItem(items, index, item);
	
	listGetFat(items)->length += 1;
	
	return items;
}


// Create list items

listItem listMakeItem(text key, listItem val, text type, int size, int length)
{
	listItem item = listAllocate(NULL, size*length/sizeof(listItem) + 1);
	memcpy(item, val, size*length);
	
	listGetFat(item)->type = type;
	listGetFat(item)->key = key;
	listGetFat(item)->length = length;
	
	return item;
}

text listTextItem(text key, text val)
{
	return listMakeItem(key, val, "text", sizeof(char), strlen(val));
}


// Free, delete, remove

void listFree(list L)
{
	free(listGetFat(L)->storage);
}

void listDelete(list items)
{
	iff(items orFail);
	
	ifeq(listGetFat(items)->type is "list",
		forEach(item in items,
			listDelete(item);
		)
	)
	listDelete(listGetFat(items)->info);
	listFree(items);
}

list listRemove(listItem item)
{
	iff(item orFail NULL);
	
	list parent = listGetFat(item)->parent;
	
	if(parent)
	{
		int index = listGetFat(item)->index;
		forEach(tailItem in parent + index + 1,
			parent = listSetItem(parent, index + tailItemIndex, tailItem);
		)
		parent[listGetFat(parent)->length - 1] = NULL;
		listGetFat(parent)->length -= 1;
	}
	
	listDelete(item);
	return parent;
}


// Add text, vec

list listAddText(list items, text key, text val)
{
	text item = listTextItem(key, val);
	return listAddItem(items, item);
}
list listAddVec(list items, text key, vec val, int length)
{
	vec item = listMakeItem(key, val, "vec", sizeof(float), length);
	return listAddItem(items, item);
}


// Join, wrap

list listJoin(list A, list B)
{
	forEach(item in B,
		A = listAddItem(A, item);
	)
	return A;
}

list listWrapItem(listItem item, text key)
{
	list oldParent = listGetFat(item)->parent,
		newParent = listNew(key);
	
	int index = listGetFat(item)->index;
	
	newParent = listAddItem(newParent, item);
	oldParent = listSetItem(oldParent, index, newParent);	// Replace item with new wrapper
	
	return oldParent;
}


// Insert

list listInsertItem(list items, int index, list item)
{
	int originalLength = listGetFat(items)->length;
	
	// Shift items toward end in reverse: from last (length - 0...) to insert location (index)
	forEach(tailProgress in items + index,
		items = listSetItem(items, originalLength - tailProgressIndex, items[originalLength - tailProgressIndex - 1]);
	)
	items = listSetItem(items, index, item);
	listGetFat(items)->length += 1;
	
	return items;
}

list listInsertAfter(listItem item, listItem insert)
{
	list parent = listGetFat(item)->parent;
	iff(parent orFail NULL);
	
	return listInsertItem(parent, listGetFat(item)->index + 1, insert);
}

list listInsertBefore(listItem item, listItem insert)
{
	list parent = listGetFat(item)->parent;
	iff(parent orFail NULL);
	
	return listInsertItem(parent, listGetFat(item)->index, insert);
}


// Unwrap

list listUnwrapChildren(list node)
{
	list parent = listGetFat(node)->parent;
	iff(parent and eq(listGetFat(node)->type is "list") orFail parent)
	
	forEach(child in node,
		parent = listInsertBefore(node, child);
		node[childIndex] = NULL;	// Remove relocated child from node. Prevents delete later.
	)
	
	parent = listRemove(node);
	return parent;
}


// Find

listItem listFind(list items, text key)
{
	forEach(item in items,
		ifeq(listGetFat(item)->key is key, return item;)
	)
	return NULL;
}


// Replace

list listReplace(listItem A, listItem B)
{
	list parent = listSetItem(listGetFat(A)->parent, listGetFat(A)->index, B);
	listDelete(A);
	return parent;
}


// Chain

list listChain(list item, ...)
{
	va_list args;
	va_start(args, item);
	
	list (*fn)();
	while(fn = va_arg(args, list(*)()))
	{
		listItem var[3];
		int i = 0;
		while(var[i++] = va_arg(args, listItem)) {}
		switch(i)
		{
			case 1: item = fn(item); break;
			case 2: item = fn(item, var[0]); break;
			case 3: item = fn(item, var[0], var[1]); break;
		}
	}
		
	return item;
}


// Info functions

list listGetInfoList(listItem item)
{
	if(!listGetFat(item)->info) { listGetFat(item)->info = listNew("info"); }
	return listGetFat(item)->info;
}

text listGetInfo(listItem item, text key)
{
	return (text)listFind(listGetInfoList(item), key);
}

listItem listSetInfo(listItem item, text key, text newVal)
{
	listItem info = listGetInfo(item, key);
	listGetFat(item)->info = info?
		listReplace(info, listTextItem(key, newVal))
		: listAddText(listGetInfoList(item), key, newVal);
	return item;
}

listItem listUnsetInfo(listItem item, text key)
{
	listGetFat(item)->info = listRemove(listGetInfo(item, key));
	return item;
}


// Simple accessors

int listGetLength(list items) { return listGetFat(items)->length; }
text listGetType(listItem item) { return listGetFat(item)->type; }
text listGetKey(listItem item) { return (text)(listGetFat(item)->key); }


// Assign private functions to public API

struct List List = {
	listNew,
	listDelete,
	
	listGetLength,
	
	listAddItem,
	listAddText,
	listAddVec,
	
	listJoin,
	listChain,
	
	listJson
};

struct Item Item = {
	listGetType,
	listGetKey,
	
	listGetInfo,
	listSetInfo,
	listUnsetInfo,
	
	listTextItem,
	
	listWrapItem,
	listUnwrapChildren,
	listRemove
};

#endif
