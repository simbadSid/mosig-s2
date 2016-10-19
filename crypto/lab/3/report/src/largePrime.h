#ifndef LARGE_PRIME
#define LARGE_PRIME


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include <iostream>
#include "list.h"




#define DEFAULT_MAX_SIZE				100
#define DEFAULT_PROBABILITY_THRESHOLD	43




class LargePrime
{
// -----------------------------
// Local methods
// -----------------------------
	public:
		static void		decomposeInteger	(mpz_t n, mpz_t s, mpz_t m);
//TODO		static void		initRandomGenerator	();
		static mpz_t*	string_to_int		(char* s, mpz_t n);
		static char*	int_to_string		(mpz_t* vec, mpz_t n);

		/**
		 * The input a is a Miller-Rabin witness for n if a^n != 1[n] and </br>
		 * a^(2^i * m) != -1 [n] for all i in {0, ..., s-1}</br>
		 */
		static char isMillerRabinWitness(mpz_t a, mpz_t n);

		/**
		 */
		static char isPrime_millerRabin(mpz_t n, int proba);

		/**
		 * Generates all the prime numbers lower than "targetSize".</br>
		 * Returns one of them randomly picked.</br>
		 * If the parameter "outputPrime" is not null, it will be filled with all the computed prime numbers.
		 */
		static mpz_t *getPrime_seiveErathosten	(mpz_t targetSize, List *outputPrime);

		/**
		 * Generates all the prime numbers lower than "targetSize".</br>
		 * Returns one of them randomly picked.</br>
		 * If the parameter "outputPrime" is not null, it will be filled with all the computed prime numbers.
		 */
		static mpz_t *getPrime_trialDivision	(mpz_t targetSize, List *outputPrime);

		/**
		 * Generates random integer lower than the given integer.   Return the generated
		 * integer if it is prime.  Uses the Miller-Rabin primality test
		 */
		static void getPrime_millerRabin(mpz_t targetSize, gmp_randstate_t randomGenerator, mpz_t res);

};


#endif
