ALL: default

CC           = gcc
CLINKER      = $(CC)
OPTFLAGS     = -O0


SHELL = /bin/sh

CFLAGS  =   -DREENTRANT
CCFLAGS = $(CFLAGS)
LIBS =  -lpthread

EXECS = dsm.o exemple

default: $(EXECS)

exemple: exemple.o dsm.o
	$(CLINKER) $(OPTFLAGS) -o exemple exemple.o dsm.o  $(LIBS)
	mv exemple ../Phase1/bin

clean:
	@-/bin/rm -f *.o *~ PI* $(EXECS) *.out core
.c:
	$(CC) $(CFLAGS) -o $* $< $(LIBS)
.c.o:
	$(CC) $(CFLAGS) -c $<
.o:
	${CLINKER} $(OPTFLAGS) -o $* $*.o $(LIBS)
