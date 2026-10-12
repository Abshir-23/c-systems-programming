INCLDIR = ./include
CC = gcc
CFLAGS = -O2
CFLAGS += -I$(INCLDIR)

OBJDIR = obj

_DEPS =
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS = task3_main.o task3_list.o task3_menu.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o:%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

task3: $(OBJS)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCLDIR)/*~ task3
