CC = gcc
CFLAGS =

# Add any header files you've added here.
HDRS = 
# Add any source files you've added here.
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

all: $(OBJS)

make: $(SRC)
	$(CC) $(CFLAGS) -o make $(OBJS)
clean:
	rm -f .*.d *.o $(TAR) *~ make
