OUTDIR=bin/
BUILDDIR=build/
SRCDIR=src/
INCDIR=inc/

CC=gcc
CFLAGS=-I.

cOBJ = client.o 
sOBJ = server.o 
mOBJ = manager.o 

%.o: $(SRCDIR)%.c $(DEPS)
	$(CC) -c -o $(BUILDDIR)$@ $< $(CFLAGS)

client: $(cOBJ)
	$(CC) -o $(OUTDIR)$@ $^ $(CFLAGS)

manager: $(mOBJ)
	$(CC) -o $(OUTDIR)$@ $(BUILDDIR)$^ $(CFLAGS)
