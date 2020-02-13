all: 
	gcc -o test test.c computer-vision.c
clean:
	rm -f test
