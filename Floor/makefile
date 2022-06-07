BINDIR=bin
BUILDDIR=build
SRCDIR=src
INCDIR=inc
TARGET=$(BINDIR)/floor
ARGS=-pthread -lm

DEP=$(INCDIR)/floor.h $(INCDIR)/random.h $(INCDIR)/LL_Floor.h

OBJECTS=$(BUILDDIR)/floor.o $(BUILDDIR)/random.o $(BUILDDIR)/LL_Floor.o $(BUILDDIR)/main.o 

CC=gcc
CFLAGS=-I.


$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(ARGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(DEP)
	$(CC) -I $(INCDIR) -c $< -o $@

clean:
	@echo "cleaning..."
	rm $(BINDIR)/*
	rm $(BUILDDIR)/*