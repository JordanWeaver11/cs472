#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define SIGN_MASK		0x8000000000000000
#define SIGN_SHIFT		63
#define EXP_MASK		0x7ff0000000000000
#define EXP_SHIFT		52

#define MANT_MASK		0x000FFFFFFFFFFFFF
#define MANT_SHIFT		52
#define MANT_LOW_MASK	0x00000000FFFFFFFF
#define MANT_LOW_SHIFT	0
#define MANT_HIGH_MASK	0x000FFFFF00000000
#define MANT_HIGH_SHIFT	32


#define SET_EXP			0x3FE0000000000000

union Data {
	uint64_t i64;
	int32_t i32;
	double d;
	char string[8];
};

//compares two floating point numbers
int fequal(double a, double b) {
	double epsilon = 0.0000001;
	return fabs(a-b) < epsilon;
}


double myfrexp(double x, int *exp) {
	//fool compiler into thinking x is an int, to use bit operations
	uint64_t *ptr = (uint64_t *) (&x);
	
	//check for special input
	//check for Not a number
	if(fequal((*ptr), NAN)) {
		*exp = 0;
		return NAN;
	}
	//check for positive and negative 0
	if(fequal((*ptr), 0) | fequal((*ptr), -0)) {
		*exp = 0;
		return 0;
	}
	//check for infinity
	if(fequal((*ptr), INFINITY)) {
		*exp = 0;
		return INFINITY;
	}
	//check for negInfinity
	else if (fequal((*ptr), -INFINITY)) {
		*exp = 0;
		return -INFINITY;
	}
	
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

//x + y
double add(double x, double y) {
	//0 = add, 1 = subtract
	int op = 0;
	int xExp, yExp, xsign, ysign, xysign;
	union Data xMant;
	union Data yMant;
	xMant.d = myfrexp(x, &xExp);
	yMant.d = myfrexp(y, &yExp);
	//ensure y <= x
	if(yMant.d > xMant.d) {
		xMant.d = myfrexp(y, &xExp);
		yMant.d = myfrexp(x, &yExp);
	}
	//set the signs
	xsign = (xMant.i64 & SIGN_MASK) >> SIGN_SHIFT;
	ysign = (yMant.i64 & SIGN_MASK) >> SIGN_SHIFT;
	//check the signs
	if(xsign & ysign) {
		xysign = 1;
	}
	//x > y, so sign = x
	else if(xsign | ysign) {
		xysign = xsign;
		op = 1;
	}
	else {
		xysign = 0;
	}
	//get mantissa for x
	xMant.i64 &= MANT_MASK;
	//add 1 to the left of the radix
	xMant.i64 |= (1 << MANT_SHIFT);
	//get mantissa for y
	yMant.i64 &= MANT_MASK;
	//add 1 to the left of the radix
	yMant.i64 |= (1 << MANT_SHIFT);
	//set a common exponent
	yExp +=  xExp - yExp;
	//shift the y mantissa to match the exponent change
	yMant.i64 <<= xExp - yExp;
	
	//add the mantissas
	union Data xyMant;
	uint64_t xyExp;
	if(op) {
		xyMant.i64 = xMant.i64 - yMant.i64;
		xyExp = yExp;
	}
	else {
		xyMant.i64 = xMant.i64 + yMant.i64;
		xyExp = yExp;

	}

	//shift to normalize
	while((xyMant.i64 >> MANT_SHIFT) > 1) {
		xyMant.i64 >>= 1;
		xyExp += 1;
	}
	//delete the extra 1 to the left of the radix
	xyMant.i64 &= ~(1<<MANT_SHIFT);
	//set the sign
	if(xysign > 0) {
		xyMant.i64 &= ~(1<<SIGN_SHIFT);
	}
	else {
		xyMant.i64 |= (1 << SIGN_SHIFT);
	}
	//set exp to account for bias
	xyExp += 1022;
	xyMant.i64 |= xyExp << EXP_SHIFT;
	return xyMant.d;
}

//x - y
double subtract(double x, double y) {
	//0 = add, 1 = subtract
	int op = 0;
	int xExp, yExp, xsign, ysign, xysign;
	union Data xMant;
	union Data yMant;
	xMant.d = myfrexp(x, &xExp);
	yMant.d = myfrexp(y, &yExp);
	//ensure y <= x
	if(yMant.d > xMant.d) {
		xMant.d = myfrexp(y, &xExp);
		yMant.d = myfrexp(x, &yExp);
	}
	//set the signs
	xsign = (xMant.i64 & SIGN_MASK) >> SIGN_SHIFT;
	ysign = (yMant.i64 & SIGN_MASK) >> SIGN_SHIFT;
	//check the signs
	if(xsign & ysign) {
		xysign = 1;
		op = 1;
	}
	//x > y, so sign = x
	else if(xsign | ysign) {
		xysign = xsign;
	}
	else {
		xysign = 0;
		op = 1;
	}
	//get mantissa for x
	xMant.i64 &= MANT_MASK;
	//add 1 to the left of the radix
	xMant.i64 |= (1 << MANT_SHIFT);
	//get mantissa for y
	yMant.i64 &= MANT_MASK;
	//add 1 to the left of the radix
	yMant.i64 |= (1 << MANT_SHIFT);
	//set a common exponent
	yExp +=  xExp - yExp;
	//shift the y mantissa to match the exponent change
	yMant.i64 <<= xExp - yExp;
	
	//subtract the mantissas
	union Data xyMant;
	uint64_t xyExp;
	if(op) {
		xyMant.i64 = xMant.i64 - yMant.i64;
		xyExp = yExp;
	}
	else {
		xyMant.i64 = xMant.i64 + yMant.i64;
		xyExp = yExp;
	}

	//shift to normalize
	while((xyMant.i64 >> MANT_SHIFT) > 1) {
		xyMant.i64 >>= 1;
		xyExp += 1;
	}

	//delete the extra 1 to the left of the radix
	xyMant.i64 &= ~(1<<MANT_SHIFT);
	//set the sign
	if(xysign > 0) {
		xyMant.i64 &= ~(1<<SIGN_SHIFT);
	}
	else {
		xyMant.i64 |= (1 << SIGN_SHIFT);
	}
	//set exp to account for bias
	xyExp += 1022;
	xyMant.i64 |= xyExp << EXP_SHIFT;
	return xyMant.d;
}

//x * y
double mult(double x, double y) {
	int xExp, yExp, xsign, ysign, xysign;
	union Data xMant;
	union Data yMant;
	xMant.d = myfrexp(x, &xExp);
	yMant.d = myfrexp(y, &yExp);
	//ensure y <= x
	if(yMant.d > xMant.d) {
		xMant.d = myfrexp(y, &xExp);
		yMant.d = myfrexp(x, &yExp);
	}
	//set the signs
	xsign = (xMant.i64 & SIGN_MASK) >> SIGN_SHIFT;
	ysign = (yMant.i64 & SIGN_MASK) >> SIGN_SHIFT;
	//check the signs
	if(xsign & ysign) {
		xysign = 0;
	}
	//x > y, so sign = x
	else if(xsign | ysign) {
		xysign = 1;
	}
	else {
		xysign = 0;
	}
	//get mantissa for x
	xMant.i64 &= MANT_MASK;
	//add 1 to the left of the radix
	xMant.i64 |= (1 << MANT_SHIFT);
	//get mantissa for y
	yMant.i64 &= MANT_MASK;
	//add 1 to the left of the radix
	yMant.i64 |= (1 << MANT_SHIFT);
	//set a common exponent
	yExp +=  xExp - yExp;
	//shift the y mantissa to match the exponent change
	yMant.i64 <<= xExp - yExp;
	
	//multiply the mantissas
	union Data xyMant;
	uint64_t xyExp;
	xyMant.i64 = xMant.i64 * yMant.i64;
	xyExp = yExp;

	//shift to normalize
	while((xyMant.i64 >> MANT_SHIFT) > 1) {
		xyMant.i64 >>= 1;
		xyExp += 1;
	}
	//delete the extra 1 to the left of the radix
	xyMant.i64 &= ~(1<<MANT_SHIFT);
	//set the sign
	if(xysign > 0) {
		xyMant.i64 &= ~(1<<SIGN_SHIFT);
	}
	else {
		xyMant.i64 |= (1 << SIGN_SHIFT);
	}
	//set exp to account for bias
	xyExp += 1022;
	xyMant.i64 |= xyExp << EXP_SHIFT;
	return xyMant.d;
}


//x / y
double div(double x, double y) {
	int xExp, yExp, xsign, ysign, xysign;
	union Data xMant;
	union Data yMant;
	xMant.d = myfrexp(x, &xExp);
	yMant.d = myfrexp(y, &yExp);
	//ensure y <= x
	if(yMant.d > xMant.d) {
		xMant.d = myfrexp(y, &xExp);
		yMant.d = myfrexp(x, &yExp);
	}
	//set the signs
	xsign = (xMant.i64 & SIGN_MASK) >> SIGN_SHIFT;
	ysign = (yMant.i64 & SIGN_MASK) >> SIGN_SHIFT;
	//check the signs
	if(xsign & ysign) {
		xysign = 0;
	}
	//x > y, so sign = x
	else if(xsign | ysign) {
		xysign = 1;
	}
	else {
		xysign = 0;
	}
	//get mantissa for x
	xMant.i64 &= MANT_MASK;
	//add 1 to the left of the radix
	xMant.i64 |= (1 << MANT_SHIFT);
	//get mantissa for y
	yMant.i64 &= MANT_MASK;
	//add 1 to the left of the radix
	yMant.i64 |= (1 << MANT_SHIFT);
	//set a common exponent
	yExp +=  xExp - yExp;
	//shift the y mantissa to match the exponent change
	yMant.i64 <<= xExp - yExp;
	
	//add the mantissas
	union Data xyMant;
	uint64_t xyExp;
	uint64_t quotient = 0;
	uint64_t remaind = 0;
	int i = 0;
	uint64_t last1 = 0;
	xyMant.i64 = xMant.i64 / yMant.i64;
	xyExp = yExp;
	for(i = 0; i < 64; i++) {
		if(quotient & (1<<i)) {
			last1 = i;
		}
	}
	quotient = xyMant.i64;
	remaind = xMant.i64 % yMant.i64;
	while(remaind > 0) {
		quotient |= (quotient / remaind) >> last1 + 1;
		remaind = quotient % remaind;
		last1++;
		xyExp--;
	}
	xyMant.i64 = quotient;
	/*
	//integer division
	xyMant.i64 = xMant.i64 / yMant.i64;
	*/

	//shift to normalize
	while((xyMant.i64 >> MANT_SHIFT) > 1) {
		xyMant.i64 >>= 1;
		xyExp += 1;
	}
	//delete the extra 1 to the left of the radix
	xyMant.i64 &= ~(1<<MANT_SHIFT);
	//set the sign
	if(xysign > 0) {
		xyMant.i64 &= ~(1<<SIGN_SHIFT);
	}
	else {
		xyMant.i64 |= (1 << SIGN_SHIFT);
	}
	//set exp to account for bias
	xyExp += 1022;
	xyMant.i64 |= xyExp << EXP_SHIFT;
	return xyMant.d;
}


int main(void) {
	double x = 100.1;
	double y = 2050.1;
	printf("%f + %f = %f\n", x, y, add(x, y));
	printf("%f - %f = %f\n", x, y, subtract(x, y));
	printf("%f * %f = %f\n", x, y, mult(x, y));
	printf("%f / %f = %f\n", x, y, div(x, y));
	
	printf("\n--------\n");
	x = 100.1;
	uint64_t *ptr = (uint64_t *) (&x);
	int exp;
	double fraction = frexp(x, &exp);
	printf("testing: %f\n", x);
	printf("frexp: fraction = %f, exp = %d\n", fraction, exp);
	
	fraction = myfrexp(x, &exp);
	printf("myfrexp: fraction = %f, exp = %d\n", fraction, exp);
	
	x = -100.1;
	printf("testing: %f\n", x);
	fraction = frexp(x, &exp);
	printf("frexp: fraction = %f, exp = %d\n", fraction, exp);
	fraction = myfrexp(x, &exp);
	printf("myfrexp: fraction = %f, exp = %d\n", fraction, exp);
	
	x = 0;
	printf("testing: %f\n", x);
	fraction = frexp(x, &exp);
	printf("frexp: fraction = %f, exp = %d\n", fraction, exp);
	fraction = myfrexp(x, &exp);
	printf("myfrexp: fraction = %f, exp = %d\n", fraction, exp);
	
	x = -0;
	printf("testing: -%f\n", x);
	fraction = frexp(x, &exp);
	printf("frexp: fraction = %f, exp = %d\n", fraction, exp);
	fraction = myfrexp(x, &exp);
	printf("myfrexp: fraction = %f, exp = %d\n", fraction, exp);
	
	x = INFINITY;
	printf("testing: %f\n", x);
	fraction = frexp(x, &exp);
	printf("frexp: fraction = %f, exp = %d\n", fraction, exp);
	fraction = myfrexp(x, &exp);
	printf("myfrexp: fraction = %f, exp = %d\n", fraction, exp);
	
	x = -INFINITY;
	printf("testing: %f\n", x);
	fraction = frexp(x, &exp);
	printf("frexp: fraction = %f, exp = %d\n", fraction, exp);
	fraction = myfrexp(x, &exp);
	printf("myfrexp: fraction = %f, exp = %d\n", fraction, exp);
	
	x = NAN;
	printf("testing: %f\n", x);
	fraction = frexp(x, &exp);
	printf("frexp: fraction = %f, exp = %d\n", fraction, exp);
	fraction = myfrexp(x, &exp);
	printf("myfrexp: fraction = %f, exp = %d\n", fraction, exp);
	
	printf("\n--------\n");
	
	printf("TESTING add function\n");
	volatile int i = 0;
	//random test values
	x = 1587.268112;
	y = 5602.16198;
	clock_t t = clock();
	printf("Software add function:\n");
	for(i = 0; i < 10000; i++) {
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
	}
	
	t = clock() - t;
	
	clock_t t2 = clock();
	for(i = 0; i < 10000; i++) {
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
		add(x, y);
	}
	
	t2 = clock() - t2;
	
	printf("\telapsed time: %ld clock ticks\n", (t - t2) / 22);
	
	printf("Hardware add function:\n");
	t = clock();
	for(i = 0; i < 100000000; i++) {
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
	}
	
	t = clock() - t;
	
	t2 = clock();
	for(i = 0; i < 100000000; i++) {
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
		x + y;
	}
	
	t2 = clock() - t2;
	printf("\telapsed time: %ld clock ticks\n", (t - t2) / 22);
	
	printf("DONE testing add() function\n");
	
	printf("TESTING subtract() function\n");
	i = 0;
	//random test values
	x = 1587.268112;
	y = 5602.16198;
	t = clock();
	printf("Software subtract function:\n");
	for(i = 0; i < 10000; i++) {
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
	}
	
	t = clock() - t;
	
	t2 = clock();
	for(i = 0; i < 10000; i++) {
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
		subtract(x, y);
	}
	
	t2 = clock() - t2;
	
	printf("\telapsed time: %ld clock ticks\n", (t - t2) / 22);
	
	printf("Hardware subtract function:\n");
	t = clock();
	for(i = 0; i < 100000000; i++) {
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
	}
	
	t = clock() - t;
	
	t2 = clock();
	for(i = 0; i < 100000000; i++) {
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
		x - y;
	}
	
	t2 = clock() - t2;
	printf("\telapsed time: %ld clock ticks\n", (t - t2) / 22);
	
	printf("DONE testing subract() function\n");
	
	printf("TESTING mul() function\n");
	i = 0;
	//random test values
	x = 1587.268112;
	y = 5602.16198;
	t = clock();
	printf("Software mul function:\n");
	for(i = 0; i < 10000; i++) {
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
	}
	
	t = clock() - t;
	
	t2 = clock();
	for(i = 0; i < 10000; i++) {
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
		mult(x, y);
	}
	
	t2 = clock() - t2;
	
	printf("\telapsed time: %ld clock ticks\n", (t - t2) / 22);
	
	printf("Hardware mul function:\n");
	t = clock();
	for(i = 0; i < 100000000; i++) {
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
	}
	
	t = clock() - t;
	
	t2 = clock();
	for(i = 0; i < 100000000; i++) {
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
		x * y;
	}
	
	t2 = clock() - t2;
	printf("\telapsed time: %ld clock ticks\n", (t - t2) / 22);
	
	printf("DONE testing mul() function\n");
	
	printf("TESTING div() function\n");
	i = 0;
	//random test values
	x = 1587.268112;
	y = 5602.16198;
	t = clock();
	printf("Software div function:\n");
	for(i = 0; i < 10000; i++) {
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
	}
	
	t = clock() - t;
	
	t2 = clock();
	for(i = 0; i < 10000; i++) {
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
		div(x, y);
	}
	
	t2 = clock() - t2;
	
	printf("\telapsed time: %ld clock ticks\n", (t - t2) / 22);
	
	printf("Hardware div function:\n");
	t = clock();
	for(i = 0; i < 100000000; i++) {
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
	}
	
	t = clock() - t;
	
	t2 = clock();
	for(i = 0; i < 100000000; i++) {
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
		x / y;
	}
	
	t2 = clock() - t2;
	printf("\telapsed time: %ld clock ticks\n", (t - t2) / 22);
	
	printf("DONE testing div() function\n");

	printf("\n--------\n");
	printf("TESTING feature extraction\n");
	union Data myData;
	int sign;
	myData.d = 2566.6251;
	printf("The value is: %f\n", myData.d);
	
	printf("Treating the value as a DOUBLE:\n");
	fraction = myfrexp(myData.d, &exp);
	sign = (myData.i64 & SIGN_MASK) >> SIGN_SHIFT;
	printf("\tsign: %d, mantissa: %f, exp: %d\n", sign, fraction, exp);
	
	myData.d = 2565.6251;
	printf("Treating the value as a LONG:\n");
	printf("\tvalue: %d, sign: %d\n", myData.i32, (myData.i32) >> 31);
	
	printf("Treating the value as CHARACTERS:\n");
	for(i = 0; i < 8; i++) {
		printf("character %d is: %c\n", i, myData.string[i]);
	}
	
	printf("DONE testing feature extraction\n");
	return 0;
}
