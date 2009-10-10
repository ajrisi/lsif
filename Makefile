lsif:	lsif.c
	gcc -Wall -o lsif lsif.c

clean:
	rm -f lsif 2> /dev/null

distclean:	clean
	rm -f *~ 2> /dev/null
