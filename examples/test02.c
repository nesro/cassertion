/*
 * https://github.com/nesro/cassertion
 */

#include <stdio.h>
#include <stdlib.h>

#include "../cassertion.h"

int main(int argc, char *argv[]) {

	int i;
	volatile int j;

	CASSERTION_INIT(argc, argv);

	for (i = 1; i < 20; i++) {
		CASSERTION_TIME();

		for (j = i * 30000000; j >= 0; j--)
			;

		CASSERTION(i % 12 != 0, "i=%02d, i is NOT a multiple of 12?", i);
	}

	CASSERTION_RESULTS();

	return EXIT_SUCCESS;
}
