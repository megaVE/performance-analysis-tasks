#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>

#define TAMANHO_PACOTE 188.0 * 8.0  // 188 Bytes
#define TAMANHO_BANDA 1000000000.0  // 1 Gigabit / segundo
#define TEMPO_SERVICO TAMANHO_PACOTE / TAMANHO_BANDA // Tempo de processamento de cada pacote
#define TEMPO_LIGACAO 120.0         // 2 minutos
#define TEMPO_INJECAO 20.0          // 20 segundos
#define MAXIMO_ARVORE 100            // Tamanho máximo suportado pela árvore

// Evento

// 'n': Nova Conexão (indica o momento quando o próximo cliente estabelecerá conexão com o roteador)
// 's': Tempo de Serviço (indica quando o pacote atual será enviado e o roteador ficará novamente disponível)
// 'p': Tempo de Pacote (indica o momento em que um pacote é enviado para a fila do roteador)
// 'c': Tempo de Coleta (intervalo contínuo de 10 segundos para a coleta dos dados e cálculo de Little)

struct evento{
    char tipo_evento;
    double tempo_evento;
    double tempo_limite; // Exclusivo de eventos "Tempo de Pacote"
};

struct evento cria_evento(char tipo, double tempo, double limite){
    struct evento novo_evento;

    novo_evento.tipo_evento = tipo;
    novo_evento.tempo_evento = tempo;
    novo_evento.tempo_limite = limite;

    return novo_evento;
}

// Heap

struct heap{
    struct evento * array;
    int size;
    int capacity;
};

void heapify(struct heap * heap, int index){
    int left = index * 2 + 1;
    int right = index * 2 + 2;
    int min = index;

    if(left >= heap->size || left < 0) left = -1;
    if(right >= heap->size || right < 0) right = -1;

    if(left != -1 && heap->array[left].tempo_evento < heap->array[index].tempo_evento) min = left;
    if(right != -1 && heap->array[right].tempo_evento < heap->array[min].tempo_evento) min = right;

    if(min != index){
        struct evento aux = heap->array[min];
        heap->array[min] = heap->array[index];
        heap->array[index] = aux;

        heapify(heap, min);
    }
}

void organiza_heap(struct heap * heap, int index){
    int parent = (index - 1) / 2;

    if(heap->array[parent].tempo_evento > heap->array[index].tempo_evento){
        struct evento aux = heap->array[parent];
        heap->array[parent] = heap->array[index];
        heap->array[index] = aux;

        organiza_heap(heap, parent);
    }
}

struct heap * cria_heap(int capacity){
    struct heap * newHeap = (struct heap*)malloc(sizeof(struct heap));

    if(newHeap == NULL){
        puts("Erro de alocacao de memoria!");
        return NULL;
    }

    newHeap->size = 0;
    newHeap->capacity = capacity;
    newHeap->array = (struct evento *)malloc(capacity * sizeof(struct evento));

    if(newHeap->array == NULL){
        puts("Erro de alocacao de memoria!");
        return NULL;
    }

    return newHeap;
}

void insere_heap(struct heap * heap, struct evento novo_evento){
    if(heap->size >= heap->capacity) return;
    
    heap->array[heap->size] = novo_evento;
    organiza_heap(heap, heap->size);
    heap->size++;
}

struct evento extrai_heap(struct heap * heap){
    if(heap->size == 0){
        puts("A heap esta vazia!");
        struct evento evento_vazio;
        return evento_vazio;
    }

    struct evento evento_extraido = heap->array[0];
    heap->array[0] = heap->array[heap->size - 1];
    heap->size--;

    heapify(heap, 0);
    return evento_extraido;
}

void imprime_heap(struct heap * heap){
    for(int i = 0; i < heap->size; i++)
        printf("[%c]: %lF,  %lF\n", heap->array[i].tipo_evento, heap->array[i].tempo_evento, heap->array[i].tempo_limite);
}

// Entrada

typedef struct {
    double media_chegada;
    double media_servico;
    double tempo_simulacao;
} parametros;

void le_parametros(parametros * params){
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

// Analise

typedef struct {
    unsigned long int no_eventos;
    double tempo_anterior;
    double soma_areas;
} little;

void inicia_little(little * l){
    l->no_eventos = 0;
    l->tempo_anterior = 0.0;
    l->soma_areas = 0.0;
}

// Simulador

double uniforme() {
	double u = rand() / ((double) RAND_MAX + 1); //limitando entre (0,1]

	return (1.0 - u);
}

void inicia_arvore(struct heap ** arvore){ // Não funciona, fazer no próprio código
    // arvore = cria_heap(MAXIMO_ARVORE);
    // Primeira Conexão
    // Primeira Coleta
}

int main(){
    // Área de Teste
    puts("Inicio do teste");

    struct heap * arvore_de_eventos = cria_heap(MAXIMO_ARVORE);
    printf("Heap Teste: Size = %d, Capacity = %d\n", arvore_de_eventos->size, arvore_de_eventos->capacity);

    struct evento novo_evento = cria_evento('d', 10.0, 0.0);
    printf("Evento Teste: [%c] %lF - %lF\n", novo_evento.tipo_evento, novo_evento.tempo_evento, novo_evento.tempo_limite);

    insere_heap(arvore_de_eventos, novo_evento);
    printf("Heap Apos Insercao: Size = %d, Capacity = %d\n", arvore_de_eventos->size, arvore_de_eventos->capacity);
    imprime_heap(arvore_de_eventos);

    novo_evento = cria_evento('a', 11.0, 0.0);
    printf("Evento Teste: [%c] %lF - %lF\n", novo_evento.tipo_evento, novo_evento.tempo_evento, novo_evento.tempo_limite);
    insere_heap(arvore_de_eventos, novo_evento);

    novo_evento = cria_evento('b', 9.0, 0.0);
    printf("Evento Teste: [%c] %lF - %lF\n", novo_evento.tipo_evento, novo_evento.tempo_evento, novo_evento.tempo_limite);
    insere_heap(arvore_de_eventos, novo_evento);

    imprime_heap(arvore_de_eventos);

    puts("Extrair");
    extrai_heap(arvore_de_eventos);
    imprime_heap(arvore_de_eventos);

    puts("Extrair");
    extrai_heap(arvore_de_eventos);
    imprime_heap(arvore_de_eventos);

    puts("Extrair");
    extrai_heap(arvore_de_eventos);
    imprime_heap(arvore_de_eventos);

    puts("Fim de teste");
    return 0;
}

/*
    // RNG

    int semente = time(NULL);
    srand(semente);

    // Input

    parametros params;
    le_parametros(&params);


    // Eventos

    struct heap * arvore_de_eventos;
    inicializa_arvore(arvore_de_eventos);

    // Controle de Simulação
    
    double tempo_decorrido = 0.0;
    // [RECALCULAR] double tempo_chegada = (-1.0/params.media_chegada) * log(uniforme());
    // [REFATORAR] double tempo_saida = DBL_MAX;
    // [REFATORAR] double tempo_analise = DBL_MAX;
    unsigned long int fila = 0;
    unsigned long int max_fila = 0;

    // Medidas de Interesse

    double soma_ocupacao = 0.0;

    // Little

    little e_n;
    little e_w_chegada;
    little e_w_saida;
    inicia_little(&e_n);
    inicia_little(&e_w_chegada);
    inicia_little(&e_w_saida);

   // Novas Variáveis

   struct evento evento_atual;

    //min_heap --> tempo_pacote_conexoes_ativas
    //eventos: nova_conexao, conexao_ativa_pacote(heap), tempo_servico, coleta_dados
    // NOVA CONEXÃO: entra um novo cliente na banda
    // CONEXÃO ATIVA: tempo em que outro pacote chega e entra na fila
    // TEMPO SERVIÇO: quando o roteador ficará disponível novamente
    // COLETA DADOS: coleta das informações de little
    //min(min(min(nova_coxeao, heap.conexao_ativa_pacote),tempo_servico),coleta_dados)
    
    while(tempo_decorrido < params.tempo_simulacao){
       
        evento_atual = extrai_heap(arvore_de_eventos);
        tempo_decorrido = evento_atual.tempo_evento;
        //printf("%lF\n", tempo_decorrido);

        switch(evento_atual.tipo_evento){
            case 'n': // Nova Conexão
                // Insere nova conexão na heap
                break;
            
            case 's': // Tempo de Serviço
                // Reduz a fila (caso exista)
                // Insere o próximo momento de disponibilidade na heap
                break;
            
            case 'p': // Tempo de Pacote
                // Aumenta a fila
                // Insere o próximo pacote da conexão na heap (caso a conexão persista)
                break;

            case 'c': // Tempo de Coleta
                // Completa os calculos de little
                // Imprime os resultados
                // Insere o novo valor de calculo na heap
                break;

            default:
                puts("Evento invalido!");
                return 1;
        }



        // if(tempo_decorrido == tempo_analise){
        //     // analise
        //     e_n.soma_areas += e_n.no_eventos * (tempo_decorrido - e_n.tempo_anterior);
        //     e_n.tempo_anterior = tempo_decorrido;

        //     e_w_chegada.soma_areas += e_w_chegada.no_eventos * (tempo_decorrido - e_w_chegada.tempo_anterior);
        //     e_w_chegada.tempo_anterior = tempo_decorrido;
            
        //     e_w_saida.soma_areas += e_w_saida.no_eventos * (tempo_decorrido - e_w_saida.tempo_anterior);
        //     e_w_saida.tempo_anterior = tempo_decorrido;

        //     double e_n_calculo = e_n.soma_areas / tempo_decorrido;
        //     double e_w_calculo = (e_w_chegada.soma_areas - e_w_saida.soma_areas) / e_w_chegada.no_eventos;
        //     double lambda = e_w_chegada.no_eventos / tempo_decorrido;

        //     // printf("%.0lF, %.20lF\n", tempo_analise, e_n_calculo - lambda * e_w_calculo);
        //     tempo_analise += 10.0;
        // } else if(tempo_decorrido == tempo_chegada){
        //     //chegada
        //     //a cabeca da fila eh quem esta em atendimento
        //     if(!fila){
        //         double tempo_servico =
        //             (-1.0/params.media_servico)
        //             * log(uniforme());
                
        //         tempo_saida = tempo_decorrido
        //             + tempo_servico;

        //         soma_ocupacao += tempo_servico;
        //     }
        //     fila++;
        //     max_fila = fila > max_fila?
        //         fila:max_fila;
            
        //     tempo_chegada = tempo_decorrido + 
        //     (-1.0/params.media_chegada) * log(uniforme());

        //     //calculo little -- E[N]
        //     e_n.soma_areas += 
        //         (tempo_decorrido - e_n.tempo_anterior)
        //         * e_n.no_eventos;
        //     e_n.no_eventos++;
        //     e_n.tempo_anterior = tempo_decorrido;
            
        //     //calculo little -- E[W] - chegada
        //     e_w_chegada.soma_areas +=
        //         (tempo_decorrido - e_w_chegada.tempo_anterior)
        //         * e_w_chegada.no_eventos;
        //     e_w_chegada.no_eventos++;
        //     e_w_chegada.tempo_anterior = tempo_decorrido;
        // } else if(tempo_decorrido == tempo_saida){
        //     //saida
        //     fila--;
        //     if(fila){
        //         double tempo_servico =
        //             (-1.0/params.media_servico)
        //             * log(uniforme());
                
        //         tempo_saida = tempo_decorrido
        //             + tempo_servico;

        //         soma_ocupacao += tempo_servico;
        //     }else{
        //         tempo_saida = DBL_MAX;
        //     }

        //     //calculo little -- E[N]
        //     e_n.soma_areas += 
        //         (tempo_decorrido - e_n.tempo_anterior)
        //         * e_n.no_eventos;
        //     e_n.no_eventos--;
        //     e_n.tempo_anterior = tempo_decorrido;

        //     //calculo little -- E[W] - saida
        //     e_w_saida.soma_areas +=
        //         (tempo_decorrido - e_w_saida.tempo_anterior)
        //         * e_w_saida.no_eventos;
        //     e_w_saida.no_eventos++;
        //     e_w_saida.tempo_anterior = tempo_decorrido;
        // } else{
        //     puts("Evento invalido!\n");
        //     return(1);
        // }
    }
    // e_w_chegada.soma_areas +=
    //     (tempo_decorrido - e_w_chegada.tempo_anterior)
    //     * e_w_chegada.no_eventos;

    // e_w_saida.soma_areas +=
    //     (tempo_decorrido - e_w_saida.tempo_anterior)
    //     * e_w_saida.no_eventos;



    // double e_n_calculo = e_n.soma_areas / tempo_decorrido;
    // double e_w_calculo = (e_w_chegada.soma_areas
    //     - e_w_saida.soma_areas)
    //     / e_w_chegada.no_eventos;
    // double lambda = e_w_chegada.no_eventos / tempo_decorrido;

    // printf("ocupacao: %lF\n", (soma_ocupacao/tempo_decorrido));
    // // printf("tamanho maximo da fila: %ld\n", max_fila);

    // printf("E[N]: %lF\n", e_n_calculo);    
    // printf("E[W]: %lF\n", e_w_calculo);
    // printf("Erro de Little: %.20lF\n", e_n_calculo - lambda * e_w_calculo);

    return 0;
}
*/