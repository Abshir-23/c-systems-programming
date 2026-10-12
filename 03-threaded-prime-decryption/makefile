
INCLDIR = ./include
CC = gcc
CFLAGS = -O0
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -I$(INCLDIR)

OBJDIR = obj

_DEPS = task4_prime.h
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS = task4_threads.o task4_prime.o xtea.o ipc.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o:%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

task4: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) -lpthread

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~ task4
