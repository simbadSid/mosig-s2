#include "largePrime.h"




int main(int argc, char *argv[])
{
	mpz_t size, *prime = NULL;
	int max = DEFAULT_MAX_SIZE, probabilityThreashold = DEFAULT_PROBABILITY_THRESHOLD;

	if (argc >= 2)
	{
		max = atoi(argv[1]);
		if (max <= 0)
		{
			max = DEFAULT_MAX_SIZE;
		}
	}
	if (argc >= 3)
	{
		probabilityThreashold = atoi(argv[2]);
		if (probabilityThreashold <= 0)
		{
			probabilityThreashold = DEFAULT_PROBABILITY_THRESHOLD;
		}
	}

	mpz_init_set_ui(size, max);
	#ifdef ERATHOSTEN
		std::cout << "\n\n-----------------------------------------------\n";
		std::cout << "Erathosten sieve\n";
		prime = LargePrime::getPrime_seiveErathosten(size, NULL);
		std::cout << "\n\tResult = " << mpz_get_str(NULL, 10, *prime) <<"\n";
	#elif TRIAL_DIVISION
		std::cout << "\n\n-----------------------------------------------\n";
		std::cout << "Trial division\n";
		prime = LargePrime::getPrime_trialDivision(size, NULL);
		std::cout << "\n\tResult = " << mpz_get_str(NULL, 10, *prime) <<"\n";
	#elif MILLER_RABIN
		std::cout << "\n\n-----------------------------------------------\n";
		std::cout << "Miller-Rabin primality test\n";
		mpz_t i;
		mpz_init_set_ui(i, 2);
		while(mpz_cmp_ui(i, max) != 0)
		{
			char test = LargePrime::isPrime_millerRabin(i, probabilityThreashold);
			std::cout << "\t\tisPrime(" << mpz_get_str(NULL, 10, i) << ")\t: ";
			if (test == 0)
			{
				std::cout << "False\n";
			}
			else
			{
				std::cout << "True (with a probability of " << probabilityThreashold << "\%)\n";
			}
			mpz_add_ui(i, i, 1);
		}
	#else
		std::cout << "**** No method to execute ****\n";
	#endif

	if (prime != NULL)
	{
		free(prime);
	}

	return 0;
}
