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

/* ******** *
 * Typedefs *
 * ******** */
/*typedef struct MMSearch {
	int time;
	Filo *(*actions)(STATE *state);
	STATE *(*result)(ACTION *a, STATE *state);
	int (*utility)(STATE *state);
	bool (*terminal_test)(STATE *state);
	Filo *(*successors)(STATE *state);
} MMSearch;*/

/* ********** *
 * Prototypes *
 * ********** */
extern ACTION   *minmax_decision    (STATE *state);
extern int       max_value          (STATE *state);
extern int       min_value          (STATE *state);
/*extern char     *minmaxsearch_start (MMSearch *search, STATE *state);*/
extern void minmaxsearch_init  (int runtime,
				     Filo *(*actions)(STATE *state),
				     STATE *(*result)(ACTION *a, STATE *state),
				     int (*utility)(STATE *state),
				     bool (*terminal_test)(STATE *state),
				     Filo *(*successors)(STATE *state),
				     void(*free_action)(ACTION *a),
				     void(*free_state)(STATE *state));

#endif
