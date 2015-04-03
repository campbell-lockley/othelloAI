/* ****************************************************************************************************************** *
 * Name:	filo.c
 * Description:	A simple FILO implemented as a linked list.
 * Author:	Campbell Lockley		studentID: 1178618
 * Date:	02/04/15
 * ****************************************************************************************************************** */

/* ******** *
 * Includes *
 * ******** */
#include <stdlib.h>
#include <stdbool.h>

#include "filo.h"

/* ********* *
 * Functions *
 * ********* */
void filo_push(Filo **filo, void *value) {
	Filo *node;
	if (!filo) return;				/* Check for null pointer				      */
	node = (Filo *)calloc(1, sizeof(Filo));		/* Create node						      */
	node->value = value;
	node->next = *filo;
	*filo = node;					/* Update head						      */
}

void *filo_pop(Filo **filo) {
	Filo *head;
	void *value;
	if (!filo) return NULL;				/* Check for null pointer				      */
	if (!(*filo)) return NULL;
	head = *filo;
	value = head->value;
	*filo = head->next;				/* Update head						      */
	free(head);					/* Remove old head					      */
	return value;					/* Return value of removed node				      */
}

void *filo_peek(Filo **filo) {
	if (!filo) return NULL;				/* Check for null pointer 				      */
	if (!(*filo)) return NULL;
	return (*filo)->value;				/* Return value of head					      */
}

bool filo_hasNext(Filo **filo) {
	if (!filo) return false;			/* Check for null pointer				      */
	if (!(*filo)) return false;
	return (!((*filo)->next)) ? false : true;	/* IF filo->next == NULL return false ELSE return true	      */
}

bool filo_isEmpty(Filo **filo) {
	if (!filo) return true;				/* Check for null pointer				      */
	return (!(*filo)) ? true : false;		/* IF filo == NULL return true ELSE return false	      */
}

int filo_size(Filo **filo) {
	int count = 0;
	if (!filo) return 0;				/* Check for null pointer				      */
	while (*filo) {					/* Count while not null					      */
		count++;
		*filo = (*filo)->next;
	}
	return count;
}

void filo_init(Filo **filo) {
	if (!filo) return;				/* Check for null pointer				      */
	*filo = NULL;
}

void filo_destroy(Filo **filo) {
	void *value;
	if (!filo) return;				/* Check for null pointer				      */
	while (!filo_isEmpty(filo)) {			/* Free every node its value in the filo		      */
		value = filo_pop(filo);
		free(value);
	}
	*filo = NULL;
}
