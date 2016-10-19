#include "largePrime.h"






/**
 * The input a is a Miller-Rabin witness for n if a^n != 1[n] and </br>
 * a^(2^i * m) != -1 [n] for all i in {0, ..., s-1}</br>
 */
char LargePrime::isMillerRabinWitness(mpz_t a, mpz_t n)
{
	mpz_t am, s, m, n_1, i, twoPowI;

	mpz_init(am);
	mpz_init(s);
	mpz_init(m);
	mpz_init(n_1);
	mpz_init(i);
	mpz_init(twoPowI);

	mpz_sub_ui(n_1, n, 1);
	decomposeInteger(n_1, s, m);
	mpz_powm(am, a, m, n);

	if (mpz_cmp_ui(am, 1) == 0)
	{
		return 0;
	}

	mpz_init_set_ui(i, 0);
	mpz_init_set_ui(twoPowI, 1);
	while(mpz_cmp(i, s) != 0)
	{
		if (mpz_cmp_ui(am, -1) == 0)
		{
			return 0;
		}
		mpz_mul_ui(twoPowI, twoPowI, 2);
		mpz_powm(am, am, twoPowI, n);
		mpz_add_ui(i, i, 1);
	}
	return 1;
}

char LargePrime::isPrime_millerRabin(mpz_t n, int proba)
{
	mpz_t a, r, i;

	mpz_init(a);
	mpz_init_set_ui(r, 100);
	mpz_init_set_ui(i, 0);

	mpz_sub_ui(r, r, proba);
	mpz_mul(r, r, n);

	gmp_randstate_t rstate;
	gmp_randinit_default(rstate);
	gmp_randseed_ui (rstate, time(NULL));

	while(mpz_cmp(i, r) != 0)
	{
		mpz_urandomm(a, rstate, n);
		while(mpz_sgn(a) == 0)
		{
			mpz_urandomm(a, rstate, n);
		}
		if (isMillerRabinWitness(a,n) == 1)
		{
			return 0;
		}

		mpz_add_ui(i, i, 1);
	}

	return 1;
}

/**
 * Generates random integer lower than the given integer.   Return the generated
 * integer if it is prime.  Uses the Miller-Rabin primality test
 */
void LargePrime::getPrime_millerRabin	(mpz_t targetSize, gmp_randstate_t randomGenerator, mpz_t res)
{
	mpz_urandomm (res, randomGenerator, targetSize);

	if (isPrime_millerRabin(res, DEFAULT_PROBABILITY_THRESHOLD) == 1)
	{
		return;
	}
	else
	{
		getPrime_millerRabin(targetSize, randomGenerator, res);
	}
}
