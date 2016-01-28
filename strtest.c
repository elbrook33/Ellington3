#include <string.h>
#include <stdio.h>

#include "Misc/List.h"
#include <stdlib.h>

int x()
{
	return 10;
}

int y(int* z)
{
	return *z;
}

typedef char* string;

int main()
{
	char str[] = "123 456 7 89 101112 13";
	
	char *tail, *word = strtok_r(str, " ", &tail);
	
	while(word = strtok_r(NULL, " ", &tail))
	{
		tail--;
		printf("%s | %s\n", word, tail);
	}
	
	int j = 0;
	for(
		int i = 0;
		j < 10,
		i < 10;
		i++,
		j = 5
	) { printf("%i%i\n", i, j); }
	
	char mat[3][4] = {"abc", "def", "ghi"};
	
	char** slist = malloc(sizeof(char*)*2);
	slist[0] = str;
	
	typeof(tail) a = "AAA";
	typeof(*str) b = *str;
	
	printf("%s %s %s, %s %c\n", mat[0], mat[1], mat[2], a, b);
	
	forEach(x in slist,
		printf("%s %lui\n", x, sizeof(string));
	)
	
	list L = List.new("List!");
	
	L = List.text(L, "key", "val");
	L = List.text(L, "key", "val");
	L = List.text(L, "key", "val");
	
	forEach(l in L,
		l = Item.set(l, "INFO", "content");
		printf("%s\n", Item.name(l));
	)
	
	List.chain(L,
		List.text, "name", "val", then
		List.text, "another", "Has this in it.", done);
	
	forEach(l in L,
		ifeq(Item.name(l) is "name",
			printf("%s: %s (%s)\n", Item.name(l), (text)l, Item.info(l, "INFO"));
		)
		
		either
			o (Item.info(l, "INFO")) { printf("Has info\n"); }
			o (eq((text)l is "val")) { printf("Is val\n"); }
			o (true) { printf("No info\n"); }
	)
	
	iff(false orFail 1);
	
	printf("Didn't fail.\n");
	
	return 0;
}
