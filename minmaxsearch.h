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
#define STATE void

/* ******** *
 * Typedefs *
 * ******** */
typedef struct MMSearch {
	int time;
	int (*utility)(STATE *state);
	bool (*terminal_test)(STATE *state);
	Filo *(*successors)(STATE *state);
} MMSearch;

/* ********** *
 * Prototypes *
 * ********** */
extern char     *minmax_decision    (STATE *state);
extern int       max_value          (STATE *state);
extern int       min_value          (STATE *state);
extern char     *minmaxsearch_start (MMSearch *search, STATE *state);
extern MMSearch *minmaxsearch_init  (int time,
				     int (*utility)(STATE *state),
				     bool (*terminal_test)(STATE *state),
				     Filo *(*successors)(STATE *state));

#endif
