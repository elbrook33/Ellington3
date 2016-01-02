#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include "UIToolkit/uiParse.h"
#include "UIToolkit/uiDraw.h"

int main()
{
	list A = List.new("A");
	list B = List.new("B");
	list C = List.new("C");
	listItem D = Item.text("d", "value");
	
	//~ List.text(A, "key", "a1");
	List.add(A, B);
	List.text(B, "key", "b1");
	List.add(B, C);
	List.text(B, "key", "b2");
	List.text(C, "key", "c1");
	List.text(C, "key", "c2");
	List.add(B, D);
	
	printf("Original A\n"
		"==========\n");
	List.json(A);
	
	printf("\nDelete D\n"
		"========\n");
	Item.delete(D);
	List.json(A);
	
	printf("\nUnwrap\n"
		"======\n");
	Item.unwrap(B);	
	List.json(A);
	
	char markup[] = "Test word	Tab	_Bold_\n*_Bold_ highlight*\n<Tag>	<_Tag_ *highlight*>\n*Confusion <With *Bold_ and loss";
	printf("\nMarkup\n"
		"======\n"
		"%s\n", markup);
	
	printf("\nParse\n"
		"=====\n");
	list result = uiParseDoc(markup);
	List.json(result);
	
	printf("\nFlatten\n"
		"=======\n");
	result = uiParseFlatten(result);
	List.json(result);
	
	List.delete(A);
	List.delete(result);
	
	
	text T = Item.text("Key", "Here is some text");
	text X = T + 5;
	
	printf("%s, advanced to: %s\n", T, X);
	Item.delete(X);
	
	printf("Deleted from the middle!\n");
	
	list doc = uiParse(markup);
	uiDraw(NULL, doc);
	
	List.delete(doc);
	
	return 0;
}
