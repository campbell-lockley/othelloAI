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
#include <stdio.h>

/* ******* *
 * Defines *
 * ******* */
#define PRIVATE		static
#define BOARD_DIM	8
#define BOARD_SIZE	(BOARD_DIM * BOARD_DIM)
#define EMPTY		'.'
#define BLACK		'B'
#define WHITE		'O'

/* ******** *
 * Typedefs *
 * ******** */
/*typedef char Board[BOARD_SIZE];					representation of board			      */

/* ********** *
 * Prototypes *
 * ********** */


PRIVATE char *parseArgs(int argc, char *argv[]);
PRIVATE void print_board(char * board);

/* ******* *
 * Globals *
 * ******* */


/* ********* *
 * Functions *
 * ********* */
int main(int argc, char *argv[]) {
	char *startBoard;
	printf("Hello World from othello AI\n");
	
	startBoard = parseArgs(argc, argv);
	
	return 0;
}

PRIVATE char *parseArgs(int argc, char *argv[]) {
	int i, j, index, time, ign, error = 0;
	char c;
	char *board = (char *)calloc(BOARD_SIZE, sizeof(char));
	
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
	printf("- abcdefghi\n");
	for (i = 1, index = 0; i <= BOARD_DIM; i++, index++) {
		if (scanf("%d ", &ign) == EOF) error = 1;
		for (j = 0; j < BOARD_DIM; j++, index++) {
			if (scanf("%c", &c) == EOF) error = 1;
			board[index] = c;
		}
		scanf("\n");
	}
	print_board(board);
	if (scanf(" %c %d\n", &c, &time) == EOF) 
	printf("%c %d\n", c, time);
	
	if (error) {
		printf("Error: incomplete board\n");
		exit(1);
	}
	
	return board;
}

PRIVATE void print_board(char *board) {
	int x, y, index;
	for (y = 0, index = 0; y < BOARD_DIM; y++, index++) {
		printf("%d ", y+1);
		for (x = 0; x < BOARD_DIM; x++, index++) printf("%c", board[index]);
		printf("\n");
	}
}
