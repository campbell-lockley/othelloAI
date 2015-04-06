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
 
/* ******* *
 * Globals *
 * ******* */
bool ready = false;
int time;

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
void minmaxsearch_init(int runtime, Filo *(*actions)(STATE *state), STATE *(*result)(ACTION *a, STATE *state), 
			    int (*utility)(STATE *state), bool (*terminal_test)(STATE *state), 
			    Filo *(*successors)(STATE *state), void(*free_action)(ACTION *a), 
			    void(*free_state)(STATE *state)) {
	if (runtime <= 0 || !actions || !result || !utility || !terminal_test || !successors || !free_action || !free_state) {
		ready = false;
		printf("%s\n", "minmaxsearch passed incompatable arguments");
		return;
	} else printf("minmaxsearch will run for a maximum of %d seconds\n", runtime);
	time = runtime;
	mmsearch_actions       = actions;
	mmsearch_result        = result;
	mmsearch_utility       = utility;
	mmsearch_terminal_test = terminal_test;
	mmsearch_successors    = successors;
	mmsearch_free_action   = free_action;
	mmsearch_free_state    = free_state;
	
	ready = true;
}

ACTION *minmax_decision(STATE *state) {
	Filo *actions;
	ACTION *a, *best = NULL;
	int min, max = INT_MIN;						/* -INF for int				      */
	
	if (!ready) {
		printf("%s\n", "minmaxsearch is not ready");
		return NULL;
	}
	
	actions = mmsearch_actions(state);
	
	while (!filo_isEmpty(&actions)) {
		a = filo_pop(&actions);
		min = min_value(mmsearch_result(a, state));
		/*printf("min_value = %d\n", min);*/
		if (min > max) {
			max = min;
			/*mmsearch_free_action(best);*/
			best = a;
		} else {
			/*mmsearch_free_action(a);*/
		}
	}
	
	return best;
}

int max_value(STATE *state) {
	Filo *successors;
	STATE *successor;
	int min, v = INT_MIN;
	
	if (mmsearch_terminal_test(state)) return mmsearch_utility(state);
	
	successors = mmsearch_successors(state);
	while (!filo_isEmpty(&successors)) {
		successor = filo_pop(&successors);
		min = min_value(successor);
		v = MAX(v, min);
		/*mmsearch_free_state(successor);*/
	}
	
	return v;
}

int min_value(STATE *state) {
	Filo *successors;
	STATE *successor;
	int max, v = INT_MAX;
	
	if (mmsearch_terminal_test(state)) return mmsearch_utility(state);
	
	successors = mmsearch_successors(state);
	while (!filo_isEmpty(&successors)) {
		successor = filo_pop(&successors);
		max = max_value(successor);
		/*printf("max_value = %d\n", max);*/
		v = MIN(v, max);
		/*mmsearch_free_state(successor);*/
	}
	
	return v;
}
