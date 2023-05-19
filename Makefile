#
# Ce Makefile contient les cibles suivantes :
#
# all   : compile le programme
# clean : supprime fichiers temporaires
CC = gcc

PROG = client server

CFLAGS = -g  # obligatoires

.PHONY: all clean

all: $(PROG)

clean:
	rm -f $(PROG) *.o
	rm -f *.aux *.log *.out *.pdf
	rm -f moodle.tgz

