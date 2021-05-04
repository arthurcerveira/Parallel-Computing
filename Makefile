CC=gcc

CFLAGS=-o

#C files .o
build: minhaBiblioteca.c
	$(CC) $(CFLAGS) minhaBiblioteca minhaBiblioteca.c

#Run test
run: minhaBiblioteca
	./minhaBiblioteca
	rm -rf *.o minhaBiblioteca

#Remove temp files
clean:
	rm -rf *.o minhaBiblioteca
