BINDIR=bin
BUILDDIR=build
SRCDIR=src
INCDIR=inc
TARGET=$(BINDIR)/manager

DEP=$(INCDIR)/manager.h $(INCDIR)/elevator.h $(INCDIR)/LL.h

OBJECTS=$(BUILDDIR)/manager.o $(BUILDDIR)/elevator.o $(BUILDDIR)/LL.o $(BUILDDIR)/main.o

CC=gcc
CFLAGS=-I.


$(TARGET): $(OBJECTS)
	$(CC) -pthread $^ -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(DEP)
	$(CC) -I $(INCDIR) -c $< -o $@

clean:
	@echo "cleaning..."
	rm $(BUILDDIR)/*
	rm $(BINDIR)/*