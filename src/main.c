#include <stdio.h>

#include "pseudo_pci.h"

int main(int argc, char **argv)
{
	char *slot;
	if (argc != 2) {
		printf("Err");
		return 1;
	}
	slot = argv[1];
	print_devices(slot);
	return 0;
}
