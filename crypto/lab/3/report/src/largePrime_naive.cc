#include "largePrime.h"





/**
 * Generates all the prime numbers lower than "targetSize".</br>
 * Returns one of them randomly picked.</br>
 * If the parameter "outputPrime" is not null, it will be filled with all the computed prime numbers.
 */
mpz_t* LargePrime::getPrime_seiveErathosten(mpz_t targetSize, List *outputPrime)
{
	size_t	sizeOfMPZ	= sizeof(mpz_t);
	List	*rawInteger	= new List();
	List *primeList, *tailRawInteger, *tailPrimeList, *nonPrimeElem;
	mpz_t i, j, nbrPrime, *res;
	gmp_randstate_t state;

	if (outputPrime == NULL)
	{
		primeList = new List();
	}
	else
	{
		primeList = outputPrime;
	}
	mpz_init_set_ui	(i,		2);

	tailRawInteger = rawInteger;										// Initialize the list of all integers
	while (mpz_cmp(i, targetSize) <= 0)
	{
		tailRawInteger->elem	= (mpz_t*)malloc(sizeOfMPZ);
		mpz_set(*(tailRawInteger->elem), i);
		tailRawInteger->next	= new List();
		tailRawInteger			= tailRawInteger->next;
		mpz_add_ui(i, i, 1);
	}

	tailRawInteger	= rawInteger;										// Seive execution
	tailPrimeList	= primeList;
	mpz_init_set_ui	(i,	2);
	mpz_init_set_ui	(nbrPrime, 0);
	while (mpz_cmp(i, targetSize) <= 0)
	{
		if (tailRawInteger->elem != NULL)
		{
//TODO
std::cout << "\tPrime: " << mpz_get_str(NULL, 10, *tailRawInteger->elem) << "\n";
//TODO
			tailPrimeList->elem	= (mpz_t*)malloc(sizeOfMPZ);		//		Mark the element as prime
			mpz_set(*(tailPrimeList->elem), i);
			tailPrimeList->next	= new List();
			tailPrimeList		= tailPrimeList->next;
			mpz_add_ui(nbrPrime, nbrPrime, 1);

			mpz_init_set(j, i);
			mpz_pow_ui(j, j, 2);
			while (mpz_cmp(j, targetSize) <= 0)
			{
				nonPrimeElem = tailRawInteger->findElemByValue(j);
				if ((nonPrimeElem != NULL) && (nonPrimeElem->elem != NULL))
				{
					free(nonPrimeElem->elem);
					nonPrimeElem->elem = NULL;
				}
				mpz_add(j, j, i);
			}
		}
		tailRawInteger = tailRawInteger->next;
		mpz_add_ui(i, i, 1);
	}

	gmp_randinit_default (state);										// Pick an element randomly among the primes
	gmp_randseed_ui(state, time(NULL));
	mpz_urandomm (i, state, nbrPrime);
	res = (mpz_t*)malloc(sizeOfMPZ);
	res = primeList->findElemByIndex(i)->elem;

	delete (rawInteger);												// Free all the used data
	if (outputPrime == NULL)
	{
		delete (primeList);
	}

	return res;
}

/**
 * Generates all the prime numbers lower than "targetSize".</br>
 * Returns one of them randomly picked.</br>
 * If the parameter "outputPrime" is not null, it will be filled with all the computed prime numbers.
 */
mpz_t* LargePrime::getPrime_trialDivision(mpz_t targetSize, List *outputPrime)
{
	mpz_t i, j, mod, nbrPrime, zero, *res;
	List *primeList, *primeListPtr;
	gmp_randstate_t state;
	size_t sizeOfMPZ = sizeof(mpz_t);

	mpz_init_set_ui(i,			2);
	mpz_init_set_ui(nbrPrime,	0);
	mpz_init_set_ui(zero,		0);
	mpz_init(mod);
	if (outputPrime == NULL)
	{
		primeList = new List();
	}
	else
	{
		primeList = outputPrime;
	}
	primeListPtr = primeList;

	while(mpz_cmp(i, targetSize) < 0)								// For each int
	{
		mpz_init_set_ui(j, 2);
		bool isPrime = true;
		while(mpz_cmp(j, i) < 0)									// Check all the potential divisors
		{
			mpz_mod(mod, i, j);
			if (mpz_cmp(mod, zero) == 0)
			{
				isPrime = false;
				break;
			}
			mpz_add_ui(j, j, 1);
		}
		if (isPrime)
		{
//TODO
std::cout << "\tPrime: " << mpz_get_str(NULL, 10, i) << "\n";
//TODO
			primeListPtr->elem	= (mpz_t*)malloc(sizeOfMPZ);
			mpz_set(*(primeListPtr->elem), i);
			primeListPtr->next	= new List();
			primeListPtr		= primeListPtr->next;
			mpz_add_ui(nbrPrime, nbrPrime, 1);
		}
		mpz_add_ui(i, i, 1);
	}

	gmp_randinit_default (state);									// Pick an element randomly among the primes
	gmp_randseed_ui(state, time(NULL));
	mpz_urandomm (i, state, nbrPrime);
	res = (mpz_t*)malloc(sizeOfMPZ);
	res = primeList->findElemByIndex(i)->elem;

	if (outputPrime == NULL)										// Free all the used data
	{
		delete (primeList);
	}

	return res;
}
