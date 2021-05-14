#include "minhaBiblioteca.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <list>
#include <iostream>
#include <unistd.h>
using namespace std;

#define SUCCESS 1
#define FAILURE 0

// Algoritmos de Escalonamento
// 0 FCFS
// 1 SJF
// 2 PRIOc
#define ESCALONAMENTO 0

bool isRunning = true;

list <process *> processesReady, processesFinished;

static pthread_t * pvs;
pthread_mutex_t lock;
pthread_mutexattr_t attr;

bool processIsRunning() {
    // Verifica se há processos pendentes
    if (!processesReady.empty()) return true;

    // Verifica se a função finish foi chamada
    return isRunning;
}

void storeResult(struct process * finishedProcess, void * returnValue){
    // Adiciona valor de retorno
    finishedProcess->returnValue = returnValue;

    // Armazena na lista de processos terminados
    processesFinished.push_back(finishedProcess);
}

struct process * getProcess(){
    struct process * currentProcess;

    if (processesReady.empty()) return NULL;

    currentProcess = processesReady.front();

    // Se encontrar, remove da lista de processos prontos
    processesReady.remove(currentProcess);

    return currentProcess;
}

void* myProcessor(void* dta) {
    void* returnValue;
    struct process *currentProcess;

    while (processIsRunning()) {
        // Lock para modificar a processesReady
        // if (!pthread_mutex_trylock(&lock)) continue;
        pthread_mutex_lock(&lock);

        currentProcess = getProcess();

        pthread_mutex_unlock(&lock);

        // Se não encontrar, volta ao início do loop
        if (!currentProcess) continue;

        // Executa função
        returnValue = currentProcess->function(currentProcess->param);

        // Guarda resultado na lista de processos terminados
        storeResult(currentProcess, returnValue);
    }

    return NULL;
}

int start(int m) {
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&lock, &attr);

    int ret = 0;
    // Aloca memoria para os threads
    pvs = (pthread_t *)malloc(m * sizeof(pthread_t));

    for( int i = 0 ; i < m; i++ ) {
        // Para cada thread, inicializa com a função myProcessor
        ret |= pthread_create(&pvs[i], NULL, myProcessor, NULL);
    }

    return ret;
}

void finish(){
    // Atualiza a variavel isRunning com valor falso
    isRunning = false;

    // Esse loop cria um deadlock na aplicação, por isso foi comentado
    // A aplicação funciona corretamente sem ele
     for ( int i = 0 ; i < 4; i++ ) {
         pthread_join(pvs[i], NULL);
     }
}

int spawn(struct Atrib *atrib, void *(*t)(void *), void *dta) {
    static int id = 0;
    // Lock do mutex aqui?
    int processId = id++;  // ID começa em 1 e é atualizado em cada execução de spawn

    // Cria o processo
    struct process * spawnedProcess = (struct process *)malloc(sizeof(struct process));

    spawnedProcess->function = t;
    spawnedProcess->param = dta;
    spawnedProcess->processId = processId;
    spawnedProcess->atrib = atrib;

// Escolhe o processo de acordo com algoritmo de escalonamento
#if ESCALONAMENTO == 0  // FCFS
    // Mutex de modificação na lista aqui?
    processesReady.push_back(spawnedProcess);
#endif

#if ESCALONAMENTO == 1  // SJF
    list <process *> :: iterator processIterator;
    bool inserted = false;

    for (processIterator = processesReady.begin();
         processIterator != processesReady.end();
         processIterator++) {
        // Percorre a lista até encontrar um custo maior
        if ((*processIterator)->atrib->c > spawnedProcess->atrib->c) {
            // Ao encontrar, insere processo antes dele
            processesReady.insert(processIterator, spawnedProcess);
            inserted = true;
            break;
        }
    }

    if (!inserted) processesReady.push_back(spawnedProcess);
#endif

#if ESCALONAMENTO == 2  // PRIOp
    list <process *> :: iterator processIterator;
    bool inserted = false;

    for (processIterator = processesReady.begin();
         processIterator != processesReady.end();
         processIterator++) {
        // Percorre a lista até encontrar um prioridade menor
        if ((*processIterator)->atrib->p < spawnedProcess->atrib->p){
            // Ao encontrar, insere processo antes dele
            processesReady.insert(processIterator, spawnedProcess);
            inserted = true;
            break;
        }
    }

    if (!inserted) processesReady.push_back(spawnedProcess);
#endif

    return processId;
}

int sync(int tId, void **res) {
    // Procura a thread por ID na lista de processos prontos
    list <process *> :: iterator processIterator;

    pthread_mutex_lock(&lock);

    for (processIterator = processesReady.begin();
         processIterator != processesReady.end();
         processIterator++) {
        if ((*processIterator)->processId == tId) {
            // Retira a tarefa da lista de tarefas prontas
            processesReady.remove(*processIterator);

            // Executa a tarefa e atribui à res
            *res = (*processIterator)->function((*processIterator)->param);

            return SUCCESS;
        }
    }

    pthread_mutex_unlock(&lock);

    while (true) {
        // Se não encontrar, procura na lista de processos terminados
        for (processIterator = processesFinished.begin();
             processIterator != processesFinished.end();
             processIterator++) {
            if ((*processIterator)->processId == tId) {
                // Retira a tarefa da lista de tarefas terminadas
                processesFinished.remove(*processIterator);

                // Salva seu valor de retorno em res
                *res = (*processIterator)->returnValue;

                return SUCCESS;
            }
        }

        // Se não encontrar, espera 0.1 segundo e tenta de novo
        usleep(100);
    }
}
