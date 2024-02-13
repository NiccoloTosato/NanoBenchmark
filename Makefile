# makefile
CC = clang
CFLAGS = -Wall -Wextra 
LDFLAGS = -lm

all: cpu.avx cpu.avx2 cpu.avx512.1 cpu.avx512.2
cpu.avx:
	$(CC) $(CFLAGS) cpu.c -o cpu.avx.x $(LDFLAGS) -mavx -D__AVX__ -DDOUBLE -DAVX_COUNT=1
cpu.avx2:
	$(CC) $(CFLAGS) cpu.c -o cpu.avx2.x $(LDFLAGS) -mavx2 -D__AVX2__ -DDOUBLE -DAVX_COUNT=1
cpu.avx512.1:
	$(CC) $(CFLAGS) cpu.c -o cpu.avx512.1.x $(LDFLAGS) -mavx512f -D__AVX512__ -DDOUBLE -DAVX_COUNT=1
cpu.avx512.2:
	$(CC) $(CFLAGS) cpu.c -o cpu.avx512.2.x $(LDFLAGS) -mavx512f -D__AVX512__ -DDOUBLE -DAVX_COUNT=2

clean:
	rm *.x
