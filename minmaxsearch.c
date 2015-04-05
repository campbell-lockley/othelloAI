/* ****************************************************************************************************************** *
 * Name:	minmaxsearch.c
 * Description:	A generic min-max algorithm which finds the next best move for a given game when provided a successor 
 * 		function, a terminal test function and a utility function.
 * Author:	Campbell Lockley		studentID: 1178618
 * Date:	31/03/15
 * ****************************************************************************************************************** */

/* ******** *
 * Includes *
 * ******** */
#include <stdlib.h>

#include "minmaxsearch.h"

/* ******* *
 * Defines *
 * ******* */

/* ********* *
 * Functions *
 * ********* */
char *minmax_decision(STATE *state) {
	return NULL;
}

int max_value(STATE *state) {
	return 0;
}

int min_value(STATE *state) {
	return 0;
}

char *minmaxsearch_start(MMSearch *search, STATE *state) {
	return "minmaxsearch not working yet";
}

MMSearch *minmaxsearch_init(int time, int (*utility)(STATE *state), bool (*terminal_test)(STATE *state),
			     Filo *(*successors)(STATE *state)) {
	MMSearch *search;
	
	search = (MMSearch *)calloc(1, sizeof(MMSearch));
	search->time = time;
	search->utility = utility;
	search->terminal_test = terminal_test;
	search->successors = successors;
	
	return search;
}
