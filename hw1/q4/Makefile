all: histogram

histogram: histogram.c
	gcc -Wall -O2 -pthread -o histogram histogram.c

run: histogram
	./histogram 3 800 16
	./histogram 3 80000 16
	./histogram 3 8000000 16

clean:
	rm -f histogram