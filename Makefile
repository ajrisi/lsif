lsif:	lsif.c
	gcc -Wall -o lsif lsif.c

clean:
	rm -f lsif 2> /dev/null

install:
	install -m 755 lsif /usr/bin

uninstall:
	rm -f /usr/bin/lsif

distclean:	clean
	rm -f *~ 2> /dev/null
