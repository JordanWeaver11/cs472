#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define SIGN_MASK		0x8000000000000000
#define SIGN_SHIFT		63
#define EXP_MASK		0x7ff0000000000000
#define EXP_SHIFT		52

#define MANT_MASK		0x000FFFFFFFFFFFFF
#define MANT_LOW_MASK	0x00000000FFFFFFFF
#define MANT_LOW_SHIFT	0
#define MANT_HIGH_MASK	0x000FFFFF00000000
#define MANT_HIGH_SHIFT	33

#define SET_EXP			0x3FE0000000000000

double myfrexp(double x, int *exp) {
	//fool compiler into thinking x is an int, to use bit operations
	uint64_t *ptr = (uint64_t *) (&x);
	//extract the sign bit
	uint64_t sign = ((*ptr) & SIGN_MASK);
	//extract the exponent bits
	*exp = ((*ptr) & EXP_MASK) >> EXP_SHIFT;
	//subtract exp bias
	*exp -= 1022;
	//extract the fraction bits
	uint64_t i = ((*ptr) & MANT_MASK);
	//set the exp to 0
	i |= SET_EXP;
	//set the sign;
	i |= sign;
	double d = 0;
	//copy into a double data type
	memcpy(&d, &i, sizeof(i));
	//return the fraction
	return d;
}

int main(int argc, char* argv[]) {
	/*
	double x = -1000.56026423302164;
	uint64_t *ptr = (uint64_t *) (&x);
	int exp;
	double fraction = frexp(x, &exp);
	printf("fraction = %f, exp = %d\n", fraction, exp);
	
	fraction = myfrexp(x, &exp);
	printf("myFraction = %f, exp = %d\n", fraction, exp);
	*/
	
	/*
	if(argc != 2) {
		printf("ERROR, pass 1 double to the frexp");
		return -1;
	}
	
	double x = argv[1];
	*/
	
	
	return 0;
}
