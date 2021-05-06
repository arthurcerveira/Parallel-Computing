CC=g++

CFLAGS=-pthread -o

#C files .o
build: minhaBiblioteca.cpp
	$(CC) $(CFLAGS) minhaBiblioteca minhaBiblioteca.cpp

#Run test
run: minhaBiblioteca.cpp
	$(CC) $(CFLAGS) minhaBiblioteca minhaBiblioteca.cpp
	./minhaBiblioteca
	rm -rf *.o minhaBiblioteca

#Remove temp files
clean:
	rm -rf *.o minhaBiblioteca exemplo

example: exemplo.cpp
	$(CC) $(CFLAGS) exemplo exemplo.cpp minhaBiblioteca.cpp
	./exemplo
	rm -rf *.o exemplo


