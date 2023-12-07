#include <stdio.h>
#include <stdlib.h>

#define MAX_CAPACITY 10

struct heap{
    int *array;
    int size;
    int capacity;
};

void heapify(struct heap * heap, int index){
    int left = index * 2 + 1;
    int right = index * 2 + 2;
    int min = index;

    if(left >= heap->size || left < 0) left = -1;
    if(right >= heap->size || right < 0) right = -1;

    if(left != -1 && heap->array[left] < heap->array[index]) min = left;
    if(right != -1 && heap->array[right] < heap->array[min]) min = right;

    if(min != index){
        int aux = heap->array[min];
        heap->array[min] = heap->array[index];
        heap->array[index] = aux;

        heapify(heap, min);
    }
}

struct heap * createHeap(int capacity, int * numbers){
    struct heap * newHeap = (struct heap*)malloc(sizeof(struct heap));

    if(newHeap == NULL){
        puts("Erro de alocacao de memoria!");
        return NULL;
    }

    newHeap->array = (int*)malloc(capacity * sizeof(int));
    newHeap->capacity = capacity;

    if(newHeap->array == NULL){
        puts("Erro de alocacao de memoria!");
        return NULL;
    }

    for(int i = 0; i < capacity; i++) newHeap->array[i] = numbers[i];

    newHeap->size = capacity;

    for(int i = (newHeap->size - 2) / 2; i >= 0; i--) heapify(newHeap, i);

    return newHeap;
}

void organizeHeap(struct heap * heap, int index){
    int parent = (index - 1) / 2;

    if(heap->array[parent] > heap->array[index]){
        int aux = heap->array[parent];
        heap->array[parent] = heap->array[index];
        heap->array[index] = aux;

        organizeHeap(heap, parent);
    }
}

void insertHeap(struct heap * heap, int data){
    if(heap->size < heap->capacity){
        heap->array[heap->size] = data;
        organizeHeap(heap, heap->size);
        heap->size++;
    }
}

int extractHeap(struct heap * heap){
    if(heap->size == 0){
        puts("A heap esta vazia!");
        return -1;
    }

    int deleteItem = heap->array[0];
    heap->array[0] = heap->array[heap->size - 1];
    heap->size--;

    heapify(heap, 0);
    return deleteItem;
}

void printHeap(struct heap * heap){
    for(int i = 0; i < heap->size; i++)
        printf("%d, ", heap->array[i]);
    putchar('\n');
}

int main(){
    int arr[9] = { 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    struct heap * tree = createHeap(9, arr);

    printHeap(tree);
    while(tree->size > 0){
        int extract = extractHeap(tree);
        printf("Extracted: %d\n", extract);
        printHeap(tree);
    }

    return 0;
}