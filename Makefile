CC=g++

CFLAGS=-pthread -o

#C files .o
build: minhaBiblioteca.cpp
	$(CC) $(CFLAGS) minhaBiblioteca minhaBiblioteca.cpp

#Remove temp files
clean:
	rm -rf *.o exemplo

run: exemplo.cpp
	$(CC) $(CFLAGS) exemplo exemplo.cpp minhaBiblioteca.cpp
	./exemplo
	rm -rf *.o exemplo

debug: exemplo.cpp
	$(CC) -g $(CFLAGS) exemplo exemplo.cpp minhaBiblioteca.cpp
	gdb ./exemplo
	rm -rf *.o exemplo

timeit: exemplo.cpp
	$(CC) -g $(CFLAGS) exemplo exemplo.cpp minhaBiblioteca.cpp
	time ./exemplo
	rm -rf *.o exemplo


