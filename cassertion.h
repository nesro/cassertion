/*
 * https://github.com/nesro/cassertion
 */

#ifndef CASSERTION_H_
#define CASSERTION_H_

#define USE_OMP 1
#define STREAM stdout

#ifdef USE_OMP
#include "omp.h"
#else
#include <sys/times.h>
#endif

typedef struct cassertion {
	/*
	 * To set up
	 */
	int exit_on_failure;

	/*
	 * Private members
	 */
	double time;
	double time_prev;
#ifndef USE_OMP
	struct timeval tv;
#endif

	int passed_assertions;
	int failed_assertions;
} cassertion_t;

/*
 * Global variable!
 */
cassertion_t cassertion;

#define BEGIN do {
#define END } while(0)

/*
 * TODO: If things will get more complicated, use settings with flags.
 */
#define CASSERTION_INIT(argc, argv) BEGIN \
	if (argc > 1 && argv[1][0] == '1') \
		cassertion.exit_on_failure = 1; \
	END

#ifdef USE_OMP
#define CASSERTION_TIME(void) BEGIN \
	cassertion.time = omp_get_wtime(); \
	END
#else
#define CASSERTION_TIME(void) BEGIN \
	gettimeofday(&cassertion.tv, NULL); \
	cassertion.time = (cassertion.tv.tv_sec) * 1000 + (cassertion.tv.tv_usec) \
		/ 1000; \
	END
#endif

/*
 * Example usage:
 * CASSERTION(2 > 1, "is %d greater than %d", 2, 1);
 *
 * Logs are in this format:
 * file|line|function|time#|result
 *
 * # time between CASSERTION_TIME() and CASSERTION(...) in microseconds
 */
#define CASSERTION(condition, message, ...) BEGIN \
	fprintf(STREAM, "@cassertion_log|%s|%d|%s|", __FILE__, __LINE__, \
					__FUNCTION__); \
	\
	if (cassertion.time > 0) { \
		cassertion.time_prev = cassertion.time; \
		CASSERTION_TIME(); \
		fprintf(STREAM, "%lf|", cassertion.time - cassertion.time_prev); \
		cassertion.time = 0; \
	} else { \
		fprintf(STREAM, "-|"); \
	} \
	\
	fprintf(STREAM, "%s|" message "|", #condition, __VA_ARGS__); \
	\
	if (condition) { \
		fprintf(STREAM, "ok\n"); \
		cassertion.passed_assertions++; \
	} else { \
		fprintf(STREAM, "fail\n"); \
		if (cassertion.exit_on_failure) { \
			fprintf(STREAM, "@cassertion_results|%s|%d|then_failed\n", \
					__FILE__, cassertion.passed_assertions); \
			exit(EXIT_FAILURE); \
		} \
		cassertion.failed_assertions++; \
	} \
	\
	fflush(STREAM); \
	END

#define CASSERTION_RESULTS(void) BEGIN \
	fprintf(STREAM, "@cassertion_results|%s|%d|%d\n", \
		__FILE__, cassertion.passed_assertions, \
		cassertion.failed_assertions); \
	END

#endif /* CASSERION_H_ */
