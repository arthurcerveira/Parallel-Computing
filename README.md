# Trabalho Prático de SO

Esse trabalho consiste na implementação de uma biblioteca de processamento concorrente em C++, utilizando o padrão _Pthreads_.

O funcionamento dessa biblioteca é baseado na criação de tarefas e segue o modelo de execução _NxM_, onde _N_ representa o número de tarefas a serem realizadas, e _M_ representa os processadores virtuais disponíveis para executar essas tarefas.

Foram implementados 3 algoritmos de escalonamento para a biblioteca: FCFS, SJF, e PRIOc. O algoritmo é definido pela macro `ESCALONAMENTO` no arquivo `minhaBiblioteca.cpp`.  

### Dificuldades e soluções

As maiores dificldades encontradas foram relacionadas à depuração dos programas em paralelo. Para solucionar esses problemas, foram utilizadas as ferramentas `valgrind` e `gdb`. 

### Instruções para execução

Para executar o programa exemplo, basta rodar o comando `run` do Makefile.

```bash
$ make run
```

### Integrante
 * Arthur Alves Cerveira