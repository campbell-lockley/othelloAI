/* ****************************************************************************************************************** *
 * Name:	othelloAI.h
 * Description:	Header file for othelloAI.c.
 * Author:	Campbell Lockley		studentID: 1178618
 * Date:	08/04/15
 * ****************************************************************************************************************** */

#ifndef _OTHELLOAI_H
#define _OTHELLOAI_H

/* ******* *
 * Defines *
 * ******* */
#define BOARD_DIM	8
#define BOARD_SIZE	(BOARD_DIM * BOARD_DIM)
#define EMPTY		'.'
#define BLACK		'B'
#define WHITE		'O'

/* ******** *
 * Typedefs *
 * ******** */
typedef struct State {						/* othelloAI's definition of a state		      */
	char colour;
	char board[BOARD_SIZE];
} State;

typedef struct Action {						/* othelloAI's definition of an action		      */
	int x;
	int y;
	State *state;
} Action;

/* ********** *
 * Prototypes *
 * ********** */
extern Action *compute_move(State *state, int time);		/* Runs othelloAI to comute best next move	      */

#endif
