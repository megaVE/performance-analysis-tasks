#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>

#define TAMANHO_PACOTE 188.0 * 8.0  // 188 Bytes
#define TAMANHO_BANDA 1000000000.0  // 1 Gigabit / segundo
#define TEMPO_LIGACAO 120.0         // 2 minutos
#define TEMPO_INJECAO 20.0          // 20 segundos
#define MAXIMO_FILA 1000            // Tamanho máximo da fila

/* Heap */

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

/* ... */

typedef struct { // Parâmetros de entrada
    double media_chegada;
    double media_servico;
    double tempo_simulacao;
} parametros;

typedef struct { // Valores de análise
    unsigned long int no_eventos;
    double tempo_anterior;
    double soma_areas;
} little;

double min(double a, double b){ return (a < b) ? a : b; } // Retorna o menor valor

void inicia_little(little * l){ // Iniicializa os valores de Little
    l->no_eventos = 0;
    l->tempo_anterior = 0.0;
    l->soma_areas = 0.0;
}

void le_parametros(parametros * params){ // Realiza a leitura dos parâmetros de entrada
    printf("Informe o tempo medio entre clientes (s): ");
    scanf("%lF", &params->media_chegada);
    params->media_chegada = 1.0/params->media_chegada;

    // printf("Informe o tempo medio de servico (s): ");
    // scanf("%lF", &params->media_servico);
    // params->media_servico = 1.0/params->media_servico;

    params->media_servico = TEMPO_LIGACAO;

    printf("Informe o tempo a ser simulado (s): ");
    scanf("%lF", &params->tempo_simulacao);
}

double uniforme() {
	double u = rand() / ((double) RAND_MAX + 1); //limitando entre (0,1]

	return (1.0 - u);
}

int main(){
    int semente = time(NULL);
    srand(semente);

    //le valores parametrizados
    parametros params;
    le_parametros(&params);

    //variaveis de controle da simulacao
    double tempo_decorrido = 0.0;
    double tempo_chegada = (-1.0/params.media_chegada) * log(uniforme());
    double tempo_saida = DBL_MAX;
    double tempo_analise = DBL_MAX;
    // tempo_analise = DBL_MAX;
    unsigned long int fila = 0;
    unsigned long int max_fila = 0;

    //variaveis de medidas de interesse
    double soma_ocupacao = 0.0;

    // Little
    little e_n;
    little e_w_chegada;
    little e_w_saida;
    inicia_little(&e_n);
    inicia_little(&e_w_chegada);
    inicia_little(&e_w_saida);

    while(tempo_decorrido < params.tempo_simulacao){
        tempo_decorrido = min(tempo_chegada, min(tempo_saida, tempo_analise));
        //printf("%lF\n", tempo_decorrido);

        if(tempo_decorrido == tempo_analise){
            // analise
            e_n.soma_areas += e_n.no_eventos * (tempo_decorrido - e_n.tempo_anterior);
            e_n.tempo_anterior = tempo_decorrido;

            e_w_chegada.soma_areas += e_w_chegada.no_eventos * (tempo_decorrido - e_w_chegada.tempo_anterior);
            e_w_chegada.tempo_anterior = tempo_decorrido;
            
            e_w_saida.soma_areas += e_w_saida.no_eventos * (tempo_decorrido - e_w_saida.tempo_anterior);
            e_w_saida.tempo_anterior = tempo_decorrido;

            double e_n_calculo = e_n.soma_areas / tempo_decorrido;
            double e_w_calculo = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
            double lambda = e_w_chegada.no_eventos / tempo_decorrido;

            // printf("%.0lF, %.20lF\n", tempo_analise, e_n_calculo - lambda * e_w_calculo);
            tempo_analise += 10.0;
        } else if(tempo_decorrido == tempo_chegada){
            //chegada
            //a cabeca da fila eh quem esta em atendimento
            if(!fila){
                double tempo_servico =
                    (-1.0/params.media_servico)
                    * log(uniforme());
                
                tempo_saida = tempo_decorrido
                    + tempo_servico;

                soma_ocupacao += tempo_servico;
            }
            fila++;
            max_fila = fila > max_fila?
                fila:max_fila;
            
            tempo_chegada = tempo_decorrido + 
            (-1.0/params.media_chegada) * log(uniforme());

            //calculo little -- E[N]
            e_n.soma_areas += 
                (tempo_decorrido - e_n.tempo_anterior)
                * e_n.no_eventos;
            e_n.no_eventos++;
            e_n.tempo_anterior = tempo_decorrido;
            
            //calculo little -- E[W] - chegada
            e_w_chegada.soma_areas +=
                (tempo_decorrido - e_w_chegada.tempo_anterior)
                * e_w_chegada.no_eventos;
            e_w_chegada.no_eventos++;
            e_w_chegada.tempo_anterior = tempo_decorrido;
        } else if(tempo_decorrido == tempo_saida){
            //saida
            fila--;
            if(fila){
                double tempo_servico =
                    (-1.0/params.media_servico)
                    * log(uniforme());
                
                tempo_saida = tempo_decorrido
                    + tempo_servico;

                soma_ocupacao += tempo_servico;
            }else{
                tempo_saida = DBL_MAX;
            }

            //calculo little -- E[N]
            e_n.soma_areas += 
                (tempo_decorrido - e_n.tempo_anterior)
                * e_n.no_eventos;
            e_n.no_eventos--;
            e_n.tempo_anterior = tempo_decorrido;

            //calculo little -- E[W] - saida
            e_w_saida.soma_areas +=
                (tempo_decorrido - e_w_saida.tempo_anterior)
                * e_w_saida.no_eventos;
            e_w_saida.no_eventos++;
            e_w_saida.tempo_anterior = tempo_decorrido;
        } else{
            puts("Evento invalido!\n");
            return(1);
        }
    }
    e_w_chegada.soma_areas +=
        (tempo_decorrido - e_w_chegada.tempo_anterior)
        * e_w_chegada.no_eventos;

    e_w_saida.soma_areas +=
        (tempo_decorrido - e_w_saida.tempo_anterior)
        * e_w_saida.no_eventos;



    double e_n_calculo = e_n.soma_areas / tempo_decorrido;
    double e_w_calculo = (e_w_chegada.soma_areas
        - e_w_saida.soma_areas)
        / e_w_chegada.no_eventos;
    double lambda = e_w_chegada.no_eventos / tempo_decorrido;

    printf("ocupacao: %lF\n", (soma_ocupacao/tempo_decorrido));
    // printf("tamanho maximo da fila: %ld\n", max_fila);

    printf("E[N]: %lF\n", e_n_calculo);    
    printf("E[W]: %lF\n", e_w_calculo);
    printf("Erro de Little: %.20lF\n", e_n_calculo - lambda * e_w_calculo);

    return 0;
}
