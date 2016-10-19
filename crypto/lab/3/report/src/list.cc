#include "list.h"





// -----------------------------
// Builder
// -----------------------------
	List::List ()
	{
		this->elem	= NULL;
		this->next	= NULL;
	}

	List::~List ()
	{
		if (this->elem != NULL)
		{
			free (this->elem);
		}

		if (this->next != NULL)
		{
			delete this->next;
		}
	}

// -----------------------------
// Local methods
// -----------------------------
	/**
	 * @return the list which head's is the given value
	 */
	List* List:: findElemByValue(mpz_t value)
	{
		List *pointer = this;

		while (pointer != NULL)
		{
			if ((pointer->elem != NULL) && (mpz_cmp(*(pointer->elem), value) == 0))
			{
				return pointer;
			}
			pointer = pointer->next;
		}
		return NULL;
	}

	/**
	 * @return the list which head's is at the given index
	 */
	List* List:: findElemByIndex(mpz_t index)
	{
		List *pointer = this;
		mpz_t i;

		mpz_init_set_ui(i, 0);

		while(mpz_cmp(i, index) < 0)
		{
			if (pointer == NULL)
			{
				return NULL;
			}
			pointer = pointer->next;
			mpz_add_ui(i, i, 1);
		}
		return pointer;
	}
