#include <gmp.h>

int main(int argc, char* argv[]) {
	mpz_t a, n, c;
	mpz_init(a);
	mpz_init(n);
	mpz_init(c);
	mpz_set_str(a, "2", 10);
	mpz_set_str(n, argv[1], 10);
	mpz_mul(c, a, a);
	while (0 >= mpz_cmp(c, n)) {
		if (mpz_divisible_p(n, a) != 0) {
			gmp_printf("%Zd*", a);
			mpz_tdiv_q(n, n, a);
		} else
			mpz_nextprime(a, a);
		mpz_mul(c, a, a);
	}
	gmp_printf("%Zd\n",n);
	return 0;
}
