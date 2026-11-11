all: scheduling

scheduling: scheduling.c
	gcc -Wall -pthread -o scheduling scheduling.c

run: scheduling
	./scheduling

clean:
	rm -f scheduling