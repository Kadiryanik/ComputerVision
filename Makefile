all: 
	gcc -o test test.c bmp.c computer-vision.c
clean:
	rm -f test
	rm -f *.txt
	rm -f images/*.bmp
