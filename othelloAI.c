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

#include "filo.h"
#include "minmaxsearch.h"

/* ******* *
 * Defines *
 * ******* */
#define PRIVATE		static
#define PUBLIC
#define BOARD_DIM	8
#define BOARD_SIZE	(BOARD_DIM * BOARD_DIM)
#define ICONV(x, y)	((y) * BOARD_DIM + (x))
#define EMPTY		'.'
#define BLACK		'B'
#define WHITE		'O'

/* ******** *
 * Typedefs *
 * ******** */
typedef struct State {
	char colour;
	char board[BOARD_SIZE];
} State;

/* ********** *
 * Prototypes *
 * ********** */
PUBLIC char *compute_move  (State *state, int time);
PUBLIC int   utility       (State *state);
PUBLIC bool  terminal_test (State *state);
PUBLIC Filo *successors    (State *state);

PRIVATE State *move        (State *state, int x, int y);
PRIVATE State *capture     (State *state, State *successor, int x, int y, int dx, int dy);
PRIVATE void   init        (State *initial_state, int *time);
PRIVATE void   print_board (char * board);
PRIVATE State *state_copy  (State *state);

/* ********* *
 * Functions *
 * ********* */
int main(int argc, char *argv[]) {
	State initial_state;						/* Initial state read from stdin	      */
	int time;							/* Time limit for algorithm		      */
	char *result;
	printf("Hello World from othello AI\n");
	
	/* Get initial state from stdin */
	init(&initial_state, &time);
	print_board(initial_state.board);
	printf("%c %d\n", initial_state.colour, time);
	
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
	printf("utility = %d\n", utility(&initial_state));
	Filo *slist;
	State *state;
	slist = successors(&initial_state);
	while (!filo_isEmpty(&slist)) {
		state = (State *)filo_pop(&slist);
		print_board(state->board);
		printf("terminal = %s\n", (terminal_test(state)) ? "true" : "false");
		printf("utility = %d\n", utility(state));
	}
	
	/* Compute next move */
	result = compute_move(&initial_state, time);
	printf("%s\n", result);
	
	return 0;
}

PUBLIC char *compute_move(State *state, int time) {
	return "No move yet";
}

PUBLIC int utility(State *state) {
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

PUBLIC bool terminal_test(State *state) {
	State *opponent;
	Filo *successor_list;
	
	/* Check if we have possible moves */
	successor_list = successors(state);
	if (!filo_isEmpty(&successor_list)) {
		filo_destroy(&successor_list);
		return false;
	}
	
	filo_destroy(&successor_list);
	opponent = state_copy(state);
	if (state->colour == WHITE) opponent->colour = BLACK;
	else opponent->colour = WHITE;
	
	/* Check if opponent has possible moves */
	successor_list = successors(opponent);
	if (!filo_isEmpty(&successor_list)) {
		filo_destroy(&successor_list);
		return false;
	}
	
	filo_destroy(&successor_list);
	return true;
}

PUBLIC Filo *successors(State *state) {
	int x, y;
	State *successor;
	Filo *successor_list;
	filo_init(&successor_list);
	
	for (y = 0; y < BOARD_DIM; y++) {
		for (x = 0; x < BOARD_DIM; x++) {
			successor = move(state, x, y);
			if (successor != NULL) filo_push(&successor_list, successor);
		}
	}
	
	return successor_list;
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
	
	/* If valid move found, place piece */
	if (successor != NULL) successor->board[ICONV(x, y)] = state->colour;
	
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
	     (x_pos >= 0) || (x_pos < BOARD_DIM) || (y_pos >= 0) || (y_pos < BOARD_DIM);
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

PRIVATE void print_board(char *board) {
	int x, y, index;
	
	printf("- abcdefgh\n");
	for (y = 0, index = 0; y < BOARD_DIM; y++) {
		printf("%d ", y+1);
		for (x = 0; x < BOARD_DIM; x++, index++) printf("%c", board[index]);
		printf("\n");
	}
}

PRIVATE State *state_copy(State *state) {
	int i;
	State *copy;
	
	copy = (State *)calloc(1, sizeof(State));
	for (i = 0; i < BOARD_SIZE; i++) copy->board[i] = state->board[i];
	copy->colour = state->colour;
	
	return copy;
}
