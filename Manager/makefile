BINDIR=bin
BUILDDIR=build
SRCDIR=src
INCDIR=inc
TARGET=$(BINDIR)/manager
ARGS=-pthread  -lm

DEP=$(INCDIR)/manager.h $(INCDIR)/elevator.h $(INCDIR)/LL.h

OBJECTS=$(BUILDDIR)/manager.o $(BUILDDIR)/elevator.o $(BUILDDIR)/LL.o $(BUILDDIR)/main.o

CC=gcc
CFLAGS=-I.


$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(ARGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(DEP)
	$(CC) -I $(INCDIR) -c $< -o $@

valgrind:
	$(CC) $(OBJECTS) -o $(BINDIR)/managerDebug $(ARGS) -g -O0

clean:
	@echo "cleaning..."
	rm $(BUILDDIR)/*
	rm $(BINDIR)/*