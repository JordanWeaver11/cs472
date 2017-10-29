#include <stdio.h>
#include <string.h>
//#include <cpuid.h> //used only for bit defintions in feature table

#define bitSSE3			(1 << 0)
#define bitPCLMULQDQ	(1 << 1)
#define bitDTES64		(1 << 2)


void printBin(unsigned int num) {
	int binaryNumber[32];
	int i = 1;
	int j = 0;
	while(num != 0) {
		binaryNumber[i++] = num % 2;
		num = num / 2;
	}
	
	printf("0b");
	for(j = i - 1; j > 0; j--) {
		printf("%d", binaryNumber[j]);
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	//define ecx features in an easy-to-print array
	//see table 3-10 for lookup
	const char *ecxF[32];
	ecxF[0] = "SSE3";
	ecxF[1] = "PCLMULQDQ";
	ecxF[2] = "DTES64";
	ecxF[3] = "MONITOR";
	ecxF[4] = "DS-CPL";
	ecxF[5] = "VMX";
	ecxF[6] = "SMX";
	ecxF[7] = "EIST";
	ecxF[8] = "TM2";
	ecxF[9] = "SSSE3";
	ecxF[10] = "CNXT-ID";
	ecxF[11] = "SDBG";
	ecxF[12] = "FMA";
	ecxF[13] = "CMPXCHG16B";
	ecxF[14] = "XTPR Update Control";
	ecxF[15] = "PDCM";
	ecxF[16] = "Reserved";
	ecxF[17] = "PCID";
	ecxF[18] = "DCA";
	ecxF[19] = "SSE4.1";
	ecxF[20] = "SSE4.2";
	ecxF[21] = "x2APIC";
	ecxF[22] = "MOVBE";
	ecxF[23] = "POPCNT";
	ecxF[24] = "TSC-Deadline";
	ecxF[25] = "AESNI";
	ecxF[26] = "XSAVE";
	ecxF[27] = "OSXSAVE";
	ecxF[28] = "AVX";
	ecxF[29] = "F16C";
	ecxF[30] = "RDRAND";
	ecxF[31] = "Not Used";
	
	//define edx features in an easy-to-print array
	//see table 3-11 for lookup
	const char *edxF[32];
	edxF[0] = "FPU";
	edxF[1] = "VME";
	edxF[2] = "DE";
	edxF[3] = "PSE";
	edxF[4] = "TSC";
	edxF[5] = "MSR";
	edxF[6] = "PAE";
	edxF[7] = "MCE";
	edxF[8] = "CX8";
	edxF[9] = "APIC";
	edxF[10] = "Reserved";
	edxF[11] = "SEP";
	edxF[12] = "MTRR";
	edxF[13] = "PGE";
	edxF[14] = "MCA";
	edxF[15] = "CMOV";
	edxF[16] = "PAT";
	edxF[17] = "PSE-36";
	edxF[18] = "PSN";
	edxF[19] = "CLFSH";
	edxF[20] = "Reserved";
	edxF[21] = "DS";
	edxF[22] = "ACPI";
	edxF[23] = "MMX";
	edxF[24] = "FXSR";
	edxF[25] = "SSE";
	edxF[26] = "SSE2";
	edxF[27] = "SS";
	edxF[28] = "HTT";
	edxF[29] = "TM";
	edxF[30] = "Reserved";
	edxF[31] = "PBE";

	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;

//CPU family, model, brand, frequency------------------------------------------------
	eax = 0x1;
	__asm__ __volatile__(
	                     "cpuid;"
	                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
	                     : "a"(eax)
	                     );
	//Model = bits 4-7, 0xf0 in hex
	unsigned int model = (0xf0 & eax) >> 4;
	//extended model = bits 16-19, 0x70000
	unsigned int model_ext = (0x70000 & eax) >> 16;
	//family = bits 8-11, 0xf00 in hex
	unsigned int family = (0xf00 & eax) >> 8;
	//extended family = bits 20-27, 0xff00000
	unsigned int family_ext = (0xff00000 & eax) >> 20;
	//processor type = bits 12-13, 0x3000
	unsigned int procType = (0x3000 & eax) >> 12;
	
	//print model and family in binary for easy table lookup
	//note that this printing method removes all leading zeros from the binary num
	if (family == 0xf) {
		family = family_ext + family;
	}
	printf("CPU family id:\n");
	printf("0x%x\n", family);
	//printBin(family);
	
	if(family == 0x06 || family == 0x0f) {
		model = (model_ext << 4) + model;
	}
	printf("CPU model number:\n");
	printf("0x%x\n", model);
	//printBin(model);

	//processor brand index can be used to find processor frequency
	printf("Use the following processor brand index with table 3-14 in the Intel developer manual to lookup the brand string.\n");
	printf("Use the brand string to find base processor frequency following figure 3-10.\n");
	printf("Processor brand index:\n");
	printf("%x\n", procType);
	
//Features----------------------------------------------------------------------------
	printf("\n");
	printf("Features:\n");
	int i = 0;
	//features found from ecx
	for (i = 0; i < 32; i++) {
		if(ecxF[i] != "Reserved"){
			if((1 << i) & ecx) {
				printf("%s SUPPORTED\n", ecxF[i]);
			}
			else {
				printf("%s NOT SUPPORTED\n", ecxF[i]);
			}
		}
	}
	//features found from edx
	for (i = 0; i < 32; i++) {
		if(edxF[i] != "Reserved"){
			if((1 << i) & edx) {
				printf("%s SUPPORTED\n", edxF[i]);
			}
			else {
				printf("%s NOT SUPPORTED\n", edxF[i]);
			}
		}
	}

//Address information-----------------------------------------------------------------
	printf("\n");
	eax = 0x80000008;
	__asm__ __volatile__(
	                     "cpuid;"
	                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
	                     : "a"(eax)
	                     );
	//physical address bits 0-7
	unsigned int physical = (0xff & eax);
	//linear address bits 8-15
	unsigned int linear = (0xff00 & eax) >> 8;
	
	printf("physical address width:\n");
	printBin(physical);
	printf("logical address width:\n");
	printBin(linear);
	
//L2 Cache information--------------------------------------------------------------
	printf("\n");
	//load different values into ecx to get L1 cache info. Unlear in manual.
	eax = 0x80000006;
	__asm__ __volatile__(
	                     "cpuid;"
	                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
	                     : "a"(eax)
	                     );
	//cache line size in bytes, bits 0-7
	unsigned int cLineSize = (0xff & ecx);
	//total cache size in 1K units, bits 16-31
	unsigned int cacheSize = (0xffff0000 & ecx) >> 16;
	
	printf("L2 cache line size:\n");
	printf("%dB\n", cLineSize);
	printf("L2 cache size:\n");
	printf("%dK\n", cacheSize);
	
	
	
	eax = 0x02;
	__asm__ __volatile__(
	                     "cpuid;"
	                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
	                     : "a"(eax)
	                     );
	                     
//TLB, cache information, and memory hierarchy information----------------------------------------------------------
	printf("\n");
	printf("TLB, cache information, and memory hierarchy (use table 3-12 for interpretation):\n");
	//Make sure register has valid info
	if((1 << 31) && eax) {
		//most sig byte in eax should be ignored
		int i = 0;
		for( i = 0; i < 3; i++) {
			unsigned int mask = 0xff << ((i + 1) * 4);
			unsigned int current_byte = (mask & eax) >> ((i + 1) * 4);
			//check for null data
			if(current_byte != 0x00) {
				printf("0x%x\n", current_byte);
			}
		}
	}
	//Make sure register has valid info
		if((1 << 31) && ebx) {
		int i = 0;
		for( i = 0; i < 4; i++) {
			unsigned int mask = 0xff << (i * 4);
			unsigned int current_byte = (mask & ebx) >> (i * 4);
			//check for null data
			if(current_byte != 0x00) {
				printf("0x%x\n", current_byte);
			}
		}
	}
	//Make sure register has valid info
		if((1 << 31) && ecx) {
		int i = 0;
		for( i = 0; i < 4; i++) {
			unsigned int mask = 0xff << (i * 4);
			unsigned int current_byte = (mask & ecx) >> (i * 4);
			//check for null data
			if(current_byte != 0x00) {
				printf("0x%x\n", current_byte);
			}
		}
	}
	//Make sure register has valid info
		if((1 << 31) && edx) {
		int i = 0;
		for( i = 0; i < 4; i++) {
			unsigned int mask = 0xff << (i * 4);
			unsigned int current_byte = (mask & edx) >> (i * 4);
			//check for null data
			if(current_byte != 0x00) {
				printf("0x%x\n", current_byte);
			}
		}
	}

	return 0;
}
