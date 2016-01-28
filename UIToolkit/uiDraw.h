#ifndef UI_DRAW_H
#define UI_DRAW_H


// Requirements
#include "UI.h"


// Headers
uiWindow uiDrawPar(uiWindow, list par, float* x, float* y);

uiWindow uiDrawTab(uiWindow ui, list tab, float* x, float* y);
uiWindow uiDrawLeft(uiWindow ui, list tab, float* x, float* y);
uiWindow uiDrawCentre(uiWindow ui, list tab, float* x, float* y);
uiWindow uiDrawRight(uiWindow ui, list tab, float* x, float* y);

uiWindow uiDrawWord(uiWindow ui, text word, float* x, float* y);
uiWindow uiDrawBold(uiWindow ui, text word, float* x, float* y);
uiWindow uiDrawHighlight(uiWindow ui, text word, float* x, float* y);

uiWindow uiDrawTag(uiWindow ui, list tag, float* x, float* y);


// Setup
uiWindow uiDraw(uiWindow ui, list doc)
{
	//~ if(!ui.canvas.markup || !*ui.canvas.markup) { return uiNoStop; }
	
	//~ // Set up drawing context
	//~ float
		//~ x = ui.canvas.left + ui.canvas.margin,
		//~ y = ui.canvas.top + ui.canvas.margin + ui.canvas.scrollY;
	
	//~ nvgFontSize(ui.canvas.nano, ui.canvas.fontSize);
	//~ nvgFontFace(ui.canvas.nano, "normal");
	//~ nvgFillColor(ui.canvas.nano, ui.canvas.fgColour);
	
	//~ ui.canvas.spaceWidth =
		//~ nvgTextBounds(ui.canvas.nano, 0, 0, "M M", NULL, NULL)
		//~ - nvgTextBounds(ui.canvas.nano, 0, 0, "MM", NULL, NULL);
	
	float x, y;
	
	forEach(par in doc,
		ui = uiDrawPar(ui, par, &x, &y);
	);
	
	// Add doc measurement
	
	return ui;
}


// Top-level categories

uiWindow uiDrawPar(uiWindow ui, list par, float* x, float* y)
{
	forEach(tab in par,
		either
			o (Item.info(tab, "Centre"), then ui = uiDrawCentre(ui, tab, x, y))
			o (Item.info(tab, "Right"), then ui = uiDrawRight(ui, tab, x, y))
			o (otherwise, ui = uiDrawLeft(ui, tab, x, y))
	);
	
	// Add par measurement
	printf("\n");
	
	return ui;
}

uiWindow uiDrawTab(uiWindow ui, list tab, float* x, float* y)
{
	// Reset font styles
	
	forEach(word in tab,
		either
			o (eq(Item.name(word) is "Tag"), then ui = uiDrawTag(ui, word, x, y))
			o (Item.get(word, "Bold"), then ui = uiDrawBold(ui, word, x, y))
			o (Item.get(word, "Highlight"), then ui = uiDrawHighlight(ui, word, x, y))
			o (otherwise, ui = uiDrawWord(ui, word, x, y))
	);
	
	// Add tab measurement
	printf("\t");
	
	return ui;
}

uiWindow uiDrawWord(uiWindow ui, text word, float* x, float* y)
{
	// Measure
	// Draw
	// Advance x, y
	printf("%s ", word);
	
	return ui;
}


// Tab styles

uiWindow uiDrawLeft(uiWindow ui, list tab, float* x, float* y)
{
	return uiDrawTab(ui, tab, x, y);
}

uiWindow uiDrawCentre(uiWindow ui, list tab, float* x, float* y)
{
	return uiDrawTab(ui, tab, x, y);
}

uiWindow uiDrawRight(uiWindow ui, list tab, float* x, float* y)
{
	return uiDrawTab(ui, tab, x, y);
}


// Word styles

uiWindow uiDrawBold(uiWindow ui, text word, float* x, float* y)
{
	return uiDrawWord(ui, word, x, y);
}

uiWindow uiDrawHighlight(uiWindow ui, text word, float* x, float* y)
{
	return uiDrawWord(ui, word, x, y);
}


// Tag (image and shadow)

uiWindow uiDrawTag(uiWindow ui, list tag, float* x, float* y)
{
	return ui;
}

uiWindow uiDrawImage(uiWindow ui, list image, float* x, float* y)
{
	return ui;
}

uiWindow uiDrawShadow(uiWindow ui, list shadow, float* x, float* y)
{
	return ui;
}

#endif

//~ <shadow box=10,10,100,100>
//~ <image file="somewhere.png">


//~ // Pars
//~ uiState uiParsePars(uiWindow ui, uiWordAction action, uiParseContext* data,
	//~ float* x, float* y, const char* markup)
//~ {
	//~ char *pars = strdup(markup);
	//~ forEachToken(par, "\n")
	//~ {
		//~ if(
			//~ uiParseTabs(ui, action, data, x, y, parIndex, par) == uiStop
		//~ )
			//~ { free(pars); return uiStop; }
		
		//~ *y += ui.canvas.lineHeight;
	//~ }

	//~ free(pars);
	//~ return uiNoStop;
//~ }


//~ // Tab

//~ uiState uiParseTabs(uiWindow ui, uiWordAction action, uiParseContext* data,
	//~ float* x, float* y, int parIndex, char* tabs)
//~ {
	//~ int tabCount = strcount(tabs, '\t') + 1;
	//~ float tabWidth = (ui.canvas.right - ui.canvas.left - 2*themeMargin) / (float)tabCount;
	
	//~ forEachToken(tab, "\t")
	//~ {
		//~ *x = (float)(tabIndex - 1)*tabWidth + ui.canvas.left + themeMargin;
		//~ int lastIndex = strlen(tab) - 1;
			
		//~ // Left
		//~ uiAlign align = uiLeft;
		//~ nvgTextAlign(ui.canvas.nano, NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
		
		//~ // Centre
		//~ if(tab[0] == '>' && tab[lastIndex] == '<')
		//~ {
			//~ align = uiCentre;
			//~ *x += tabWidth / 2.0;
			//~ nvgTextAlign(ui.canvas.nano, NVG_ALIGN_CENTER|NVG_ALIGN_TOP);
			//~ tab[lastIndex] = '\0';
			//~ tab++;
		//~ }
		
		//~ // Right
		//~ if(tab[0] == '>' && tab[lastIndex] == '>')
		//~ {
			//~ align = uiRight;
			//~ *x += tabWidth;
			//~ nvgTextAlign(ui.canvas.nano, NVG_ALIGN_RIGHT|NVG_ALIGN_TOP);
			//~ tab[lastIndex] = '\0';
			//~ tab++;
		//~ }
		
		//~ if(
			//~ uiParseWords(ui, action, data, x, y, align, parIndex, tabIndex, tab) == uiStop
		//~ )
			//~ { return uiStop; }
	//~ }
	//~ return uiNoStop;
//~ }

//~ // Word
//~ uiState uiParseWords(uiWindow ui, uiWordAction action, uiParseContext* data,
	//~ float* x, float* y, uiAlign align, int parIndex, int tabIndex, char* words)
//~ {
	//~ uiItemType itemType = uiText;
	
	//~ forEachToken(word, " ")
	//~ {
		//~ // Handle bold and italics
		//~ int lastIndex = strlen(word) - 1;
		
		//~ while(*word && strchr("\\_*<|", *word))
		//~ {
			//~ switch(*word)
			//~ {
				//~ case '\\':
					//~ break;
				//~ case '_':
					//~ nvgFontFace(ui.canvas.nano, "italic");
					//~ break;
				//~ case '*':
					//~ nvgFontFace(ui.canvas.nano, "bold");
					//~ break;
				//~ case '<':
					//~ uiParseTag(ui, action, data, x, y, align, parIndex, tabIndex, word, &wordProgress);
					//~ continue;
				//~ case '|':
					//~ itemType = uiHighlighted;
					//~ break;
			//~ }
			//~ word += 1;
			//~ lastIndex -= 1;
		//~ }
		
		//~ // Handle trailing stylings
		//~ char lastLetter = '\0';
		
		//~ while(lastIndex > 0 && strchr("\\_*<|", word[lastIndex]))
		//~ {
			//~ switch(word[lastIndex])
			//~ {
				//~ case '\\':
					//~ break;
				//~ case '_':
				//~ case '*':
					//~ lastLetter = '_';
					//~ break;
				//~ case '|':
					//~ lastLetter = '|';
					//~ break;
			//~ }
			//~ word[lastIndex] = '\0';
			//~ lastIndex -= 1;
		//~ }
		
		//~ // Send word to "action"
		//~ float bounds[4];
		//~ float textWidth = nvgTextBounds(ui.canvas.nano, *x, *y, word, NULL, bounds) + ui.canvas.spaceWidth;
		//~ uiIndices indices = { parIndex, tabIndex, wordIndex };
		
		//~ bounds[0] -= ui.canvas.spaceWidth/4.0;
		//~ bounds[1] = *y - ui.canvas.margin;
		//~ bounds[2] += ui.canvas.spaceWidth/4.0;
		//~ bounds[3] = *y + ui.canvas.lineHeight;
		
		//~ if(action &&
			//~ action(ui, itemType, word, *x, *y, boxXYXY(bounds), indices, data) == uiStop
		//~ )
			//~ { return uiStop; }
		
		//~ // Advance text entry position
		//~ if(*word != '\0')
		//~ {
			//~ switch(align)
			//~ {
				//~ case uiLeft: *x += textWidth; break;
				//~ case uiRight: *x -= textWidth; break;
			//~ }
		//~ }
		
		//~ // Post-word processing
		//~ switch(lastLetter)
		//~ {
			//~ case '_':
			//~ case '*':
				//~ nvgFontFace(ui.canvas.nano, "normal");
				//~ break;
			//~ case '|':
				//~ itemType = uiText;
				//~ break;
		//~ }
	//~ }
	
	//~ return uiNoStop;
//~ }

//~ // Tag
//~ uiState uiParseTag(uiWindow ui, uiWordAction action, uiParseContext* data,
	//~ float* x, float* y, uiAlign align, int parIndex, int tabIndex, char* word, char** wordProgress)
//~ {
	//~ while(word = strtok_r(NULL, " ", wordProgress))
	//~ {
		//~ int lastIndex = strlen(word) - 1;
		//~ if(word[lastIndex] == '>')
			//~ { return uiStop; }
	//~ }
	//~ return uiNoStop;
//~ }

//~ // Shadow
//~ void uiShadow(uiWindow ui)
//~ {
	//~ int shadowSize = 20, shadowOffset = 10;
	//~ nvgBeginPath(ui.canvas.nano);
	//~ nvgFillPaint(ui.canvas.nano,
		//~ nvgBoxGradient(ui.canvas.nano,
			//~ window.attributes.x,
			//~ window.attributes.y - themeLineHeight + shadowOffset,
			//~ window.attributes.width,
			//~ window.attributes.height + themeLineHeight,
			//~ shadowSize / 2,
			//~ 2 * shadowSize,
			//~ nvgHSLA(0, 0, 0, 191),
			//~ nvgHSLA(0, 0, 0, 0)
		//~ )
	//~ );
	//~ nvgRect(ui.canvas.nano,
		//~ window.attributes.x - shadowSize,
		//~ window.attributes.y - themeLineHeight - shadowSize + shadowOffset,
		//~ window.attributes.width + 2 * shadowSize,
		//~ window.attributes.height + themeLineHeight + 2 * shadowSize
	//~ );
	//~ nvgFill(ui.canvas.nano);
//~ }
