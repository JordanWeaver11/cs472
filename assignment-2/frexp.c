#include <stdio.h>
#include <math.h>

#define SIGN_MASK	0x8000000000000000
#define SIGN_SHIFT	63
#define EXP_MASK	0x7ff0000000000000
#define EXP_SHIFT	52

int main() {
	double x = -200.01;
	//unsigned int *ptr = (unsigned int *) (&x);
	//long unsigned int sign = (( *(long unsigned int *) (&x) ) & 0x8000000000000000) >> 63;
	long unsigned int *ptr = (long unsigned int *) (&x);
	int sign = ((*ptr) & SIGN_MASK) >> SIGN_SHIFT;
	if(sign) {
		printf("sign: 1\n");
	}
	else {
		printf("sign: 0\n");
	}
	printf("Number: %f\n", x);
	printf("Sign: %d\n", sign);
	double fraction;
	int exp;
	fraction = frexp(x, &exp);
	printf("fraction = %f, exp = %d\n", fraction, exp);
	int myExp = ((*ptr) & EXP_MASK) >> EXP_SHIFT;
	myExp -= 1022;
	printf("myExp = %d\n", myExp);
	return 0;
}
