INCLDIR = ./include
CC      = gcc
CFLAGS  = -O2
CFLAGS += -I$(INCLDIR)

OBJDIR  = obj

_DEPS = task6.h
DEPS  = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS = task6.o task6_client.o task6_main.o
OBJS  = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

task6: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~ task6
