rawexample:rawexample.o
	gcc -o rawexample rawexample.o

rawexample.o:rawexample.c
	gcc -c rawexample.c
