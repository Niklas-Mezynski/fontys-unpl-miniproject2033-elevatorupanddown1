BINDIR=bin
BUILDDIR=build
SRCDIR=src
INCDIR=inc
TARGET=$(BINDIR)/manager

DEP=$(INCDIR)/manager.h

OBJECTS=$(BUILDDIR)/manager.o $(BUILDDIR)/main.o

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