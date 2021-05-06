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

bool processIsRunning() {
//    cout << "processIsRunning\n";
    // Verifica se há processos pendentes
    if (!processesReady.empty()) return true;

    // Verifica se a função finish foi chamada
    return isRunning;
}

void storeResult(struct process * finishedProcess, void * returnValue){
    cout << "storeResult\n";
    // Remove processo da processesReady
    processesReady.remove(finishedProcess);

    // Adiciona valor de retorno
    finishedProcess->returnValue = returnValue;
    cout << "ret3: " << *(int*)returnValue << "\n";

    // Armazena na lista de processos processesFinished
    processesFinished.push_back(finishedProcess);
}

struct process * getProcess(){
    struct process * currentProcess;

    currentProcess = processesReady.front();

    if (!currentProcess || currentProcess->executed) return NULL;

    return currentProcess;
}

void* myProcessor(void* dta) {
    cout << "myProcessor\n";

    // Inicializa valor de retorno e processo
    void* returnValue;
    struct process *currentProcess;

    // Enquanto aplicação estiver sendo executada
    while (processIsRunning()) {
        // Procura o processo atual
        currentProcess = getProcess();

        if (!currentProcess) continue;

        // cout << currentProcess->processId << '\n';
        // Se encontrar, executa função
        currentProcess->executed = true;
        returnValue = currentProcess->function(currentProcess->param);
        storeResult(currentProcess, returnValue);
    }

    return NULL;
}

int start(int m) {
    cout << "Start\n";

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
    cout << "Finish\n";
    // Atualiza a variavel isRunning com valor falso
    isRunning = false;

    for ( int i = 0 ; i < 4; i++ ) {
        // Para cada thread, inicializa com a função MeuPV
        pthread_join(pvs[i], NULL);
    }
}

int spawn(struct Atrib *atrib, void *(*t)(void *), void *dta) {
    cout << "Spawn\n";

    static int id = 0;
    id++;  // ID começa em 1 e é atualizado em cada execução de spawn

    // Cria o processo
    struct process * spawnedProcess = (struct process *)malloc(sizeof(struct process));

    spawnedProcess->function = t;
    spawnedProcess->param = dta;
    spawnedProcess->processId = id;
    spawnedProcess->executed = false;

    // Adiciona à lista
    processesReady.push_back(spawnedProcess);

    return id;
}

int sync(int tId, void **res) {
    // struct process * syncedProcess;
    cout << "Sync\n";

    // Procura a thread por ID na lista de processos terminados
    list <process *> :: iterator processIterator;

    for(processIterator = processesFinished.begin();
        processIterator != processesFinished.end();
        ++processIterator) {
        if ((*processIterator)->processId == tId) {
            *res = (*processIterator)->returnValue;

            return SUCCESS;
        }
    }

    for(processIterator = processesReady.begin();
        processIterator != processesReady.end();
        ++processIterator) {
        if ((*processIterator)->processId == tId) {
            // Espera processo ser executado
            *res = (*processIterator)->function((*processIterator)->param);

            storeResult((*processIterator), *res);

            return SUCCESS;
        }
    }

    // Se nao encontrar, retorna FAILURE
    return FAILURE;
}
