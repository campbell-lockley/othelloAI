/* ****************************************************************************************************************** *
 * Name:	filo.h
 * Description:	Header file for filo.c
 * Author:	Campbell Lockley		studentID: 1178618
 * Date:	02/04/15
 * ****************************************************************************************************************** */

#ifndef _FILO_H
#define _FILO_H

/* ******** *
 * Typedefs *
 * ******** */
typedef struct Filo {
	void *value;
	struct Filo *next;
} Filo;

/* ********** *
 * Prototypes *
 * ********** */
extern void  filo_push    (Filo **filo, void *value);		/* Push new value onto head of filo		      */
extern void *filo_pop     (Filo **filo);			/* Pop head off filo, returning value		      */
extern void *filo_peek    (Filo **filo);			/* Retrieve value of head of filo		      */
extern bool  filo_hasNext (Filo **filo);			/* Check if head has a next node		      */
extern bool  filo_isEmpty (Filo **filo);			/* Check if this filo is empty			      */
extern int   filo_size    (Filo **filo);			/* Counts size of filo				      */
extern void  filo_init    (Filo **filo);			/* Initialises filo for safe usage		      */
extern void  filo_destroy (Filo **filo);			/* Destroys filo				      */

#endif
