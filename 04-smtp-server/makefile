INCLDIR = ./include
CC      = gcc
CFLAGS  = -O2
CFLAGS += -I$(INCLDIR)

OBJDIR  = obj

_DEPS = task5.h
DEPS  = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS = task5_main.o task5_server.o task5_utils.o
OBJS  = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

task5: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~ task5
