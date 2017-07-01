all: slave
slave: slave.c
	gcc -Wall -pthread -o slave slave.c -lpigpiod_if2 -lrt

clean:
	rm -rf ./slave
