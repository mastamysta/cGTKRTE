IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj
LDIR =../lib

LIBS = `pkg-config --libs gtk+-3.0` -lhunspell-1.7
PACKAGE = `pkg-config --cflags --libs gtk+-3.0`

_DEPS = maingraphics.h debugmsg.h spellcheck.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o maingraphics.o spellcheck.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(PACKAGE)

buk: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
	
.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
