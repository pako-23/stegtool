#include <stdlib.h>
#include <stdio.h>

int main(int argc, const char *argv[]) 
{ 
	if (argc != 2)
		return 1;
	
	const char *filename = argv[1];

	FILE *fp = fopen(filename, "rb+");
	if (!fp) 
		return 1;
}
