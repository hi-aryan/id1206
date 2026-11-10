all: parallel-sum

parallel-sum: parallel-sum.c
	gcc -Wall -O2 -pthread -o parallel-sum parallel-sum.c

run: parallel-sum
	./parallel-sum 1
	./parallel-sum 2
	./parallel-sum 4

clean:
	rm -f parallel-sum