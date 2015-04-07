/* ****************************************************************************************************************** *
 * Name:	othelloAI.c
 * Description:	A simple move generator that uses an AI algorithm to choose an intelligent next move for the game 
 * 		othello within a specified time limit.
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
#define ICONV(x, y)	((y) * BOARD_DIM + (x))
#define FLIP(c)		(((c) == WHITE) ? BLACK : WHITE)

/* ********** *
 * Prototypes *
 * ********** */
PUBLIC Action *compute_move  (State *state, int time);

PRIVATE Filo  *actions       (State *state);
PRIVATE State *result        (Action *a, State *state);
PRIVATE int    utility       (State *state);
PRIVATE bool   terminal_test (State *state);
PRIVATE Filo  *successors    (State *state);
PRIVATE void   free_action   (Action *a);
PRIVATE void   free_state    (State *a);

PRIVATE State *move          (State *state, int x, int y);
PRIVATE State *capture       (State *state, State *successor, int x, int y, int dx, int dy);
PRIVATE void   init          (State *initial_state, int *time);
PRIVATE void   print_state   (State *state);
PRIVATE State *state_copy    (State *state);
PRIVATE void   free_actions  (Filo *actions_list);

/* ******* *
 * Globals *
 * ******* */
PRIVATE int expand_count = 0;
PRIVATE char axis_convert[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

/* ********* *
 * Functions *
 * ********* */
int main(int argc, char *argv[]) {
	State initial_state;						/* Initial state read from stdin	      */
	int time;							/* Time limit for algorithm		      */
	Action *a = NULL;
	printf("Hello World from othello AI\n");
	
	/* Get initial state from stdin */
	init(&initial_state, &time);
	print_state(&initial_state);
	printf("%d\n", time);
	
	/* Test FILO */
	/*char s1[] = "first";
	char s2[] = "second";
	char s3[] = "third";
	Filo *filo;
	filo_init(&filo);
	filo_push(&filo, s1);
	filo_push(&filo, s2);
	filo_push(&filo, s3);
	printf("%d\n", filo_size(&filo));
	printf("filo is %sempty\n", (filo_isEmpty(&filo) ? "" : "not "));*/
	
	/* Test Problem Domain Functions */
	/*printf("utility = %d\n", utility(&initial_state));
	printf("terminal_test = %s\n", (terminal_test(&initial_state) ? "yes" : "no"));
	Filo *slist;
	State *state;
	slist = successors(&initial_state);
	while (!filo_isEmpty(&slist)) {
		state = (State *)filo_pop(&slist);
		print_state(state);
	}*/
	
	/* Misc. Testing */
	
	/* Compute next move */
	a = compute_move(&initial_state, time);
	if (a == NULL) printf("Move: no move found\n");
	else {
		printf("Move: (%c,%d)\n", axis_convert[a->x], (a->y) + 1);
		print_state(a->state);
	}
	printf("expand count = %d\n", expand_count);
	if (a != NULL) free_action(a);
	
	return 0;
}

PUBLIC Action *compute_move(State *state, int time) {
	minmaxsearch_init(time, (Filo *(*)(void *))actions, 
	                  (void *(*)(void *, void *))result, 
	                  (int(*)(void *))utility, 
	                  (bool(*)(void *))terminal_test, 
	                  (Filo *(*)(void *))successors,
	                  (void(*)(void *))free_action,
	                  (void(*)(void *))free_state);
	return minmax_decision(state);
}

PRIVATE Filo *actions(State *state) {
	int x, y;
	Action *a;
	State *result;
	Filo *action_list;
	filo_init(&action_list);
	
	/*printf("%s\n", (state->colour == WHITE) ? "white" : "black");*/
	
	for (y = 0; y < BOARD_DIM; y++) {
		for (x = 0; x < BOARD_DIM; x++) {
			result = move(state, x, y);
			if (result != NULL) {
				/*printf("a:(%c,%d) ",axis_convert[x], y+1);*/
				a = (Action *)calloc(1, sizeof(Action));
				a->x = x;
				a->y = y;
				a->state = result;
				filo_push(&action_list, a);
			}
		}
	}
	/*printf("\n");*/
	
	return action_list;
}

PRIVATE State *result(Action *a, State *state) {
	return a->state;
}

PRIVATE int utility(State *state) {
	int i, white = 0, black = 0;
	
	for (i = 0; i < BOARD_SIZE; i++) {
		if (state->board[i] == WHITE) white++;
		else if (state->board[i] == BLACK) black++;
	}
	
	if (terminal_test(state)) {
		if (white > black) white += 100;
		else if (black > white) black += 100;
	}
	
	return (state->colour == WHITE) ? (white - black) : (black - white);
}

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

PRIVATE Filo *successors(State *state) {
	int x, y;
	State *successor;
	Filo *successor_list;
	filo_init(&successor_list);
	
	expand_count++;
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

PRIVATE void free_action(Action *a) {
	free(a->state);
	free(a);
}

PRIVATE void free_state(State *state) {
	free(state);
}

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
	
	/* If valid move found, place piece and update colour to enemy */
	if (successor != NULL) {
		successor->board[ICONV(x, y)] = state->colour;
		successor->colour = FLIP(state->colour);
	}
	
	return successor;
}

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
	/*printf("(x_pos,y_pos)=(%c,%d) (x_max,y_max)=(%c,%d) dx = %d, dy = %d\n", axis_convert[x+dx], y+dy+1, axis_convert[x_max], y_max+1, dx, dy);*/
	for (x_pos = x + dx, y_pos = y + dy; (x_pos != x_max) || (y_pos != y_max); x_pos += dx, y_pos += dy) {
		successor->board[ICONV(x_pos, y_pos)] = state->colour;
	}
	
	return successor;
}

PRIVATE void init(State *initial_state, int *time) {
	int i, j, index, ign, error = 0;
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

	if (scanf("- abcdefgh\n") == EOF) error = 1;
	for (i = 0, index = 0; i < BOARD_DIM; i++) {
		if (scanf("%d ", &ign) == EOF) error = 1;
		for (j = 0; j < BOARD_DIM; j++, index++) {
			if (scanf("%c", &c) == EOF) error = 1;
			initial_state->board[index] = c;
		}
		scanf("\n");
	}
	if (scanf(" %c %d\n", &(initial_state->colour), time) == EOF) error = 1;
	
	if (error) {
		printf("Error: incomplete board\n");
		exit(1);
	}
	
	return;
}

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

PRIVATE State *state_copy(State *state) {
	int i;
	State *copy;
	
	copy = (State *)calloc(1, sizeof(State));
	for (i = 0; i < BOARD_SIZE; i++) copy->board[i] = state->board[i];
	copy->colour = state->colour;
	
	return copy;
}

PRIVATE void free_actions(Filo *actions_list) {
	Action *a;
	
	while (!filo_isEmpty(&actions_list)) {
		a = filo_pop(&actions_list);
		free_action(a);
	}
}
