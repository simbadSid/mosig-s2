#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <iostream>


class List
{
// -----------------------------
// Attributes
// -----------------------------
	public:
		mpz_t	*elem;
		List	*next;

// -----------------------------
// Builder
// -----------------------------
		List ();
		~List ();

// -----------------------------
// Local methods
// -----------------------------
		List *findElemByValue(mpz_t index);
		List *findElemByIndex(mpz_t index);
};


#endif
