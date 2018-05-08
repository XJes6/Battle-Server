$(CC) = gcc
SRCS = $(wildcard *.c) Queue.h
PROGS = $(patsubst %.c,%,$(SRCS))

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS)  -o $@ $<
