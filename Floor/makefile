BINDIR=bin
BUILDDIR=build
SRCDIR=src
INCDIR=inc
TARGET=$(BINDIR)/floor

DEP=$(INCDIR)/floor.h

OBJECTS=$(BUILDDIR)/floor.o

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