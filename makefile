all: write.c control.c
	gcc -o control control.c
	gcc -o write write.c

clean:
	rm *.o
	rm *~
