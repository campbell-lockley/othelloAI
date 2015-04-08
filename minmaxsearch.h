/* ****************************************************************************************************************** *
 * Name:	minmaxsearch.h
 * Description:	Header file for minmaxsearch.c
 * Author:	Campbell Lockley		studentID: 1178618
 * Date:	31/03/15
 * ****************************************************************************************************************** */

#ifndef _MMSEARCH_H
#define _MMSEARCH_H

/* ******** *
 * Includes *
 * ******** */
#include <stdbool.h>

#include "filo.h"

/* ******* *
 * Defines *
 * ******* */
#define STATE	void
#define ACTION	void

/* ********** *
 * Prototypes *
 * ********** */
extern ACTION *minmax_decision   (STATE *state);			/* Start a minmax search. Returns best action */
extern void    minmaxsearch_init (int time,				/* Sets required values for minmaxsearch      */
				  Filo *(*actions)(STATE *state),
				  STATE *(*result)(ACTION *a, STATE *state),
				  int (*utility)(STATE *state),
				  bool (*terminal_test)(STATE *state),
				  Filo *(*successors)(STATE *state),
				  void(*set_estimate)(ACTION *a, int estimate),
				  void(*free_action)(ACTION *a),
				  void(*free_state)(STATE *state));
extern int     minmax_get_depth  (void);				/* Returns the last maximum depth reached     */

#endif
