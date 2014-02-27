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

	for (i = 0; i < 10; i++) {
		CASSERTION_TIME();

		for (j = i * 50000000; j >= 0; j--)
			;

		CASSERTION(i > -1, "i=%02d", i);
	}

	CASSERTION_RESULTS();

	return EXIT_SUCCESS;
}
