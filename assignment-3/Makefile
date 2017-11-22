all: cacheSize endian_p4 endian_p5

cacheSize: cacheSize.c
	gcc -o cacheSize cacheSize.c -O0
endian_p4: endian_p4.c
	gcc -o endian_p4 endian_p4.c
endian_p5: endian_p5.c
	gcc -o endian_p5 endian_p5.c
run: cacheSize endian_p4 endian_p5
	./cacheSize
	./endian_p4
	./endian_p5
clean:
	rm -f cacheSize endian_p4 endian_p5
