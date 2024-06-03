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
} SortArgs;

typedef struct {
    int* array;
    int size;
    int target;
} SearchArgs;

typedef struct {
    int index;
    char* message;
    char* output;
    char* timeMessage;
    char* tOutput;
} SearchResult;

typedef struct {
    char* sortedArray;
    int size;
    char* output;
    char* timeMessage;
    char* tOutput;
} SortResult;

pthread_t createThread(void* (*function)(void*), void* args) {
    pthread_t thread;
    if (pthread_create(&thread, NULL, function, args)) {
        fprintf(stderr, "Error al crear el hilo\n");
        exit(1);
    }
    return thread;
}

char* arrayToString(int* array, int size) {
    char* str = (char*)malloc(100 * sizeof(char));
    sprintf(str, "[");
    for (int i = 0; i < size; i++) {
        if (i == size - 1) {
            sprintf(str, "%s%d]", str, array[i]);
        } else {
            sprintf(str, "%s%d, ", str, array[i]);
        }
    }
    return str;
  
}

void handleSearchResult(void* args) {
    SearchResult* result = (SearchResult*)args;
    EM_ASM({
        var divId = UTF8ToString($0);
        var resultMessage = UTF8ToString($1);
        var tOutput = UTF8ToString($2);
        var timeMessage = UTF8ToString($3);

        document.getElementById(divId).innerText = resultMessage;
        document.getElementById(tOutput).innerText = timeMessage;
     
        _free($0);
        _free($1);
        _free($2);
        _free($3);

    }, result->output, result->message, result->tOutput, result->timeMessage);
   
}

void handleSortResult(void* args) {
    SortResult* result = (SortResult*)args;
    EM_ASM({
        var divId = UTF8ToString($0);
        var array = UTF8ToString($1);
        var tOutput = UTF8ToString($2);
        var timeMessage = UTF8ToString($3);

        document.getElementById(divId).innerText = array;
        document.getElementById(tOutput).innerText = timeMessage;

        _free($0);
        _free($1);
        _free($2);
        _free($3);

    }, result->output, result->sortedArray, result->tOutput, result->timeMessage);
}

//Bubble sort
void* bubbleSortThread(void* args) {

    SortArgs* sortArgs = (SortArgs*)args;
    int* copiedArray = (int*)malloc(sizeof(int) * sortArgs->size);
    memcpy(copiedArray, sortArgs->array, sizeof(int) * sortArgs->size);
    double start_time = emscripten_get_now();
    bubbleSort(copiedArray, sortArgs->size);
    double end_time = emscripten_get_now();
    double elapsed_time = (end_time - start_time) / 1000.0;

    size_t bufferSize = (sortArgs->size * 12) + 1; 
    char* sortedArrayStr = (char*)malloc(bufferSize);
    sortedArrayStr[0] = '\0'; // Inicializar la cadena vacía

    for (int i = 0; i < sortArgs->size; ++i) {
        char elementStr[12]; // Tamaño suficiente para un entero
        snprintf(elementStr, sizeof(elementStr), "%d", copiedArray[i]);
        strncat(sortedArrayStr, elementStr, bufferSize - strlen(sortedArrayStr) - 1);
        if (i < sortArgs->size - 1) {
            strncat(sortedArrayStr, ", ", bufferSize - strlen(sortedArrayStr) - 1);
        }
    }

    char* timeMessage = (char*)malloc(100 * sizeof(char));
    sprintf(timeMessage, "Tiempo de ordenamiento: %.6f segundos", elapsed_time);

    SortResult* result = (SortResult*)malloc(sizeof(SortResult));

    result->sortedArray = sortedArrayStr;
    result->size = sortArgs->size;
    result->output = "bubbleSortOutput";
    result->timeMessage = timeMessage;
    result->tOutput = "bubbleSortTime";

    emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_VI, (void (*)(void*))handleSortResult, result); 
    pthread_exit(NULL);

}

//Quick sort
void* quickSortThread(void* args) {
    SortArgs* sortArgs = (SortArgs*)args;
    int* copiedArray = (int*)malloc(sizeof(int) * sortArgs->size);
    memcpy(copiedArray, sortArgs->array, sizeof(int) * sortArgs->size);
    double start_time = emscripten_get_now();
    quickSort(copiedArray, 0, sortArgs->size - 1);
    double end_time = emscripten_get_now();
    double elapsed_time = (end_time - start_time) / 1000.0;

    char* timeMessage = (char*)malloc(100 * sizeof(char));
    sprintf(timeMessage, "Tiempo de ordenamiento: %.6f segundos", elapsed_time);

    size_t bufferSize = (sortArgs->size * 12) + 1; 
    char* sortedArrayStr = (char*)malloc(bufferSize);
    sortedArrayStr[0] = '\0'; // Inicializar la cadena vacía

    for (int i = 0; i < sortArgs->size; ++i) {
        char elementStr[12]; // Tamaño suficiente para un entero
        snprintf(elementStr, sizeof(elementStr), "%d", copiedArray[i]);
        strncat(sortedArrayStr, elementStr, bufferSize - strlen(sortedArrayStr) - 1);
        if (i < sortArgs->size - 1) {
            strncat(sortedArrayStr, ", ", bufferSize - strlen(sortedArrayStr) - 1);
        }
    }


    SortResult* result = (SortResult*)malloc(sizeof(SortResult));

    result->sortedArray = sortedArrayStr;
    result->size = sortArgs->size;
    result->output = "quickSortOutput";
    result->timeMessage = timeMessage;
    result->tOutput = "quickSortTime";

    emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_VI, (void (*)(void*))handleSortResult, result); 
    pthread_exit(NULL);
}

//Insertion sort
void* insertionSortThread(void* args) {
    SortArgs* sortArgs = (SortArgs*)args;
    int* copiedArray = (int*)malloc(sizeof(int) * sortArgs->size);
    memcpy(copiedArray, sortArgs->array, sizeof(int) * sortArgs->size);
    double start_time = emscripten_get_now();
    insertionSort(copiedArray, sortArgs->size);
    double end_time = emscripten_get_now();
    double elapsed_time = (end_time - start_time) / 1000.0;

    char* timeMessage = (char*)malloc(100 * sizeof(char));
    sprintf(timeMessage, "Tiempo de ordenamiento: %.6f segundos", elapsed_time);

    size_t bufferSize = (sortArgs->size * 12) + 1; 
    char* sortedArrayStr = (char*)malloc(bufferSize);
    sortedArrayStr[0] = '\0'; // Inicializar la cadena vacía

    for (int i = 0; i < sortArgs->size; ++i) {
        char elementStr[12]; // Tamaño suficiente para un entero
        snprintf(elementStr, sizeof(elementStr), "%d", copiedArray[i]);
        strncat(sortedArrayStr, elementStr, bufferSize - strlen(sortedArrayStr) - 1);
        if (i < sortArgs->size - 1) {
            strncat(sortedArrayStr, ", ", bufferSize - strlen(sortedArrayStr) - 1);
        }
    }



    SortResult* result = (SortResult*)malloc(sizeof(SortResult));

    result->sortedArray = sortedArrayStr;
    result->size = sortArgs->size;
    result->output = "insertionSortOutput";
    result->timeMessage = timeMessage;
    result->tOutput = "insertionSortTime";

    emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_VI, (void (*)(void*))handleSortResult, result); 
    pthread_exit(NULL);
}


//Secuencial search
void* printSequentialSearch(void* args) {
    
    SearchArgs* searchArgs = (SearchArgs*)args;
    const int target = searchArgs->target;
    double start_time = emscripten_get_now();
    int index = sequentialSearch(searchArgs->array, searchArgs->size, target);
    emscripten_sleep(2000);
    double end_time = emscripten_get_now();
    double elapsed_time = (end_time - start_time) / 1000.0; 
    
    char* message;
    char* timeMessage;
    if (index != -1) {
        message = (char*)malloc(100 * sizeof(char)); 
        timeMessage = (char*)malloc(100 * sizeof(char)); 
        sprintf(message, "%d Encontrado en la posición #%d usando Busqueda Secuencial.",target, index);
        sprintf(timeMessage, "Tiempo de búsqueda: %.6f segundos", elapsed_time);
    } else {
        sprintf(message,"%d No encontrado usando BS",target);
        timeMessage = strdup("");
    }
    
    SearchResult* result = (SearchResult*)malloc(sizeof(SearchResult));

    result->index = index;
    result->message = message;
    result->output = "sequentialSearchOutput";
    result->timeMessage = timeMessage;
    result->tOutput = "sequentialSearchTime";
   
    emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_VI, (void (*)(void*))handleSearchResult, result); 
    pthread_exit(NULL);
}

//Binary search
void* printBinarySearch(void* args) {
    SearchArgs* searchArgs = (SearchArgs*)args;
    const int target = searchArgs->target;
    double start_time = emscripten_get_now();
    int index = binarySearch(searchArgs->array, 0, searchArgs->size - 1, searchArgs->target);
    double end_time = emscripten_get_now();
    double elapsed_time = (end_time - start_time) / 1000.0; 

    char* message;
    char* timeMessage;
    if (index != -1) {
        message = (char*)malloc(100 * sizeof(char)); 
        timeMessage = (char*)malloc(100 * sizeof(char));
        sprintf(message, "%d Encontrado en la posición #%d usando Busqueda Binaria.",target, index);
        sprintf(timeMessage, "Tiempo de búsqueda: %.6f segundos", elapsed_time);
    } else {
        message = (char*)malloc(100 * sizeof(char)); 
        sprintf(message,"%d No encontrado usando BS",target);
        timeMessage = strdup("");
    }
    
    SearchResult* result = (SearchResult*)malloc(sizeof(SearchResult));

    result->index = index;
    result->message = message;
    result->output = "binarySearchOutput";
    result->timeMessage = timeMessage;
    result->tOutput = "binarySearchTime";

    
    emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_VI, (void (*)(void*))handleSearchResult, result); 
    pthread_exit(NULL);
}

//run all search algorithms
EXTERN EMSCRIPTEN_KEEPALIVE void runSearch(int size, int array[], int target) {
    pthread_t searchThreads[2];
     SearchArgs* searchArgs1 = (SearchArgs*)malloc(sizeof(SearchArgs));
    SearchArgs* searchArgs2 = (SearchArgs*)malloc(sizeof(SearchArgs));

    searchArgs1->array = array;
    searchArgs1->size = size;
    searchArgs1->target = target;

    searchArgs2->array = array;
    searchArgs2->size = size;
    searchArgs2->target = target;

    searchThreads[0] = createThread(printSequentialSearch, searchArgs1);
    searchThreads[1] = createThread(printBinarySearch, searchArgs2);

}

//run all sort algorithms
EXTERN EMSCRIPTEN_KEEPALIVE void runSort(int size, int array[]) {
    pthread_t threads[3];
    SortArgs* sortArgs = (SortArgs*)malloc(sizeof(SortArgs));

    sortArgs->array = array;
    sortArgs->size = size;

    threads[0] = createThread(bubbleSortThread, sortArgs);
    threads[1] = createThread(quickSortThread, sortArgs);
    threads[2] = createThread(insertionSortThread, sortArgs);
}

int main() {
    printf("Hello, World!\n");
    return 0;
}