#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define KB 2^10
#define MB 2^20
#define CACHE_SWITCH 100

//structure = 1 gigabyte
//(2^20) / 4 = (2^20) / (2^2) = 2^18
#define R_SIZE 2^18
struct s {
	uint32_t r[R_SIZE];
};

void main(void)
{
	struct s data = {0};
	int i = 0;
 	clock_t i1kb = 0, f1kb = 0, i2kb = 0, f2kb = 0, i4kb = 0, f4kb = 0, i8kb, f8kb, i16kb = 0, f16kb = 0,
					 i32kb = 0, f32kb = 0, i64kb = 0, f64kb = 0, i128kb = 0, f128kb = 0, i256kb = 0, f256kb = 0,
	         i1mb = 0, f1mb = 0, i2mb = 0, f2mb = 0, i4mb = 0, f4mb = 0, i8mb, f8mb, i16mb = 0, f16mb = 0,
	         i32mb = 0, f32mb = 0, i64mb = 0, f64mb = 0, i128mb = 0, f128mb = 0, i256mb = 0, f256mb = 0,
	         i512mb = 0, f512mb = 0, i900mb = 0, f900mb = 0;

	for(i = 0; i < (256 * KB); i++) {
		i900mb = clock();
		i512mb = clock();
		i256mb = clock();
		i128mb = clock();
		i32mb = clock();
		i16mb = clock();
		i8mb = clock();
		i4mb = clock();
		i2mb = clock();
		i1mb = clock();
		i256kb = clock();
		i128kb = clock();
		i64kb = clock();
		i32kb = clock();
		i16kb = clock();
		i8kb = clock();
		i4kb = clock();
		i2kb = clock();
		i1kb = clock();
		data.r[i];
		f1kb += (clock() - i1kb) & (i < KB);
		f2kb += (clock() - i2kb) & (i < (2 *KB));
		f4kb += (clock() - i4kb) & (i < (4 *KB));
		f8kb += (clock() - i8kb) & (i < (8 *KB));
		f16kb += (clock() - i16kb) & (i < (16 *KB));
		f32kb += (clock() - i32kb) & (i < (32 *KB));
		f64kb += (clock() - i64kb) & (i < (64 *KB));
		f128kb += (clock() - i128kb) & (i < (128 *KB));
		f256kb += (clock() - i256kb) & (i < (256 *KB));
		f1mb += (clock() - i1mb) & (i < (1 *MB));
		f2mb += (clock() - i2mb) & (i < (2 *MB));
		f4mb += (clock() - i4mb) & (i < (4 *MB));
		f8mb += (clock() - i8mb) & (i < (8 *MB));
		f16mb += (clock() - i16mb) & (i < (16 *MB));
		f32mb += (clock() - i32mb) & (i < (32 *MB));
		f64mb += (clock() - i64mb) & (i < (64 *MB));
		f128mb += (clock() - i128mb) & (i < (128 *MB));
		f256mb += (clock() - i256mb) & (i < (256 *MB));
		f512mb += (clock() - i512mb) & (i < (512 *MB));
		f900mb += (clock() - i900mb) & (i < (900 *MB));
	}
	
  printf("working set 1KB access took %ld clock ticks\n", f1kb);
  printf("working set 2KB access took %ld clock ticks\n", f2kb);
  printf("working set 4KB access took %ld clock ticks\n", f4kb);
  printf("working set 8KB access took %ld clock ticks\n", f8kb);
  printf("working set 16KB access took %ld clock ticks\n", f16kb);
  printf("working set 32KB access took %ld clock ticks\n", f32kb);
  printf("working set 64KB access took %ld clock ticks\n", f64kb);
  printf("working set 128KB access took %ld clock ticks\n", f128kb);
  printf("working set 256KB access took %ld clock ticks\n", f256kb);
  printf("working set 1MB access took %ld clock ticks\n", f1mb);
  printf("working set 2MB access took %ld clock ticks\n", f2mb);
  printf("working set 4MB access took %ld clock ticks\n", f4mb);
  printf("working set 8MB access took %ld clock ticks\n", f8mb);
  printf("working set 16MB access took %ld clock ticks\n", f16mb);
  printf("working set 32MB access took %ld clock ticks\n", f32mb);
  printf("working set 64MB access took %ld clock ticks\n", f64mb);
  printf("working set 128MB access took %ld clock ticks\n", f128mb);
  printf("working set 256MB access took %ld clock ticks\n", f256mb);
  printf("working set 512MB access took %ld clock ticks\n", f256mb);
  printf("working set 900MB access took %ld clock ticks\n", f900mb);
	
	printf("Your system has the following caches:\n");
	//I should have thought this part through earlier (so much copying)
	int l_num = 1;
	if( (int)(f2kb - f1kb) > CACHE_SWITCH) {
		printf("l%d cache of size %dKB\n", l_num, 1);
		l_num++;
	}
	if( (int)(f4kb - f2kb) > CACHE_SWITCH) {
		printf("l%d cache of size %dKB\n", l_num, 2);
		l_num++;
	}
	if( (int)(f8kb - f4kb) > CACHE_SWITCH) {
		printf("l%d cache of size %dKB\n", l_num, 4);
		l_num++;
	}
	if( (int)(f16kb - f8kb) > CACHE_SWITCH) {
		printf("l%d cache of size %dKB\n", l_num, 8);
		l_num++;
	}
	if( (int)(f32kb - f16kb) > CACHE_SWITCH) {
		printf("l%d cache of size %dKB\n", l_num, 16);
		l_num++;
	}
	if( (int)(f64kb - f32kb) > CACHE_SWITCH) {
		printf("l%d cache of size %dKB\n", l_num, 32);
		l_num++;
	}
	if( (int)(f128kb - f64kb) > CACHE_SWITCH) {
		printf("l%d cache of size %dKB\n", l_num, 64);
		l_num++;
	}
	if( (int)(f256kb - f128kb) > CACHE_SWITCH) {
		printf("l%d cache of size %dKB\n", l_num, 128);
		l_num++;
	}
	if( (int)(f1mb - f256kb) > CACHE_SWITCH) {
		printf("l%d cache of size %dKB\n", l_num, 256);
		l_num++;
	}
	if( (int)(f2mb - f1mb) > CACHE_SWITCH) {
		printf("l%d cache of size %dMB\n", l_num, 1);
		l_num++;
	}
	if( (int)(f4mb - f2mb) > CACHE_SWITCH) {
		printf("l%d cache of size %dMB\n", l_num, 2);
		l_num++;
	}
	if( (int)(f8mb - f4mb) > CACHE_SWITCH) {
		printf("l%d cache of size %dMB\n", l_num, 4);
		l_num++;
	}
	if( (int)(f16mb - f8mb) > CACHE_SWITCH) {
		printf("l%d cache of size %dMB\n", l_num, 8);
		l_num++;
	}
	if( (int)(f32mb - f16mb) > CACHE_SWITCH) {
		printf("l%d cache of size %dMB\n", l_num, 16);
		l_num++;
	}
	if( (int)(f64mb - f32mb) > CACHE_SWITCH) {
		printf("l%d cache of size %dMB\n", l_num, 32);
		l_num++;
	}
	if( (int)(f128mb - f64mb) > CACHE_SWITCH) {
		printf("l%d cache of size %dMB\n", l_num, 64);
		l_num++;
	}
	if( (int)(f256mb - f128mb) > CACHE_SWITCH) {
		printf("l%d cache of size %dMB\n", l_num, 128);
		l_num++;
	}
	if( (int)(f512mb - f256mb) > CACHE_SWITCH) {
		printf("l%d cache of size %dMB\n", l_num, 256);
		l_num++;
	}
	if( (int)(f900mb - f512mb) > CACHE_SWITCH) {
		printf("l%d cache of size %dMB\n", l_num, 512);
		l_num++;
	}
}
