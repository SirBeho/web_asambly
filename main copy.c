#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <emscripten/emscripten.h>
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
    char* algorithm;
    int* sortedArray;
    int size;
    char* outputDiv;
} Result;

typedef struct {
    int index;
    char* message;
} SearchResult;

void printArray(int arr[], int size, char* from) {
    printf("\n--> Array from %s: ", from);
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void processResult(Result* result) {
    char resultStr[1024];
    int offset = snprintf(resultStr, sizeof(resultStr), "%s Result: ", result->algorithm);
    for (int i = 0; i < result->size; i++) {
        offset += snprintf(resultStr + offset, sizeof(resultStr) - offset, "%d ", result->sortedArray[i]);
    }
    EM_ASM({
        var divId = UTF8ToString($0);
        var result = UTF8ToString($1);
        document.getElementById(divId).firstChild.innerText = result;
    }, result->outputDiv, resultStr);

    free(result->sortedArray);
    free(result);
}

/*Crear Hilo*/
pthread_t createThread(void* (*function)(void*), void* args) {
    pthread_t thread;
    if (pthread_create(&thread, NULL, function, args)) {
        fprintf(stderr, "Error creating thread\n");
        exit(1);
    }
    return thread;
}

int main() {
    printf("Waiting for inputs...\n\n");
    return 0;
}

/*Ordenamiento de burbuja*/
void* bubbleSortThread(void* args) {
    SortArgs* sortArgs = (SortArgs*)args;
    int* copiedArray = (int*)malloc(sizeof(int) * sortArgs->size);
    memcpy(copiedArray, sortArgs->array, sizeof(int) * sortArgs->size);
    bubbleSort(copiedArray, sortArgs->size);
    Result* result = (Result*)malloc(sizeof(Result));
    result->algorithm = "Bubble Sort";
    result->sortedArray = copiedArray;
    result->size = sortArgs->size;
    pthread_exit((void*)result);
}

/*Ordenamiento rápido*/
void* quickSortThread(void* args) {
    SortArgs* sortArgs = (SortArgs*)args;
    int* copiedArray = (int*)malloc(sizeof(int) * sortArgs->size);
    memcpy(copiedArray, sortArgs->array, sizeof(int) * sortArgs->size);
    quickSort(copiedArray, 0, sortArgs->size - 1);
    Result* result = (Result*)malloc(sizeof(Result));
    result->algorithm = "Quick Sort";
    result->sortedArray = copiedArray;
    result->size = sortArgs->size;
    pthread_exit((void*)result);
}

/*Ordenamiento por inserción*/
void* insertionSortThread(void* args) {
    SortArgs* sortArgs = (SortArgs*)args;
    int* copiedArray = (int*)malloc(sizeof(int) * sortArgs->size);
    memcpy(copiedArray, sortArgs->array, sizeof(int) * sortArgs->size);
    insertionSort(copiedArray, sortArgs->size);
    Result* result = (Result*)malloc(sizeof(Result));
    result->algorithm = "Insertion Sort";
    result->sortedArray = copiedArray;
    result->size = sortArgs->size;
    pthread_exit((void*)result);
}

/*Búsqueda secuencial*/
void* printSequentialSearch(void* args) {
    SearchArgs* searchArgs = (SearchArgs*)args;
    int index = sequentialSearch(searchArgs->array, searchArgs->size, searchArgs->target);
    SearchResult* result = (SearchResult*)malloc(sizeof(SearchResult));
    result->index = index;
    if (index != -1) {
        result->message = "Element found using Sequential Search.";
    } else {
        result->message = "Element not found using Sequential Search.";
    }
    pthread_exit((void*)result);
}

/*Búsqueda binaria*/
void* printBinarySearch(void* args) {
    SearchArgs* searchArgs = (SearchArgs*)args;
    int index = binarySearch(searchArgs->array, 0, searchArgs->size - 1, searchArgs->target);
    SearchResult* result = (SearchResult*)malloc(sizeof(SearchResult));
    result->index = index;
    if (index != -1) {
        result->message = "Element found using Binary Search.";
    } else {
        result->message = "Element not found using Binary Search.";
    }
    pthread_exit((void*)result);
}

EXTERN EMSCRIPTEN_KEEPALIVE void runSort(int size, int array[]) {
    pthread_t threads[3];
    SortArgs sortArgs = {array, size};

    threads[0] = createThread(bubbleSortThread, &sortArgs);
    threads[1] = createThread(quickSortThread, &sortArgs);
    threads[2] = createThread(insertionSortThread, &sortArgs);

    for (int i = 0; i < 3; i++) {
        void* result;
        pthread_join(threads[i], &result);
        processResult((Result*)result);
    }
}

EXTERN EMSCRIPTEN_KEEPALIVE void runSearch(int size, int array[], int target) {
    pthread_t searchThreads[2];
    SearchArgs searchArgs = {array, size, target};

    searchThreads[0] = createThread(printSequentialSearch, &searchArgs);
    searchThreads[1] = createThread(printBinarySearch, &searchArgs);

    for (int i = 0; i < 2; i++) {
        void* result;
        pthread_join(searchThreads[i], &result);
        SearchResult* searchResult = (SearchResult*)result;
        printf("%s\n", searchResult->message);
        free(searchResult);
    }
}

