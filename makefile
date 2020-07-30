delaunay:
	find -name delaunay -delete
	gcc -o delaunay main.c delaunay.c utils.c -I. -Wall -pedantic -lm

test:
	find -name test -delete
	gcc -o test test.c -Wall -pedantic