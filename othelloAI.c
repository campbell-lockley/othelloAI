/* ****************************************************************************************************************** *
 * Name:	othelloAI.c
 * Description:	A simple move generator that uses a minmax game tree algorithm to choose an intelligent next move for 
 * 		an AI othello player. To start a search run compute_move(). The search will run untill the optimal 
 * 		solution is found or untill the time limit expires. The best move found so far will then be returned.
 * Author:	Campbell Lockley		studentID: 1178618
 * Date:	31/03/15
 * ****************************************************************************************************************** */

/* ******** *
 * Includes *
 * ******** */
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "othelloAI.h"
#include "filo.h"
#include "minmaxsearch.h"

/* ******* *
 * Defines *
 * ******* */
#define PRIVATE		static
#define PUBLIC
#define ICONV(x, y)	((y) * BOARD_DIM + (x))				/* Convert (x,y) to 1d array index	      */
#define FLIP(c)		(((c) == WHITE) ? BLACK : WHITE)

/* ********** *
 * Prototypes *
 * ********** */
/* Runs othelloAI to compute best next move									      */
PUBLIC Action *compute_move  (State *state, int time);

/* These are othello specific implementatons of the problem domain functions required by minmaxsearch		      */
PRIVATE Filo  *actions       (State *state);
PRIVATE State *result        (Action *a, State *state);
PRIVATE int    utility       (State *state);
PRIVATE bool   terminal_test (State *state);
PRIVATE Filo  *successors    (State *state);
PRIVATE void   set_estimate  (Action *a, int estimate);
PRIVATE void   free_action   (Action *a);
PRIVATE void   free_state    (State *a);

/* Utility functions for othelloAI										      */
PRIVATE State *move          (State *state, int x, int y);
PRIVATE State *capture       (State *state, State *successor, int x, int y, int dx, int dy);
PRIVATE bool   scan_state    (State *state, int *time);
PRIVATE void   print_state   (State *state);
PRIVATE State *state_copy    (State *state);
PRIVATE void   free_actions  (Filo *actions_list);

/* ******* *
 * Globals *
 * ******* */
PRIVATE char ai_colour;
PRIVATE int  expand_count = 0;
PRIVATE char axis_convert[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

/* ********* *
 * Functions *
 * ********* */
/*
 * Reads a board state from stdin and computes a next move, printing move and details to stdout.
 */
int main(int argc, char *argv[]) {
	State initial_state;						/* Initial state read from stdin	      */
	int time;							/* Time limit for algorithm		      */
	Action *a = NULL;
	
	/* Get initial state from stdin */
	if(!scan_state(&initial_state, &time)) return 1;
	
	/* Compute next move */
	a = compute_move(&initial_state, time);
	if (a != NULL) {
		printf("move %c %d nodes %d depth %d minmax %d\n", 
		       axis_convert[a->x], (a->y) + 1, expand_count, minmax_get_depth(), a->estimate);
		free_action(a);
	} else printf("move a -1 nodes 0 depth 0 minmax 0\n");
	
	return 0;
}

/*
 * Runs othelloAI to compute best next move.
 */
PUBLIC Action *compute_move(State *state, int time) {
	ai_colour = state->colour;
	minmaxsearch_init(time, (Filo *(*)(void *))actions, 		/* Must first init minmaxsearch		      */
	                  (void *(*)(void *, void *))result, 
	                  (int(*)(void *))utility, 
	                  (bool(*)(void *))terminal_test, 
	                  (Filo *(*)(void *))successors,
	                  (void(*)(void *, int))set_estimate,
	                  (void(*)(void *))free_action,
	                  (void(*)(void *))free_state);
	return minmax_decision(state);
}

/*
 * Finds possible actions for a state.
 */
PRIVATE Filo *actions(State *state) {
	int x, y;
	Action *a;
	State *result;
	Filo *action_list;
	
	filo_init(&action_list);
	
	/* Check every board position for a possible move */
	for (y = 0; y < BOARD_DIM; y++) {
		for (x = 0; x < BOARD_DIM; x++) {
			result = move(state, x, y);			/* Pre-calculate the result of an action      */
			if (result != NULL) {
				a = (Action *)calloc(1, sizeof(Action));
				a->x = x;
				a->y = y;
				a->state = result;
				filo_push(&action_list, a);
			}
		}
	}
	
	return action_list;
}

/*
 * Returns result of an action on a state.
 */
PRIVATE State *result(Action *a, State *state) {
	return a->state;						/* Result has been pre-calculated	      */
}

/*
 * Returns utility value for a state.
 */
PRIVATE int utility(State *state) {
	int i, white = 0, black = 0;
	char c;
	
	for (i = 0; i < BOARD_SIZE; i++) {
		c = state->board[i];
		if (c == WHITE) white++;
		else if (c == BLACK) black++;
	}
	
	if (terminal_test(state)) {
		if (white > black) white += 100;
		else if (black > white) black += 100;
	}
	
	return (ai_colour == WHITE) ? (white - black) : (black - white);
}

/*
 * Tests for a terminal state.
 */
PRIVATE bool terminal_test(State *state) {
	State *opponent;
	Filo *actions_list;
	
	/* Check if we have possible moves */
	actions_list = actions(state);
	if (!filo_isEmpty(&actions_list)) {
		free_actions(actions_list);
		return false;
	}
	
	free_actions(actions_list);
	opponent = state_copy(state);
	if (state->colour == WHITE) opponent->colour = BLACK;
	else opponent->colour = WHITE;
	
	/* Check if opponent has possible moves */
	actions_list = actions(opponent);
	if (!filo_isEmpty(&actions_list)) {
		free_actions(actions_list);
		free_state(opponent);
		return false;
	}
	
	free_actions(actions_list);
	free_state(opponent);
	return true;
}

/*
 * Expands a state.
 */
PRIVATE Filo *successors(State *state) {
	int x, y;
	State *successor;
	Filo *successor_list;
	
	expand_count++;
	filo_init(&successor_list);
	
	/* Check every board position for a possible move which will result in a new state */
	for (y = 0; y < BOARD_DIM; y++) {
		for (x = 0; x < BOARD_DIM; x++) {
			successor = move(state, x, y);
			if (successor != NULL) filo_push(&successor_list, successor);
		}
	}
	
	/* If no move possible, pass */
	if (filo_isEmpty(&successor_list)) {
		successor = state_copy(state);
		successor->colour = FLIP(successor->colour);
		filo_push(&successor_list, successor);
	}
	
	return successor_list;
}

/*
 * Updates minmax estimate of an action.
 */
PRIVATE void set_estimate(Action *a, int estimate) {
	if (a != NULL) a->estimate = estimate;
}

/*
 * Safely frees an action.
 */
PRIVATE void free_action(Action *a) {
	free(a->state);
	free(a);
}

/*
 * Safely frees a state.
 */
PRIVATE void free_state(State *state) {
	free(state);
}

/*
 * Checks if (x,y) is a possible move, returning the resulting state if it is a move.
 */
PRIVATE State *move(State *state, int x, int y) {
	bool left = false, right = false, up = false, down = false;
	State *successor = NULL;
	
	/* Can't move if position is not empty */
	if (state->board[ICONV(x, y)] != EMPTY) return NULL;
	
	/* Check for possible captures */
	if (!(x == 0)) {		/* SEARCH LEFT */
		left = true;
		successor = capture(state, successor, x, y, -1, 0);
	}
	if (!(x == (BOARD_DIM - 1))) {	/* SEARCH RIGHT */
		right = true;
		successor = capture(state, successor, x, y, +1, 0);
	}
	if (!(y == 0)) {		/* SEARCH UP */
		up = true;
		successor = capture(state, successor, x, y, 0, -1);
	}
	if (!(y == (BOARD_DIM - 1))) {	/* SEARCH DOWN */
		down = true;
		successor = capture(state, successor, x, y, 0, +1);
	}
	if (left && up) {		/* SEARCH UP & LEFT */
		successor = capture(state, successor, x, y, -1, -1);
	}
	if (right && up) {		/* SEARCH UP & RIGHT */
		successor = capture(state, successor, x, y, +1, -1);
	}
	if (left && down) {		/* SEARCH DOWN & LEFT */
		successor = capture(state, successor, x, y, -1, +1);
	}
	if (right && down) {		/* SEARCH DOWN & RIGHT */
		successor = capture(state, successor, x, y, +1, +1);
	}
	
	/* If valid move found, place piece and update colour to enemy's' */
	if (successor != NULL) {
		successor->board[ICONV(x, y)] = state->colour;
		successor->colour = FLIP(state->colour);
	}
	
	return successor;
}

/*
 * Checks in a single direction for pieces which can be captured and captures any capturable pieces.
 */
PRIVATE State *capture(State *state, State *successor, int x, int y, int dx, int dy) {
	int x_pos, y_pos, x_max = x, y_max = y;
	char enemy, c;
	bool valid = false;
	
	if (state->colour == WHITE) enemy = BLACK;
	else enemy = WHITE;
	
	/* Early exit if first piece not an enemies */
	if (state->board[ICONV(x + dx, y + dy)] != enemy) return successor;
	
	/* Find flanking piece */
	for (x_pos = x + dx, y_pos = y + dy;
	     (x_pos >= 0) && (x_pos < BOARD_DIM) && (y_pos >= 0) && (y_pos < BOARD_DIM);
	     x_pos += dx, y_pos += dy) {
	     
		c = state->board[ICONV(x_pos, y_pos)];
		if (c == enemy) continue;
		if (c == EMPTY) break;
		if (c == state->colour) {
			valid = true;
			x_max = x_pos;
			y_max = y_pos;
			break;
		}
	}
	
	/* If no flanking piece, return board unchanged */
	if (!valid) return successor;
	
	/* Wait untill last possible moment to create successor board */
	if (successor == NULL) successor = state_copy(state);
	
	/* Flip flanked pieces */
	for (x_pos = x + dx, y_pos = y + dy; (x_pos != x_max) || (y_pos != y_max); x_pos += dx, y_pos += dy) {
		successor->board[ICONV(x_pos, y_pos)] = state->colour;
	}
	
	return successor;
}

/*
 * Attempts to read a state from stdin.
 */
PRIVATE bool scan_state(State *state, int *time) {
	int i, j, index, ign;
	bool success = true;
	char c;
	
	/* Board shoud be of the form:
		- abcdefgh
		1 ........
		2 ........
		3 ........
		4 ...OB...
		5 ...BO...
		6 ........
		7 ........
		8 ........
		B 60			*/

	if (scanf("- abcdefgh\n") == EOF) success = false;
	for (i = 0, index = 0; i < BOARD_DIM; i++) {
		if (scanf("%d ", &ign) == EOF) success = false;
		for (j = 0; j < BOARD_DIM; j++, index++) {
			if (scanf("%c", &c) == EOF) success = false;
			state->board[index] = c;
		}
		scanf("\n");
	}
	if (scanf(" %c %d\n", &(state->colour), time) == EOF) success = false;
	
	if (!success) printf("Error: incomplete board\n");
	
	return success;
}

/*
 * Prints a state to stdout.
 */
PRIVATE void print_state(State *state) {
	int x, y, index;
	
	printf("- abcdefgh\n");
	for (y = 0, index = 0; y < BOARD_DIM; y++) {
		printf("%d ", y+1);
		for (x = 0; x < BOARD_DIM; x++, index++) printf("%c", state->board[index]);
		printf("\n");
	}
	printf("%c\n", state->colour);
}

/*
 * Returns a copy of a state.
 */
PRIVATE State *state_copy(State *state) {
	int i;
	State *copy;
	
	copy = (State *)calloc(1, sizeof(State));
	for (i = 0; i < BOARD_SIZE; i++) copy->board[i] = state->board[i];
	copy->colour = state->colour;
	
	return copy;
}

/*
 * Safely frees a Filo<Action>.
 */
PRIVATE void free_actions(Filo *actions_list) {
	Action *a;
	
	while (!filo_isEmpty(&actions_list)) {
		a = filo_pop(&actions_list);
		free_action(a);
	}
}
