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
#define MANT_HIGH_SHIFT	32
#define MANT_SHIFT		52

#define SET_EXP			0x3FE0000000000000

union Data {
	uint64_t i64;
	double d;
};

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

/*
double add(double x, double y) {
	//y <= x
	int xExp, yExp;
	double xMantd = myfrexp(xMantd, &xExp);
	double yMantd = myfrexp(yMantd, &yExp);
	//get mantissa for x
	uint64_t *xMantp = (uint64_t *) &xMantd;
	uint64_t xMant = *xMantp;
	xMant &= MANT_MASK;
	//add 1 to the left of the radix
	xMant |= 1 << MANT_SHIFT;
	//get mantissa for y
	uint64_t *yMantp = (uint64_t *) &yMantd;
	uint64_t yMant = *yMantp;
	yMant &= MANT_MASK;
	//add 1 to the left of the radix
	yMant |= 1 << MANT_SHIFT;
	//set a common exponent
	yExp +=  xExp - yExp;
	//shift the y mantissa to match the exponent change
	yMant <<= xExp - yExp;
	
	//add the mantissas
	uint64_t xyMant = xMant + yMant;
	uint64_t xyExp = yExp;
	//shift
	while((xyMant >> MANT_SHIFT) > 1) {
		xyMant >>= 1;
		xyExp += 1;
	}
	//delete the extra 1 to the left of the radix
	xyMant &= ~(1<<MANT_SHIFT);
	//set exp to account for bias
	xyExp += 1022;
	xyMant |= xyExp << EXP_SHIFT;
	double result = 0;
	memcpy(&result, &xyMant, sizeof(xyMant));
	return result;
}
*/

double add(double x, double y) {
	//y <= x
	int xExp, yExp;
	union Data xMant;
	union Data yMant;
	xMant.d = myfrexp(x, &xExp);
	yMant.d = myfrexp(y, &yExp);
	//get mantissa for x
	xMant.i64 &= MANT_MASK;
	//add 1 to the left of the radix
	xMant.i64 |= 1 << MANT_SHIFT;
	//get mantissa for y
	yMant.i64 &= MANT_MASK;
	//add 1 to the left of the radix
	yMant.i64 |= 1 << MANT_SHIFT;
	//set a common exponent
	yExp +=  xExp - yExp;
	//shift the y mantissa to match the exponent change
	yMant.i64 <<= xExp - yExp;
	
	//add the mantissas
	union Data xyMant;
	xyMant.i64 = xMant.i64 + yMant.i64;
	uint64_t xyExp = yExp;
	//shift
	while((xyMant.i64 >> MANT_SHIFT) > 1) {
		xyMant.i64 >>= 1;
		xyExp += 1;
	}
	//delete the extra 1 to the left of the radix
	xyMant.i64 &= ~(1<<MANT_SHIFT);
	//set exp to account for bias
	xyExp += 1022;
	xyMant.i64 |= xyExp << EXP_SHIFT;
	return xyMant.d;
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
	
	double x = 10.551;
	double y = 5.551;
	printf("%f + %f = %f\n", x, y, add(x, y));
	
	return 0;
}
