BINDIR=bin
BUILDDIR=build
SRCDIR=src
INCDIR=inc
TARGET=$(BINDIR)/elevator

DEP=$(INCDIR)/elevator.h

OBJECTS=$(BUILDDIR)/elevator.o

CC=gcc
CFLAGS=-I.


$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(DEP)
	$(CC) -I$(INCDIR) -c $< -o $@

clean:
	@echo "cleaning..."
	rm $(BINDIR)/*
	rm $(BUILDDIR)/*