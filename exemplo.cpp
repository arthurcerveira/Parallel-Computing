#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include "minhaBiblioteca.h"
#include <iostream>
using namespace std;

void* fibo( void* dta ) {
    int t1, t2;
    int n = *(int*) dta;
    int *n1, *n2,
        *r = (int *) malloc(sizeof(int)),
        *r1, *r2;
    struct Atrib a1, a2;
    if( n <= 2 ) *r = 1;
    else {
        n1 = (int *) malloc(sizeof(int));
        *n1 = n - 1;
        a1.p = 0; a1.c = *n1;
        t1 = spawn( &a1, fibo, (void*) n1 );
        n2 = (int *) malloc(sizeof(int));
        *n2 = n - 2;
        a2.p = 0; a2.c = *n2;
        t2 = spawn( &a2, fibo, (void*) n2 );
        sync( t1, (void**) &r1 );
        sync( t2, (void**) &r2 );
        *r = *r1 + *r2;
        free(r1); free (r2);
        free(n1); free (n2);
    }
    return r;
}

void set_args(int argc, char** argv, int* m, int* n) {
    if(argc == 3)  {
        // Argumento 1: nº de processadores
        sscanf(argv[1], "%d", m);  // sscanf converte string para int
        // Argumento 2: entrada do fibonacci
        sscanf(argv[2], "%d", n);
    } else {
        // Utiliza valores padrão
        *m = 4;
        *n = 10;
    }
}


int main(int argc, char** argv) {
    int n, *r, tId, m;
    struct Atrib a;

    // Argumentos da linha de comando
    set_args(argc, argv, &m, &n);

    start( m );

    r = (int*)malloc(sizeof(int));

    a.p = 0;
    a.c = n;
    tId = spawn( &a, fibo, &n);
    sync(tId,(void**) &r);

    printf("Fibonacci(%d) = %d\n", n, *(int *)r);

    finish(m);

    return 0;
}
