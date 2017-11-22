#include <stdio.h>
#include <endian.h>

#if __BYTE_ORDER == __ORDER_BIG_ENDIAN
int big_endian = 1;
#else
int big_endian = 0;
#endif

int main(int argc, char **argv)
{
	short val;
	char *p_val;
	p_val = (char *) &val;
	/*
	  The following two lines assume big-endian
	  Architecture to initialize the variable Val
	  to 0x1234.
	*/
	if(big_endian) {
		p_val[0] = 0x12;
		p_val[1] = 0x34;
		
	}
	else {
		p_val[0] = 0x34;
		p_val[1] = 0x12;
	}

	printf("%x\n", val);
	
	return 0;
}
