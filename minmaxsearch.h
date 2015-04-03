/* ****************************************************************************************************************** *
 * Name:	minmaxsearch.h
 * Description:	Header file for minmaxsearch.c
 * Author:	Campbell Lockley		studentID: 1178618
 * Date:	31/03/15
 * ****************************************************************************************************************** */

#ifndef _MMSEARCH_H
#define _MMSEARCH_H

/* ******* *
 * Defines *
 * ******* */
#define STATE void

/* ******** *
 * Typedefs *
 * ******** */

/* ********** *
 * Prototypes *
 * ********** */
extern char *minmax_decision (STATE *state);
extern int   max_value       (STATE *state);
extern int   min_value       (STATE *state);

#endif
