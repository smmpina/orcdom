CFLAGS = -std=c11 -Wall -pedantic

desp: orcdom.o ordenacao.o manut.o fopag.o relat.o \
		pagamento.o impress.o newlib.o config.o brws.o menuh.o
	gcc -o desp *.o -lm

orcdom.o: orcdom.c
	gcc $(CFLAGS) -c orcdom.c

newlib.o: newlib.c
	gcc $(CFLAGS) -c newlib.c

ordenacao.o: ordenacao.c
	gcc $(CFLAGS) -c ordenacao.c

manut.o: manut.c
	gcc $(CFLAGS) -c manut.c

fopag.o: fopag.c
	gcc $(CFLAGS) -c fopag.c

relat.o: relat.c
	gcc $(CFLAGS) -c relat.c

pagamento.o: pagamento.c
	gcc $(CFLAGS) -c pagamento.c

impress.o: impress.c
	gcc $(CFLAGS) -c impress.c

config.o: config.c
	gcc $(CFLAGS) -c config.c

brws.o: brws.c
	gcc $(CFLAGS) -c brws.c

menuh.o: menuh.c
	gcc $(CFLAGS) -c menuh.c

run:
	./desp

clean:
	rm -rf desp *.o
