#ifndef TESTS_H
#define TESTS_H

#include <stdio.h>
#include <stdlib.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define FAILED_PREFIX KCYN "%s" KNRM ":" KYEL "%d " KMAG "FAILED: " KNRM

#define TEST_BEGIN()\
	int _nb_fail = 0; \
	int _total = 0;\


// use local to avoid muti-function call when arg is function

#define assertInt(actual, expected) \
do \
{ \
	int _actual = (int)(actual);\
	int _expected = (int)(expected); \
	_total++; \
	if (_actual != _expected) \
	{ \
		fprintf(stdout, FAILED_PREFIX "expected %d (0x%02x), got %d (0x%02x)\n", \
				__FILE__, __LINE__, _expected, _expected, _actual, _actual); \
		_nb_fail++; \
	} \
} \
while(0)

#define assertArray(actual, expected, size) \
do \
{ \
	unsigned int _i; \
	const char * _actual = (const char*)(actual); \
	const char * _expected = (const char *)(expected); \
	unsigned int _size = (unsigned int)(size);\
	_total++; \
	for (_i = 0; _i < _size; _i++) \
	{ \
		if (_actual[_i] != _expected[_i]) \
		{ \
			fprintf(stdout, FAILED_PREFIX "at position %d expected %d (0x%02x), got %d (0x%02x)\n", \
					__FILE__, __LINE__, _i, (int)(_expected[_i]), (int)(_expected[_i]), (int)(_actual[_i]), (int)(_actual[_i])); \
			_nb_fail++; \
			break; \
		} \
	} \
} \
while(0)

#define assertString(actual, expected) \
do \
{ \
	unsigned int _i; \
	const char * _actual = (const char*)(actual); \
	const char * _expected = (const char *)(expected); \
	_total++; \
	for (_i = 0; _actual[_i] && _expected[_i]; _i++) \
	{ \
		if (_actual[_i] != _expected[_i]) break; \
	} \
	if (_actual[_i] != '\0' || _expected[_i] != '\0') \
	{ \
		fprintf(stdout, FAILED_PREFIX "expected \"%s\", got \"%s\"\n", \
				__FILE__, __LINE__, _expected, _actual); \
		_nb_fail++; \
	} \
} \
while(0)

#define TEST_END()\
do \
{ \
	if (_nb_fail > 0) \
	{ \
		fprintf(stdout, KCYN "%-40s" KNRM KRED "TEST FAILED: " KNRM "failed %d, pass %d, total %d\n", \
				__FILE__ ":", _nb_fail, _total - _nb_fail, _total); \
		exit(EXIT_FAILURE); \
	} \
	else \
	{ \
		fprintf(stdout, KCYN "%-40s" KNRM KGRN "TEST SUCESS: " KNRM "failed %d, pass %d, total %d\n", \
				__FILE__ ":", _nb_fail, _total - _nb_fail, _total); \
		exit(EXIT_SUCCESS); \
	} \
} \
while(0)

#endif /*TESTS_H*/
