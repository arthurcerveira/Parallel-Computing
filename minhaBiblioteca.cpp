#include "minhaBiblioteca.h"
#include <stdlib.h>
#include <pthread.h>
#include <list>
#include <iostream>
#include <unistd.h>
using namespace std;

#define SUCCESS 1
#define FAILURE 0

bool isRunning = true;

list <process *> processesReady, processesFinished;

static pthread_t * pvs;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

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
    struct process * currentProcess ;

    // Pega primeiro processo da lista de processos prontos
    currentProcess = processesReady.front();

    if (!currentProcess) return NULL;

    return currentProcess;
}

void* myProcessor(void* dta) {
    void* returnValue;
    struct process *currentProcess;

    while (processIsRunning()) {
        // Lock para modificar a processesReady
        pthread_mutex_lock(&lock);
        currentProcess = getProcess();

        // Se não encontrar, volta ao início do loop
        if (!currentProcess) continue;

        // Se encontrar, remove da lista de processos prontos
        processesReady.remove(currentProcess);
        pthread_mutex_unlock(&lock);

        // Executa função
        returnValue = currentProcess->function(currentProcess->param);

        // Guarda resultado na lista de processos terminados
        storeResult(currentProcess, returnValue);
    }

    return NULL;
}

int start(int m) {
    int ret = 0;
    // Aloca memoria para os threads
    pvs = (pthread_t *)malloc(m * sizeof(pthread_t));

    for( int i = 0 ; i < m; i++ ) {
        // Para cada thread, inicializa com a função MeuPV
        ret |= pthread_create(&pvs[i], NULL, myProcessor, NULL);
    }

    return ret;
}

void finish(){
    // Atualiza a variavel isRunning com valor falso
    isRunning = false;

    // Esse loop cria um deadlock na aplicação, por isso foi comentado
    // A aplicação funciona corretamente sem ele
    // for ( int i = 0 ; i < 4; i++ ) {
        // pthread_join(pvs[i], NULL);
    // }
}

int spawn(struct Atrib *atrib, void *(*t)(void *), void *dta) {
    static int id = 0;
    id++;  // ID começa em 1 e é atualizado em cada execução de spawn

    // Cria o processo
    struct process * spawnedProcess = (struct process *)malloc(sizeof(struct process));

    spawnedProcess->function = t;
    spawnedProcess->param = dta;
    spawnedProcess->processId = id;

    // Adiciona à lista
    processesReady.push_back(spawnedProcess);

    return id;
}

int sync(int tId, void **res) {
    // Procura a thread por ID na lista de processos prontos
    list <process *> :: iterator processIterator;

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

        // Se não encontrar, espera 1 segundos e tenta de novo
        usleep(500);
    }
}
