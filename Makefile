CC=gcc
CFLAGS=-L/home/students/pskiba/Dokumenty/papi-5.4.1/src -I/home/students/pskiba/Dokumenty/papi-5.4.1/src -lpapi
OBJS=profiler.o

prof:
	gcc -o prof -O0 counters.c -L/home/students/pskiba/Dokumenty/papi-5.4.1/src -I/home/students/pskiba/Dokumenty/papi-5.4.1/src -lpapi

clean:
	rm -f *.o prof

run:
	./prof
