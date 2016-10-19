#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include "largePrime.h"


#define KEY_SIZE	13
gmp_randstate_t state;





/**
 * Randomly generte a RSA key:
 * Input:
 * 		- b: number of bits in the key (n)
 * 	Ouptu:
 * 		- e: Public key: used to cipher a message m: cipher = m ^ e
 * 		- n: Public key: product of two prime numbers. Is of order b
 * 		- d: Private key: used to decipher a message s: decipher = s ^ d
 */
void RSA_keygen(int b, mpz_t e, mpz_t d, mpz_t n)
{
	mpz_t p, q, totientOfN, keyMax;

	mpz_init(p);
	mpz_init(q);
	mpz_init(totientOfN);
	mpz_init_set_ui(keyMax, 2);

	mpz_pow_ui(keyMax, keyMax, b);
	LargePrime::getPrime_millerRabin(keyMax, state, p);
	LargePrime::getPrime_millerRabin(keyMax, state, q);

	mpz_mul(n, p, q);
	mpz_sub_ui(p, p, 1);
	mpz_sub_ui(q, q, 1);
	mpz_mul(totientOfN,	p, q);
	LargePrime::getPrime_millerRabin(keyMax, state, e);
	mpz_invert(d, e, totientOfN);
}

void RSA_encrypt(int messageSize, mpz_t e, mpz_t n, char originalMessage[], mpz_t cipheredMessage[])
{
	mpz_t m;
	int i;

	mpz_init(m);
	for (i=0; i<messageSize; i++)
	{
		mpz_init(cipheredMessage[i]);
		mpz_set_ui(m, originalMessage[i]);
		mpz_powm(cipheredMessage[i], m, e, n);
	}
}

void RSA_decrypt(int messageSize, mpz_t d, mpz_t n, mpz_t cipheredMessage[], mpz_t decipheredMessage[])
{
	mpz_t s;
	int i;

	mpz_init(s);
	for (i=0; i<messageSize; i++)
	{
		mpz_init(decipheredMessage[i]);
		mpz_set(s, cipheredMessage[i]);
		mpz_powm(decipheredMessage[i], s, d, n);
	}
}

int main(int argc, char *argv[])
{
	mpz_t e, n, d;
	char message[200];
	int i, keySize = KEY_SIZE;
	gmp_randinit_default(state);
	gmp_randseed_ui(state, time(NULL));

	if (argc > 1)
	{
		keySize = atoi(argv[1]);
		if (keySize <= 0)
		{
			keySize = KEY_SIZE;
		}
	}
	if (argc <= 2)
	{
		sprintf(message, "Sorry for the delay");
	}
	else
	{
		sprintf(message, "%s", argv[2]);
	}
	mpz_init(e);
	mpz_init(n);
	mpz_init(d);
	RSA_keygen(KEY_SIZE, e, d, n);

	std::cout << "\t - Key size (in bits): \"" << KEY_SIZE << "\"\n";
	std::cout << "\t - Public Key        : n = " << mpz_get_str(NULL, 10, n) << ", e = " << mpz_get_str(NULL, 10, e) << "\n";
	std::cout << "\t - Private Key       : d = " << mpz_get_str(NULL, 10, d) << "\n";

	int messageSize = strlen(message);
	mpz_t cipher[messageSize];
	mpz_t decipher[messageSize];
	RSA_encrypt(messageSize, e, n, message, cipher);
	RSA_decrypt(messageSize, d, n, cipher, decipher);
	std::cout << "\t - Original message  : \"" << message << "\"\n";
	printf("\t - %15s  | %15s | %15s\n", "Message", "Cipher", "Decipher");
	for(i=0; i<messageSize; i++)
	{
		std::cout << "\t\t------------------------------------------------------\n";
		printf("\t    %15d | %15s | %15s\n",
				(int)message[i],
				mpz_get_str(NULL, 10, cipher[i]),
				mpz_get_str(NULL, 10, decipher[i]));
	}

	return 0;
}
