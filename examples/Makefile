# https://github.com/nesro/cassertion

FILES=test01 test02

CC=gcc
CFLAGS=-std=c99 -Wall -pedantic
CLIBS=-fopenmp

all: $(FILES)

test01: test01.c
	$(CC) $(CFLAGS) -o $@ $< $(CLIBS)

test02: test02.c
	$(CC) $(CFLAGS) -o $@ $< $(CLIBS)

.PHONY: clean
clean:
	rm -f $(FILES) ./cassertion_log/*

.PHONY: cassertion
cassertion:
	../cassertion.sh
