all: scheduling scheduling-aging

scheduling: scheduling.c
	gcc -Wall -pthread -o scheduling scheduling.c

scheduling-aging: scheduling-aging.c
	gcc -Wall -pthread -o scheduling-aging scheduling-aging.c

run: all
	./scheduling
	./scheduling-aging

clean:
	rm -f scheduling scheduling-aging