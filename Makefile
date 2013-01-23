all: kbmerge kbsend kilogui

kbmerge: kbmerge.c
	gcc -o kbmerge kbmerge.c

kbsend: kbsend.c
	gcc -o kbsend kbsend.c -lftdi

kilogui:
	qmake qtgui -o qtgui/Makefile
	make -C qtgui

install: kbsend kbmerge kbprogram kbgui
	install -m 755 kbsend /usr/bin
	install -m 755 kbmerge /usr/bin
	install -m 755 kbcompile /usr/bin
	install -m 755 kbprogram /usr/bin
	install -m 755 kbgui /usr/bin

clean:
	rm -f kbsend kbmerge
