CC=gcc
CFLAGS=-O2 -Wall
ARCH := $(shell uname -m)

ifeq ($(ARCH), x86_64)
ALL= average_avx512\
		average_avx2\
		average_sse\
		scalarxmat44\
		mat44xmat44\
        vect4xmat44
all: $(ALL)

average_sse: average_sse.c
	$(CC) $(CFLAGS) average_sse.c -o average_sse

average_avx2: average_avx2.c
	$(CC) -mavx2 $(CFLAGS) average_avx2.c -o average_avx2

average_avx512: average_avx512.c
	$(CC) -mavx512f $(CFLAGS) average_avx512.c -o average_avx512

scalarxmat44: scalarxmat44.c
	$(CC) $(CFLAGS) -mavx2 -mavx512f scalarxmat44.c -o scalarxmat44
mat44xmat44: mat44xmat44.c
	$(CC) $(CFLAGS) -mavx2 -mavx512f mat44xmat44.c -o mat44xmat44
vect4xmat44: vect4xmat44.c
	$(CC) $(CFLAGS) -mavx2 -mavx512f -mfma vect4xmat44.c -o vect4xmat44

else ifeq ($(ARCH), arm64)
CFLAGS += -march=native
ALL=average_neon
all: $(ALL)

average_neon: average_neon.c
	$(CC) $(CFLAGS) -o average_neon average_neon.c

else ifeq ($(ARCH), ppc64le)
CFLAGS += -mcpu=native
ALL=average_power
all: $(ALL)

average_power: average_power.c
	$(CC) $(CFLAGS) -o average_power average_power.c
endif
	

.PHONY: clean
clean:
	-rm -f $(ALL)
