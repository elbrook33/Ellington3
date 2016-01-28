#ifndef LIST_H
#define LIST_H


// Requirements

#include "Helpers.h"
#include <inttypes.h>
#include <stdarg.h>
#include <stdlib.h>


// Data structures

typedef char* text;
typedef void* listItem;
typedef listItem* list;

typedef struct listMetadata
{
	text type, key;
	int size, length, index;
	list parent;
	list info;
	void *storage,	// Original allocation. Used for free()
		*bookend;	// First negative item is always set to zero, i.e. null-terminated in reverse
} listMetadata;


// Public API

struct List
{
	// Create and delete
	list	(*new)	(text key);
	list	(*delete)	(list);
	
	// Add and retrieve items
	list	(*add)	(list, listItem);
	listItem	(*get)	(list, text);
	
	// Metadata
	int	(*length)	(list);
	
	// Modify
	list	(*unwrap)	(list);
	
	// Print
	void (*json)	(list);
};

struct Item
{
	// Create and delete
	text	(*text)	(text key, text val);
	listItem	(*custom)	(text key, listItem val, text type, int size, int length);
	list	(*delete)	(listItem);
	
	// Get and set metadata
	text	(*name)	(listItem);
	text	(*type)	(listItem);
	text	(*get)	(listItem, text infoKey);
	listItem	(*set)	(listItem, listItem infoItem);
};


// Private functions


// Fat (some slightly gnarly pointer calculations)

list listHideFat(char* fat)
{
	return (list)(fat + sizeof(listMetadata));
}

#define RoundToNearest	0x10
#define RoundMask	~0xf
#define listRoundDown(ptr)	 ((list)((uintptr_t)ptr & RoundMask))
#define listRoundUp(ptr)	 ( (list) (((uintptr_t)ptr & RoundMask)+RoundToNearest) )

listMetadata* listGetFat(list noFat)
{
	noFat = listRoundDown(noFat);
	while(noFat[-1]) { noFat--; }	// Find NULL at -1
	return (listMetadata*)(((char*)noFat) - sizeof(listMetadata));
}


// Print (printf gets a bit ugly!)

void listJsonWorker(list L, int* indent)
{
	printf("\"%s\": {", listGetFat(L)->key);
	*indent += 2;
	
	forEach(item in L,
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

void listJson(list L)
{
	int indent = 0;
	listJsonWorker(L, &indent);
	printf("\b \n");
}


// Allocate, grow, delete

list listAllocate(list oldList, int N)
{
	int size = sizeof(listMetadata) + N*(sizeof(listItem)) + RoundToNearest;
	
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

int listDefaultSize = 20;

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

list listSetItem(list L, int index, listItem item)
{
	while(index + 1 >= listGetFat(L)->size)	// +1 for NULL-end
		{ listGrow(L); }
	iff(L orFail NULL); // In case realloc fails
	
	L[index] = item;
	
	iff(item orFail L);
	listGetFat(item)->parent = L;
	listGetFat(item)->index = index;

	return L;
}

list listAddItem(list L, listItem item)
{
	int index = listGetFat(L)->length;
	L = listSetItem(L, index, item);
	
	listGetFat(L)->length += 1;
	
	return L;
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

list listDelete(list L)
{
	iff(L orFail NULL);	// To avoid deleting a NULL info list
	
	ifeq(listGetFat(L)->type is "list",
		forEach(item in L,
			listDelete(item);
		);
	);
	listDelete(listGetFat(L)->info);
	listFree(L);
	
	return NULL;
}

list listRemoveWithoutDelete(listItem item)
{
	list parent = listGetFat(item)->parent;
	
	if(parent)
	{
		int index = listGetFat(item)->index;
		forEach(tailItem in parent + index + 1,
			parent = listSetItem(parent, index + tailItemIndex, tailItem);
		);
		parent[listGetFat(parent)->length - 1] = NULL;
		listGetFat(parent)->length -= 1;
		listGetFat(item)->parent = NULL;
	}
	
	return parent;
}

list listDeleteItem(listItem item)
{
	list L = listRemoveWithoutDelete(item);
	listDelete(item);
	return L;
}


// Add text, vec

list listAddText(list L, text key, text val)
{
	text item = listTextItem(key, val);
	return listAddItem(L, item);
}
list listAddVec(list L, text key, vec val, int length)
{
	vec item = listMakeItem(key, val, "vec", sizeof(float), length);
	return listAddItem(L, item);
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

list listInsertItem(list L, int index, list item)
{
	if(listGetFat(item)->parent == L)
		{ L = listRemoveWithoutDelete(item); }
	
	int originalLength = listGetFat(L)->length;
	
	// Shift items toward end in reverse: from last (length - 0...) to insert location (index)
	forEach(tailProgress in L + index,
		L = listSetItem(L, originalLength - tailProgressIndex, L[originalLength - tailProgressIndex - 1]);
	);
	
	L = listSetItem(L, index, item);
	listGetFat(L)->length += 1;
	
	return L;
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
	);
	
	parent = listDeleteItem(node);
	return parent;
}


// Find

listItem listFind(list L, text key)
{
	forEach(item in L,
		ifeq(listGetFat(item)->key is key, return item;)
	);
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

list listChain(list L, ...)
{
	va_list args;
	va_start(args, L);
	
	list (*fn)();
	while(fn = va_arg(args, list(*)()))
	{
		listItem var[3];
		int i = 0;
		while(var[i++] = va_arg(args, listItem)) {}
		switch(i)
		{
			case 1: L = fn(L); break;
			case 2: L = fn(L, var[0]); break;
			case 3: L = fn(L, var[0], var[1]); break;
		}
	}
	
	return L;
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
	listGetFat(item)->info = listDeleteItem(listGetInfo(item, key));
	return item;
}


// Simple accessors

int listGetLength(list L) { return listGetFat(L)->length; }
text listGetType(listItem item) { return listGetFat(item)->type; }
text listGetKey(listItem item) { return (text)(listGetFat(item)->key); }


// Assign private functions to public API

struct List List = {
	listNew,
	listDelete,
	
	listAddItem,
	listFind,
	
	listGetLength,
	listUnwrapChildren,
	
	listJson
};

struct Item Item = {
	listTextItem,
	listMakeItem,
	listDeleteItem,
	
	listGetKey,
	listGetType,
	listGetInfo,
	listSetInfo
};

#endif
