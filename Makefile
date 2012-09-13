all: kmerge ksend

kmerge: kmerge.c
	gcc -o kmerge kmerge.c

ksend: ksend.c
	gcc -o ksend kmerge.c -lftdi

install: ksend kmerge kprogram kgui
	install -m 755 ksend /usr/bin
	install -m 755 kmerge /usr/bin
	install -m 755 kprogram /usr/bin
	install -m 755 kgui /usr/bin

clean:
	rm -f ksend kmerge
