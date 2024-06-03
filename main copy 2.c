#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <emscripten/emscripten.h>
#include <emscripten/threading.h>
#include "algoritmos.c"

#define EXTERN


// Structs
typedef struct {
    int* array;
    int size;
    int target;
} SearchArgs;

typedef struct {
    int index;
    char* message;
    char* output;
    char* time;
} SearchResult;

int main () {
    printf("Hello, World22!\n");
    return 0;
}

// Functions Generales
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
        var timeMessage = UTF8ToString($2);
        document.querySelector("#" + divId + " > div").innerText = resultMessage;
        document.querySelector("#" + divId + " > span").innerText = timeMessage;
        _free($1);  
        _free($2);
    }, result->output, result->message, result->time);
    free(result);
      free(result->message);
        free(result->time);
   
}
void* printSequentialSearch(void* args) {
    SearchArgs* searchArgs = (SearchArgs*)args;
    double start_time = emscripten_get_now();
    int index = sequentialSearch(searchArgs->array, searchArgs->size, searchArgs->target);
    double end_time = emscripten_get_now();
    double elapsed_time = (end_time - start_time) / 1000.0; 

    char* message;
    char* timeMessage;
    if (index != -1) {
        message = (char*)malloc(100 * sizeof(char));
        timeMessage = (char*)malloc(100 * sizeof(char));
        sprintf(message, "%d Encontrado en la posición (%d) usando BS.", searchArgs->target, index);
        sprintf(timeMessage, "Tiempo de búsqueda: %.6f segundos", elapsed_time);
    } else {
        message = strdup("No encontrado usando BS");
        timeMessage = strdup("");
    }

    SearchResult* result = (SearchResult*)malloc(sizeof(SearchResult));
    result->index = index;
    result->message = message;
    result->output = "Outputbinary";
    result->time = timeMessage;

    emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_VI, (void (*)(void*))handleSearchResult, result);
    pthread_exit(NULL); 
} 

void* printBinarySearch2(void* args) {
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
        sprintf(message, "%d Encontrado en la posición (%d) usando BB.", searchArgs->target, index);
        sprintf(timeMessage, "Tiempo de búsqueda: %.6f segundos", elapsed_time);
    } else {
        message = strdup("No encontrado usando BB");
        timeMessage = strdup("");
    }

    SearchResult* result = (SearchResult*)malloc(sizeof(SearchResult));
    result->index = index;
    result->message = message;
    result->time = timeMessage;
    result->output = "Outputsequential";

    emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_VI, (void (*)(void*))handleSearchResult, result);
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
        sprintf(message, "%d Encontrado en la posición (%d) usando BB.", searchArgs->target, index);
        sprintf(timeMessage, "Tiempo de búsqueda: %.6f segundos", elapsed_time);
    } else {
        message = strdup("No encontrado usando BB");
        timeMessage = strdup("");
    }

    SearchResult* result = (SearchResult*)malloc(sizeof(SearchResult));
    result->index = index;
    result->message = message;
    result->time = timeMessage;
    result->output = "Outputsequential";

    emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_VI, (void (*)(void*))handleSearchResult, result);
    pthread_exit(NULL);
}


EXTERN EMSCRIPTEN_KEEPALIVE void runSearch(int size, int array[], int target) {
    printf("Running search\n");
    pthread_t searchThreads[3];
    SearchArgs searchArgs = {array, size, target};

    searchThreads[0] = createThread(printSequentialSearch, &searchArgs);
    searchThreads[0] = createThread(printSequentialSearch, &searchArgs);
    searchThreads[1] = createThread(printBinarySearch, &searchArgs);

   
}

