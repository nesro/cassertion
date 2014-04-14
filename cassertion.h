/*
 * https://github.com/nesro/cassertion
 */

#include <stdio.h>

#ifndef CASSERTION_H_
#define CASSERTION_H_

#define USE_OMP 1
#define STREAM stdout

#define COLORS 1

#if COLORS

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define OK  KGRN "OKAY" KNRM
#define FAIL  KRED "FAIL" KNRM
#else /* COLORS */
#define OK "[OKAY]"
#define FAIL  "[FAIL]"
#endif /* COLORS */

#ifdef USE_OMP
#include "omp.h"
#else
#include <sys/times.h>
#endif /* USE_OMP */

typedef struct cassertion {

	/*
	 * Public.
	 */
	int dontrun;

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
#endif /* USE_OMP */

	int passed_assertions;
	int failed_assertions;
	int dontrun_assertions;
} cassertion_t;

/*
 * Global variable!
 */
cassertion_t cassertion;

/*
 * Cassertion macros are like function so they're wrapped in
 * a do-while(0) cycle. You MUST add a semicolon (;) after them.
 * Like this:
 * CASSERTION_SOMETHING(arg0, arg1);
 *                       semicolon ^
 */
#define BEGIN do {
#define END } while(0)

#define CASSERTION_LOG(type) BEGIN \
	fprintf(STREAM, "@cassertion_" type "|%s|%d|%s|", __FILE__, __LINE__, \
			__FUNCTION__); \
	END

#define CASSERTION_MSG(message, ...) BEGIN \
	CASSERTION_LOG("message"); \
	fprintf(STREAM, message, __VA_ARGS__); \
	END

#define CASSERTION_VARDUMP(print, var) BEGIN \
	CASSERTION_LOG("vardump"); \
	fprintf(STREAM, print, var); \
	END

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
#else /* USE_OMP */
#define CASSERTION_TIME(void) BEGIN \
	gettimeofday(&cassertion.tv, NULL); \
	cassertion.time = (cassertion.tv.tv_sec) * 1000 + (cassertion.tv.tv_usec) \
		/ 1000; \
	END
#endif /* USE_OMP */

/*
 * Example usage:
 * CASSERTION(2 > 1, "is %d greater than %d", 2, 1);
 *
 * Logs are in this format:
 * file|line|function|time#|result
 *
 * # time between CASSERTION_TIME() and CASSERTION(...) in microseconds
 *
 *
 *
 * fprintf(STREAM, "@cassertion_test|%s|%d|%s|", __FILE__, __LINE__, \
					__FUNCTION__); \
	\
 */
#define CASSERTION(condition, message, ...) BEGIN \
	CASSERTION_LOG("test"); \
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
		fprintf(STREAM,  OK "\n"); \
		cassertion.passed_assertions++; \
	} else { \
		fprintf(STREAM, FAIL "\n"); \
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

/*
 * If condition is true, the test suite will not run.
 */
#define CASSERTION_DONTRUN(condition, message, ...) BEGIN \
	if (condition) { \
		CASSERTION_LOG("dontrun"); \
		fprintf(STREAM, "%s|" message "|", #condition, __VA_ARGS__); \
		cassertion.dontrun = 1; \
		cassertion.dontrun_assertions++; \
	} else { \
		cassertion.dontrun = 0; \
	} \
	END

#define CASSERTION_RESULTS(void) BEGIN \
	fprintf(STREAM, "@cassertion_results|%s|" OK "|%d|" FAIL "|%d|dontrun|%d\n", \
		__FILE__, cassertion.passed_assertions, \
		cassertion.failed_assertions, cassertion.dontrun_assertions); \
	END

#endif /* CASSERION_H_ */
