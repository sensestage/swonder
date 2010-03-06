all: twondercont_1989.so twondercont_4ch_1990.so twondercont_2ch_1991.so 

twondercont_1989.so: twondercont_1989.o
	gcc -o twondercont_1989.so -shared -nostartfiles -llo twondercont_1989.o

twondercont_1989.o: twondercont_1989.c
	gcc -c -fPIC -o twondercont_1989.o -I../src twondercont_1989.c

twondercont_1989.c: twondercont_1989.xml
	./xml2c.pl twondercont_1989.xml >twondercont_1989.c


twondercont_4ch_1990.so: twondercont_4ch_1990.o
	gcc -o twondercont_4ch_1990.so -shared -nostartfiles -llo twondercont_4ch_1990.o

twondercont_4ch_1990.o: twondercont_4ch_1990.c
	gcc -c -fPIC -o twondercont_4ch_1990.o -I../src twondercont_4ch_1990.c

twondercont_4ch_1990.c: twondercont_4ch_1990.xml
	./xml2c.pl twondercont_4ch_1990.xml >twondercont_4ch_1990.c


twondercont_2ch_1991.so: twondercont_2ch_1991.o
	gcc -o twondercont_2ch_1991.so -shared -nostartfiles -llo twondercont_2ch_1991.o

twondercont_2ch_1991.o: twondercont_2ch_1991.c
	gcc -c -fPIC -o twondercont_2ch_1991.o -I../src twondercont_2ch_1991.c

twondercont_2ch_1991.c: twondercont_2ch_1991.xml
	./xml2c.pl twondercont_2ch_1991.xml >twondercont_2ch_1991.c

clean:
	rm *.c *.so *.o

