#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <emscripten/emscripten.h>
#include <emscripten/threading.h>
#include "algoritmos.c"

#define EXTERN

typedef struct {
    int* array;
    int size;
    int target;
} SearchArgs;

typedef struct {
    int index;
    char* message;
    char* outputDiv;
} SearchResult;

pthread_t createThread(void* (*function)(void*), void* args) {
    pthread_t thread;
    if (pthread_create(&thread, NULL, function, args)) {
        fprintf(stderr, "Error al crear el hilo\n");
        exit(1);
    }
    return thread;
}

void handleSearchResult(void* args) {
    SearchResult* result = (SearchResult*)args;
    EM_ASM({
        var divId = UTF8ToString($0);
        var resultMessage = UTF8ToString($1);
        document.getElementById(divId).innerText = resultMessage;
        _free($1);  
    }, result->outputDiv, result->message);
    free(result);  
}


void* printSequentialSearch(void* args) {
    SearchArgs* searchArgs = (SearchArgs*)args;
    double start_time = emscripten_get_now();
    int index = sequentialSearch(searchArgs->array, searchArgs->size, searchArgs->target);
    emscripten_sleep(2000);
    double end_time = emscripten_get_now();
    double elapsed_time = (end_time - start_time) / 1000.0; // Convertir de milisegundos a segundos

    
    char* message;
    char* timeMessage;
    if (index != -1) {
        message = (char*)malloc(100 * sizeof(char)); // Espacio para almacenar el mensaje
        timeMessage = (char*)malloc(100 * sizeof(char)); // Espacio para almacenar el mensaje de tiempo
        sprintf(message, "Encontrado en la posición %d usando BS.", index);
        sprintf(timeMessage, "Tiempo de búsqueda: %.6f segundos", elapsed_time);
    } else {
        message = strdup("No encontrado usando BS");
        timeMessage = strdup("");
    }
    
    SearchResult* result = (SearchResult*)malloc(sizeof(SearchResult));
    SearchResult* timeResult = (SearchResult*)malloc(sizeof(SearchResult));
    result->index = index;
    result->message = message;
    result->outputDiv = "sequentialSearchOutput";

    timeResult->index = index;
    timeResult->message = timeMessage;
    timeResult->outputDiv = "sequentialSearchTime";

     emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_VI, (void (*)(void*))handleSearchResult, result); // Envía el mensaje de búsqueda
    emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_VI, (void (*)(void*))handleSearchResult, timeResult); // Envía el mensaje de tiempo
    
    pthread_exit(NULL);
}


void* printBinarySearch(void* args) {
    SearchArgs* searchArgs = (SearchArgs*)args;
    
    double start_time = emscripten_get_now();
    int index = binarySearch(searchArgs->array, 0, searchArgs->size - 1, searchArgs->target);
    double end_time = emscripten_get_now();
    double elapsed_time = (end_time - start_time) / 1000.0; 



    char* message;
    char* timeMessage;
    if (index != -1) {
        message = (char*)malloc(100 * sizeof(char)); 
        timeMessage = (char*)malloc(100 * sizeof(char));
        sprintf(message, "Encontrado en la posición %d usando BB.", index);
        sprintf(timeMessage, "Tiempo de búsqueda: %.6f segundos", elapsed_time);
    } else {
        message = strdup("No encontrado usando BB");
        timeMessage = strdup("");
    }
    
    SearchResult* result = (SearchResult*)malloc(sizeof(SearchResult));
    SearchResult* timeResult = (SearchResult*)malloc(sizeof(SearchResult));
    result->index = index;
    result->message = message;
    result->outputDiv = "binarySearchOutput";


    timeResult->index = index;
    timeResult->message = timeMessage;
    timeResult->outputDiv = "binarySearchTime";

    emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_VI, (void (*)(void*))handleSearchResult, result); // Envía el mensaje de búsqueda
    emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_VI, (void (*)(void*))handleSearchResult, timeResult); // Envía el mensaje de tiempo
    

    pthread_exit(NULL);
}


EXTERN EMSCRIPTEN_KEEPALIVE void runSearch(int size, int array[], int target) {
    pthread_t searchThreads[2];
    SearchArgs searchArgs = {array, size, target};

    searchThreads[0] = createThread(printSequentialSearch, &searchArgs);
    searchThreads[1] = createThread(printBinarySearch, &searchArgs);

    for (int i = 0; i < 2; i++) {
        pthread_join(searchThreads[i], NULL);
    }
}

int main() {
    printf("Hello, World!\n");
    return 0;
}
