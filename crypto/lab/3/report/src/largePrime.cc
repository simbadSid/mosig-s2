#include "largePrime.h"







/**
 * Decomposes the input n as n = (2 ^ s) * m where s is the highest integer and m is odd
 */
void LargePrime::decomposeInteger(mpz_t n, mpz_t s, mpz_t m)
{
	mpz_t twoPowS, mod;

	mpz_init(mod);
	mpz_init_set_ui(twoPowS, 1);
	mpz_set_ui(s, 0);

	do
	{
		mpz_add_ui(s, s, 1);
		mpz_mul_ui(twoPowS, twoPowS, 2);
		mpz_mod(mod, n, twoPowS);
	}while(mpz_cmp(twoPowS, mod) == 0);

	mpz_sub_ui(s, s, 1);
	mpz_div_ui(twoPowS, twoPowS, 2);
	mpz_div(m, n, twoPowS);
}


/**
 * TODO
 */
mpz_t* LargePrime::string_to_int(char* s, mpz_t n)
{
	int block_length = ((int)mpz_sizeinbase(n,2))>>3;
	int nb_blocks = strlen(s)/block_length+1;
	mpz_t* res = (mpz_t*)malloc((nb_blocks+1)*sizeof(mpz_t));
	mpz_t tmp;
	int i,j;

	mpz_init(tmp);
	for(i=0;i<nb_blocks;i++)
	{
		mpz_set_ui(tmp,0);
		for(j=0;j<block_length;j++)
		{
			mpz_mul_2exp(tmp,tmp,8);
			if (s[i*block_length+j] == '\0')	break;
			mpz_add_ui(tmp,tmp,(unsigned long int)(s[i*block_length+j]));
		}
		mpz_init_set(res[i],tmp);
	}
	mpz_init(res[nb_blocks]);
	mpz_clear(tmp);
	return res;
}

/**
 * TODO
 */
char* LargePrime::int_to_string(mpz_t* vec, mpz_t n)
{
	int block_length = ((int)mpz_sizeinbase(n,2))>>3;
	int vec_length = 0;
	while(mpz_cmp_ui(vec[vec_length],0) != 0)
	{
		vec_length++;
	}
	char* res = (char*)malloc(vec_length*block_length+1);
	res[vec_length*block_length]='\0';
	mpz_t tmp1,tmp2;
	mpz_init(tmp2);
	mpz_init(tmp1);
	int i,j;
	for(i = 0;i<vec_length;i++)
	{
		mpz_set(tmp1,vec[i]);
		for(j=block_length-1;j>=0;j--)
		{
			mpz_fdiv_r_2exp(tmp2,tmp1,8);
			mpz_fdiv_q_2exp(tmp1,tmp1,8);
			res[i*block_length+j]=(char) mpz_get_ui(tmp2);
		}
	}
	mpz_clear(tmp1);
	mpz_clear(tmp2);
	return res;
}
