/*
 * Parser
 * ======
 * 
 * Three steps:
 * 1. Split markup into pars and tabs (by \n and \t).
 * 2. Every tab (i.e. list of words) is parsed for markup:
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
list uiParsePanel(text, text* remainder)
list uiParseBody(text);
list uiParseTab(text);
list uiParseWord(text, text* remainder);

list uiParseOptions(text par, list target);
list uiParseBlock(text firstLine, char* start, char* end, text* remainder);
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

list uiParseDoc(text markup)
{
	list doc = List.new("Doc");
	list panel = List.new("Panel");
	
	doc = List.add(doc, panel);
	
	forEachToken(par in markup, "\n",
		// Check if a new panel is being declared
		if(strncmp(par, "PANEL", 5) == 0)
		{
			panel = List.new("Panel");
			panel = uiParseOptions(par, panel);
			doc = List.add(doc, panel);
		}
		else
			{ panel = List.add(panel, uiParsePanel(par, &parTail)); }
	);
	return doc;
}

list uiParsePanel(text par, text* remainder)
{
	list node = {0};
	
	either
		o (node = uiParseBlock(par, "BLOCK", "KCOLB", remainder), done)
		o (node = uiParseBlock(par, "FOR", "ROF", remainder), done)
		o (node = uiParseBlock(par, "IF", "FI", remainder), done)
		o (otherwise, node = uiParseBody(par))
	
	return node;
}

list uiParseBody(text par)
{
	list tabs = List.new("Par");
	
	forEachToken(tab in par, "\t",
		list node = {0};		

		either
			o (node = uiParseTabStyle(tab, "Centre", '>', '<'), done)
			o (node = uiParseTabStyle(tab, "Right", '>', '>'), done)
			o (otherwise, node = uiParseTab(tab))
		
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
	
	either
		o (node = uiParseBrackets(word, "Bold", "_", "_", remainder), done)
		o (node = uiParseBrackets(word, "Highlight", "*", "*", remainder), done)
		o (node = uiParseBrackets(word, "Tag", "<", ">", remainder), done)
		o (otherwise, node = uiParseNormal(word))
	
	return node;
}


// Blocks, styles and brackets

list uiParseOptions(text par, list target)
{
	forEachToken(word in par, " ",
		// Parse "key=val"
		char* val = strchr(word, '=');
		if(val) { *val = '\0'; val += 1; }
		else { val = "yes"; }
		
		// Add pair to target's metadata
		target = Item.set(target, Item.text(word, val));
	);
	return target;
}

list uiParseBlock(text firstLine, char* start, char* end, text* remainder)
{
	iff(strncmp(firstLine, start, strlen(start)) == 0 orFail NULL);
	
	list block;
	block = List.new(start);
	block = uiParseOptions(firstLine, block);
	
	// Parse block 
	forEachToken(par in *remainder, "\n",
		if(strncmp(par, end, strlen(end)) == 0) { *remainder = parTail; break; }

		// Should probably potentially recurse.
		block = List.add(block, uiParsePar(par));
	);
	
	return block;
}

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
	
	// Chop off left bracket
	word += 1;
	
	list node = List.new(styleName);
	bool bracketsClosed = false;
	
	// Make a copy of the tail so we can roll back if closing bracket is not found
	text tempTail = Item.text("tail", *remainder);
	
	do
	{
		int last = strlen(word)-1;
		
		// Remove end bracket if it's a match
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
		// Swap in our tempTail for remainder
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
	// Don't flatten text nodes
	iff(eq(Item.type(tree) is "list") orFail tree);
	
	forEach(item in tree,
		// Depth-first recursion
		item = uiParseFlatten(item);
		
		// Pre-emptively move current index forward to accomodate new items
		itemIndex += List.length(item) - 1;

		either
			o (eq(Item.name(item) is "Centre"), then tree = uiParseUnwrap(item, "Centre"))
			o (eq(Item.name(item) is "Right"), then tree = uiParseUnwrap(item, "Right"))
			o (eq(Item.name(item) is "Bold"), then tree = uiParseUnwrap(item, "Bold"))
			o (eq(Item.name(item) is "Highlight"), then tree = uiParseUnwrap(item, "Highlight"))
			o (otherwise, itemIndex -= List.length(item) - 1) // Move index back if not unwrapping
	)
	return tree;
}

list uiParseUnwrap(list item, text styleName)
{
	forEach(child in item,
		item[childIndex] = Item.set(child, Item.text(styleName, "yes"));
	);
	return List.unwrap(item);
}


#endif
