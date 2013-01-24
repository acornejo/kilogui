all: kbmerge kbsend kilogui/kilogui

kbmerge: kbmerge.c
	gcc -o kbmerge kbmerge.c

kbsend: kbsend.c
	gcc -o kbsend kbsend.c -lftdi

kilogui/kilogui:
	qmake kilogui -o kilogui/Makefile
	make -C kilogui 

install: kbsend kbmerge kbprogram kbcompile kilogui/kilogui
	install -m 755 kbsend /usr/bin
	install -m 755 kbmerge /usr/bin
	install -m 755 kbcompile /usr/bin
	install -m 755 kbprogram /usr/bin
	install -m 755 kilogui/kilogui /usr/bin

clean:
	rm -f kbsend kbmerge kilogui/kilogui
	make -C kilogui clean
