#ifndef MEMORY_H
#define MEMORY_H

// Estrutura que representa um segmento de memória
typedef struct Segmento {
    char nome[20];       // Nome do processo (ex: "P1") ou "Unused" se for buraco
    int inicio;          // Posição inicial na memória
    int fim;             // Posição final na memória
    struct Segmento* prox; // Ponteiro para o próximo segmento da lista
} Segmento;

// Ponteiro para o início da lista de segmentos de memória
extern Segmento* memoria;

// Tamanho total da memória
extern int TAM_MEMORIA;

// Funções públicas
Segmento* criar_segmento(const char* nome, int inicio, int fim);
void inicializar_memoria(int tamanho);
void mostrar_memoria();
void requisitar_memoria(const char* nome, int tamanho, char estrategia);
void liberar_memoria(const char* nome);
void compactar_memoria();
void log_operacao(const char* operacao, const char* formato, ...);

#endif