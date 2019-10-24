CC = gcc

build: huffman.c sort.c 
	$(CC) -o huffman huffman.c sort.c
	
bwlib: huffman.c libsort.a
	$(CC) -o huffman.o -c huffman.c
	$(CC) -o huffman huffman.o -L./ -lsort
