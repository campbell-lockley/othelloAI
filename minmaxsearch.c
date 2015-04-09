/* ****************************************************************************************************************** *
 * Name:	minmaxsearch.c
 * Description:	A generic min-max algorithm which finds the next best move for a given game when provided with the 
 * 		required problem domain specific functions. These functions are:
 * 			actions(STATE)
 * 				Returns a Filo<ACTION> containing the actions which are possible to perform on this 
 * 				state.
 * 			result(ACTION, STATE)
 * 				Applies the action to the state, returning the resulting state.
 * 			utility(STATE)
 * 				Calculates a heuristic value for this state.
 * 			terminal_test(STATE)
 * 				Returns true if this state is a terminal state, false otherwise.
 * 			successors(STATE)
 * 				Returns a Filo<STATE> containing the the children of this state, being the states 
 * 				resulting from every possible action being performed on this state.
 * 
 * 		And some non-algorithm utility functions:
 * 			set_estimate(ACTION, estimate)
 * 				Updates the estimate value associated with this action.
 * 			free_action(ACTION)
 * 				Safely frees an action.
 * 			free_state(STATE)
 * 				Safely frees a state.
 * 
 * 		There must also be an external definition of a STATE and an ACTION which is used by the suppied problem
 * 		domain specific functions.
 * 
 * 		A search is started by calling minmax_decision() with the initial state of this search.
 * 		Important! minmaxsearch_init() must be run before minmax_decision() with valid arguments.
 * 
 * 		minmaxsearch will find an optimal solution iff it completes its search within the time specified to 
 * 		minmaxsearch_init(). Otherwise it will return the best solution found so far.
 * 
 * 		The efficiency of this search depends upon the performance of the problem domain functions, however 
 * 		this implementation of a minmax search is tuned with alpha-beta pruning to optimise performance.
 * 			
 * Author:	Campbell Lockley		studentID: 1178618
 * Date:	31/03/15
 * ****************************************************************************************************************** */

/* ******** *
 * Includes *
 * ******** */
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "minmaxsearch.h"

/* ******* *
 * Defines *
 * ******* */
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

/* ********** *
 * Prototypes *
 * ********** */
extern int max_value(STATE *state, int alpha, int beta, int depth);
extern int min_value(STATE *state, int alpha, int beta, int depth);
 
/* ******* *
 * Globals *
 * ******* */
bool   ready = false;							/* True if minmaxsearch_init run correctly    */
int    depth_limit = 0;							/* Depth reached by last minmax_decision()    */
bool   done;								/* Optimal solution found before time limit   */
double time_limit;							/* Time constraint on minmaxsearch	      */
time_t start_time;							/* Time search started			      */
bool   timeout;								/* Search timed out			      */

/* These are the problem domain functions required by minmaxsearch */
Filo  *(*mmsearch_actions)       (STATE *state)            = NULL;	/* Finds possible actions for a state	      */
STATE *(*mmsearch_result)        (ACTION *a, STATE *state) = NULL;	/* Returns result of an action on a state     */
int    (*mmsearch_utility)       (STATE *state)            = NULL;	/* Returns utility value for a state	      */
bool   (*mmsearch_terminal_test) (STATE *state)            = NULL;	/* Tests for a terminal state		      */
Filo  *(*mmsearch_successors)    (STATE *state)            = NULL;	/* Expands a state			      */
void   (*mmsearch_set_estimate)  (ACTION *a, int estimate) = NULL;	/* Updates minmax estimate of an action	      */
void   (*mmsearch_free_action)   (ACTION *a)               = NULL;	/* Safely frees an action		      */
void   (*mmsearch_free_state)    (STATE *a)                = NULL;	/* Safely frees a state			      */

/* ********* *
 * Functions *
 * ********* */
/*
 * Sets required values for minmaxsearch. Must be called before minmax_decision().
 */
void minmaxsearch_init(int time, Filo *(*actions)(STATE *state), STATE *(*result)(ACTION *a, STATE *state), 
		       int (*utility)(STATE *state), bool (*terminal_test)(STATE *state), 
		       Filo *(*successors)(STATE *state), void (*set_estimate)(ACTION *a, int estimate),
		       void(*free_action)(ACTION *a), void(*free_state)(STATE *state)) {
		       
	/* Check for valid args */
	if (time <= 0 || !actions || !result || !utility || !terminal_test || !successors || !set_estimate 
	    || !free_action || !free_state) {
		ready = false;
		return;
	}
	
	/* Use provided functions as the problem domain functions for this search */
	time_limit             = (double)time;
	mmsearch_actions       = actions;
	mmsearch_result        = result;
	mmsearch_utility       = utility;
	mmsearch_terminal_test = terminal_test;
	mmsearch_successors    = successors;
	mmsearch_set_estimate  = set_estimate;
	mmsearch_free_action   = free_action;
	mmsearch_free_state    = free_state;
	
	ready = true;
}

/*
 * Start a minmax search with STATE *state as the initial state. Returns best action found by this search.
 */
ACTION *minmax_decision(STATE *state) {
	Filo *actions, *node;
	ACTION *a, *best = NULL, *curr_best = NULL;
	int v, alpha, beta, min;
	
	/* Need to have problem domain functions before starting a search */
	if (!ready) return NULL;
	
	/* Start the clock */
	start_time = time(&start_time);
	timeout = false;
	
	/* Get possible actions */
	actions = mmsearch_actions(state);
	
	/* Find best action  using iterative deepening */
	depth_limit = 0;							/* Start at min depth		      */
	done = false;
	while (!done) {
		depth_limit++;
		node = actions;							/* Restart search at root	      */
		done = true;
		curr_best = NULL;
		v = INT_MIN;							/* -INF for int			      */
		alpha = INT_MIN;
		beta = INT_MAX;
		
		while (!filo_isEmpty(&node)) {
			a = node->value;
			
			/* Start recursive minmax search */
			min = min_value(mmsearch_result(a, state), alpha, beta, 1);
			if (timeout) break;					/* Check for timeout		      */
		
			if (min > v) {
				v = min;
				curr_best = a;
			}
			
			alpha = MAX(alpha, v);
			
			node = node->next;
		}
		
		if (timeout) break;						/* Check for timeout		      */
		
		best = curr_best;						/* Update best			      */
		mmsearch_set_estimate(best, v);					/* Record new minmax estimate	      */
	}
	
	if (timeout) depth_limit--;
	
	return best;								/* Return best action found	      */
}

/*
 * Evaluates a max step in the search. Passes up the max of its children.
 */
int max_value(STATE *state, int alpha, int beta, int depth) {
	Filo *successors;
	STATE *successor;
	int min, v = INT_MIN;						/* -INF for int				      */
	time_t curr_time;
	
	/* Check for timeout */
	if (timeout) return INT_MIN;
	time(&curr_time);
	if (difftime(curr_time, start_time) >= time_limit) {
		timeout = true;
		return INT_MIN;
	}
	
	/* If depth limit reached stop searching and pass up value of this state */
	if (depth == depth_limit) {
		done = false;
		return mmsearch_utility(state);
	}
	
	/* If terminal state stop searching and pass up value of this state */
	if (mmsearch_terminal_test(state)) return mmsearch_utility(state);
	
	/* Expand current state */
	successors = mmsearch_successors(state);
	
	/* Find max of children */
	while (!filo_isEmpty(&successors)) {
		successor = filo_pop(&successors);
		min = min_value(successor, alpha, beta, depth + 1);	/* Perform next min step		      */
		v = MAX(v, min);
		alpha = MAX(alpha, v);
		mmsearch_free_state(successor);
		if (v >= beta) return v;
	}
	
	return v;							/* Return max of children		      */
}

/*
 * Evaluates a min step in the search. Passes up the min of its children.
 */
int min_value(STATE *state, int alpha, int beta, int depth) {
	Filo *successors;
	STATE *successor;
	int max, v = INT_MAX;						/* +INF for int				      */
	time_t curr_time;
	
	/* Check for timeout */
	if (timeout) return INT_MAX;
	time(&curr_time);
	if (difftime(curr_time, start_time) >= time_limit) {
		timeout = true;
		return INT_MAX;
	}
	
	/* If depth limit reached stop searching and pass up value of this state */
	if (depth == depth_limit) {
		done = false;
		return mmsearch_utility(state);
	}
	
	/* If terminal state stop searching and pass up value of this state */
	if (mmsearch_terminal_test(state)) return mmsearch_utility(state);
	
	/* Expand current state */
	successors = mmsearch_successors(state);
	
	/* Find min of children */
	while (!filo_isEmpty(&successors)) {
		successor = filo_pop(&successors);
		max = max_value(successor, alpha, beta, depth + 1);	/* Perform next min step		      */
		v = MIN(v, max);
		beta = MIN(beta, v);
		mmsearch_free_state(successor);
		if (v <= alpha) return v;
	}
	
	return v;							/* Return min of children		      */
}

/*
 * Returns the maximum depth reached by the last call to minmax_decision().
 */
int minmax_get_depth() {
	return depth_limit;
}
