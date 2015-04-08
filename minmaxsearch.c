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
#include <stdio.h>
#include <limits.h>

#include "minmaxsearch.h"

/* ******* *
 * Defines *
 * ******* */
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

/* ********** *
 * Prototypes *
 * ********** */
extern int max_value(STATE *state);
extern int min_value(STATE *state);
 
/* ******* *
 * Globals *
 * ******* */
bool ready = false;							/* True if minmaxsearch_init run correctly    */
int  time;								/* Time constraint on minmaxsearch	      */

/* These are the problem domain functions required by minmaxsearch */
Filo  *(*mmsearch_actions)       (STATE *state)            = NULL;
STATE *(*mmsearch_result)        (ACTION *a, STATE *state) = NULL;
int    (*mmsearch_utility)       (STATE *state)            = NULL;
bool   (*mmsearch_terminal_test) (STATE *state)            = NULL;
Filo  *(*mmsearch_successors)    (STATE *state)            = NULL;
void   (*mmsearch_free_action)   (ACTION *a)               = NULL;
void   (*mmsearch_free_state)    (STATE *a)                = NULL;

/* ********* *
 * Functions *
 * ********* */
/*
 * Sets required values for minmaxsearch. Must be called before minmax_decision().
 */
void minmaxsearch_init(int runtime, Filo *(*actions)(STATE *state), STATE *(*result)(ACTION *a, STATE *state), 
		       int (*utility)(STATE *state), bool (*terminal_test)(STATE *state), 
		       Filo *(*successors)(STATE *state), void(*free_action)(ACTION *a), 
		       void(*free_state)(STATE *state)) {
		       
	/* Check for valid args */
	if (runtime <= 0 || !actions || !result || !utility || !terminal_test || !successors || !free_action || !free_state) {
		ready = false;
		printf("%s\n", "minmaxsearch passed incompatable arguments");
		return;
	} else printf("minmaxsearch will run for a maximum of %d seconds\n", runtime);
	
	/* Use provided functions as the problem domain functions for this search */
	time                   = runtime;
	mmsearch_actions       = actions;
	mmsearch_result        = result;
	mmsearch_utility       = utility;
	mmsearch_terminal_test = terminal_test;
	mmsearch_successors    = successors;
	mmsearch_free_action   = free_action;
	mmsearch_free_state    = free_state;
	
	ready = true;
}

/*
 * Start a minmax search with STATE *state as the initial state. Returns best action found by this search.
 */
ACTION *minmax_decision(STATE *state) {
	Filo *actions;
	ACTION *a, *best = NULL;
	int min, max = INT_MIN;						/* -INF for int				      */
	
	/* Need to have problem domain functions before starting a search */
	if (!ready) {
		printf("%s\n", "minmaxsearch is not ready");
		return NULL;
	}
	
	/* Get possible actions */
	actions = mmsearch_actions(state);
	
	/* Find best action */
	while (!filo_isEmpty(&actions)) {
		a = filo_pop(&actions);
		
		min = min_value(mmsearch_result(a, state));		/* Start recursive minmax search	      */
		
		if (min > max) {
			max = min;
			if (best != NULL) mmsearch_free_action(best);
			best = a;
		} else mmsearch_free_action(a);
	}
	
	return best;							/* Return best action found		      */
}

/*
 * Evaluates a max step in the search. Passes up the max of its children.
 */
int max_value(STATE *state) {
	Filo *successors;
	STATE *successor;
	int min, v = INT_MIN;						/* -INF for int				      */
	
	/* If terminal state stop searching and pass up value of this state */
	if (mmsearch_terminal_test(state)) return mmsearch_utility(state);
	
	/* Expand current state */
	successors = mmsearch_successors(state);
	
	/* Find max of children */
	while (!filo_isEmpty(&successors)) {
		successor = filo_pop(&successors);
		min = min_value(successor);				/* Perform next min step		      */
		v = MAX(v, min);
		mmsearch_free_state(successor);
	}
	
	return v;							/* Return max of children		      */
}

/*
 * Evaluates a min step in the search. Passes up the min of its children.
 */
int min_value(STATE *state) {
	Filo *successors;
	STATE *successor;
	int max, v = INT_MAX;						/* +INF for int				      */
	
	/* If terminal state stop searching and pass up value of this state */
	if (mmsearch_terminal_test(state)) return mmsearch_utility(state);
	
	/* Expand current state */
	successors = mmsearch_successors(state);
	
	/* Find min of children */
	while (!filo_isEmpty(&successors)) {
		successor = filo_pop(&successors);
		max = max_value(successor);				/* Perform next min step		      */
		v = MIN(v, max);
		mmsearch_free_state(successor);
	}
	
	return v;							/* Return min of children		      */
}
