/*
 * Parser
 * ======
 * 
 * Three steps:
 * 1. Split markup into pars and tabs (by \n and \t).
 * 2. Every tab is parsed for markup:
 * 	- Tries various brackets, munching through copies of strings until they work.
 * 	- If a match is found, the new position is returned.
 * 3. The tree is flattened into a table (par/tab/word) with embedded style information.
 */

#ifndef UI_PARSER_H
#define UI_PARSER_H


// Requirements
#include "Misc/List.h"


// Parser headers
list uiParseDoc(text);
list uiParsePar(text);
list uiParseTab(text);
list uiParseWord(text, text* remainder);

list uiParseTabStyle(text tab, char* style, char left, char right);
list uiParseBrackets(text word, char* style, char left, char right, text* remainder);
list uiParseNormal(text);

list uiParseFlatten(list tree);
list uiParseUnwrap(list item, text style);


// Main outline

list uiParse(text markup)
{
	list result = uiParseDoc(markup);
	return uiParseFlatten(result);
}


// Top-level

list uiParseDoc(text doc)
{
	list pars = List.new("Doc");
	
	forEachToken(par in doc, "\n",
		pars = List.add(pars, uiParsePar(par));
	);
	return pars;
}

list uiParsePar(text par)
{
	list tabs = List.new("Par");
	
	forEachToken(tab in par, "\t",
		list node = {0};
		
		maybe(node): uiParseTabStyle(tab, "Centre", '>', '<');
		maybe(node): uiParseTabStyle(tab, "Right", '>', '>');
		maybe(node): uiParseTab(tab);

		tabs = List.add(tabs, node);
	);
	return tabs;
}

list uiParseTab(text tab)
{
	list words = List.new("Tab");
	
	forEachToken(word in tab, " ",
		words = List.add(words, uiParseWord(word, &wordTail));
	);
	return words;
}

list uiParseWord(text word, text* remainder)
{
	list node = {0};
	
	maybe(node): uiParseBrackets(word, "Bold", '_', '_', remainder);
	maybe(node): uiParseBrackets(word, "Highlight", '*', '*', remainder);
	maybe(node): uiParseBrackets(word, "Tag", '<', '>', remainder);
	maybe(node): uiParseNormal(word);
	
	return node;
}


// Specifics

list uiParseTabStyle(text tab, char* styleName, char left, char right)
{
	iff(tab[0] == left
			and
		tab[strlen(tab)-1] == right
			orFail NULL);
	
	// Chop off ends
	tab[strlen(tab)-1] = '\0';
	tab += 1;
	
	list node = List.new(styleName);
	return List.add(node, uiParseTab(tab));
}

list uiParseBrackets(text word, char* styleName, char left, char right, text* remainder)
{
	iff(word[0] == left orFail NULL);
	
	word += 1;
	
	list node = List.new(styleName);
	bool bracketsClosed = false;
	
	// Make a copy of the tail so we can roll back if closing bracket is not found
	text tempTail = Item.text("tail", *remainder);
	
	do
	{
		int last = strlen(word)-1;
		
		// Remove bracket if it's a match
		if(word[last] == right)
		{
			word[last] = '\0';
			bracketsClosed = true;
		}
		
		// Recurse into word
		node = List.add(node, uiParseWord(word, &tempTail));
		
	} while(!bracketsClosed && (word = strtok_r(NULL, " ", &tempTail)));
	
	if(bracketsClosed)
	{
		// Swap in our tempTail
		Item.delete(*remainder);
		*remainder = tempTail;
		return node;
	}
	else
	{
		// Never mind
		Item.delete(tempTail);
		List.delete(node);
		return NULL;
	}
}

list uiParseNormal(text word)
{
	return (list)Item.text("Word", word);
}


// Flatten

list uiParseFlatten(list tree)
{
	iff(eq(Item.type(tree) is "list") orFail tree);	// Don't try to flatten text nodes
	
	forEach(item in tree,
		item = uiParseFlatten(item);	// Depth-first recursion
		itemIndex += List.length(item) - 1;	// Pre-emptively move current index forward to accomodate new items
		either
			o (eq(Item.name(item) is "Centre")) { tree = uiParseUnwrap(item, "Centre"); }
			o (eq(Item.name(item) is "Right")) { tree = uiParseUnwrap(item, "Right"); }
			o (eq(Item.name(item) is "Bold")) { tree = uiParseUnwrap(item, "Bold"); }
			o (eq(Item.name(item) is "Highlight")) { tree = uiParseUnwrap(item, "Highlight"); }
			o (otherwise) { itemIndex -= List.length(item) - 1; }	// Move index back if not unwrapping
	)
	return tree;
}

list uiParseUnwrap(list item, text styleName)
{
	forEach(child in item,
		item[childIndex] = Item.set(child, styleName, "yes");
	);
	return Item.unwrap(item);
}


#endif
