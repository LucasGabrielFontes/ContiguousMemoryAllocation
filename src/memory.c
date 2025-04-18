#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/memory.h"

Segmento* memoria = NULL;
int TAM_MEMORIA = 0;

// Cria e retorna um novo segmento de memória
Segmento* criar_segmento(const char* nome, int inicio, int fim) {
    Segmento* novo = (Segmento*)malloc(sizeof(Segmento)); // Aloca memória para o segmento
    strcpy(novo->nome, nome);  // Copia o nome do processo ou "Unused"
    novo->inicio = inicio;     // Define posição inicial
    novo->fim = fim;           // Define posição final
    novo->prox = NULL;         // Inicializa o ponteiro para o próximo como NULL
    return novo;
}

// Inicializa a memória com um único grande buraco (segmento livre)
void inicializar_memoria(int tamanho) {
    memoria = criar_segmento("Unused", 0, tamanho - 1); // Cria um segmento livre com todo o tamanho da memória
}

// Exibe todos os segmentos da memória: nome, início, fim e tamanho
void mostrar_memoria() {
    printf("\n"); // Linha em branco antes da exibição (deixa a saída visualmente separada)
    Segmento* atual = memoria;
    while (atual != NULL) {
        printf("Nome: %-10s | Início: %-6d | Fim: %-6d | Tamanho: %d\n",
               atual->nome, atual->inicio, atual->fim,
               atual->fim - atual->inicio + 1);
        atual = atual->prox;
    }
    printf("\n"); // Linha em branco após exibir (separa da próxima saída)
}

// Função principal de alocação: insere um processo segundo a estratégia
void requisitar_memoria(const char* nome, int tamanho, char estrategia) {
    Segmento* atual = memoria;
    Segmento* escolhido = NULL;          // Melhor buraco encontrado
    Segmento* anterior = NULL;           // Segmento anterior na lista
    Segmento* anterior_escolhido = NULL; // Segmento anterior ao escolhido

    // Estratégia: First Fit
    if (estrategia == 'F') {
        while (atual != NULL) {
            if (strcmp(atual->nome, "Unused") == 0 &&
                atual->fim - atual->inicio + 1 >= tamanho) {
                escolhido = atual;
                break;
            }
            anterior = atual;
            atual = atual->prox;
        }
    }
    // Estratégia: Best Fit
    else if (estrategia == 'B') {
        int menor = TAM_MEMORIA + 1;
        while (atual != NULL) {
            int tam = atual->fim - atual->inicio + 1;
            if (strcmp(atual->nome, "Unused") == 0 &&
                tam >= tamanho && tam < menor) {
                menor = tam;
                escolhido = atual;
                anterior_escolhido = anterior;
            }
            anterior = atual;
            atual = atual->prox;
        }
        anterior = anterior_escolhido; // Atualiza anterior com o do buraco escolhido
    }
    // Estratégia: Worst Fit
    else if (estrategia == 'W') {
        int maior = -1;
        while (atual != NULL) {
            int tam = atual->fim - atual->inicio + 1;
            if (strcmp(atual->nome, "Unused") == 0 &&
                tam >= tamanho && tam > maior) {
                maior = tam;
                escolhido = atual;
                anterior_escolhido = anterior;
            }
            anterior = atual;
            atual = atual->prox;
        }
        anterior = anterior_escolhido;
    }

    // Se nenhum buraco foi encontrado
    if (escolhido == NULL) {
        printf("Erro: memória insuficiente para alocar %s\n", nome);
        return;
    }

    // Calcula início e fim do novo processo
    int inicio = escolhido->inicio;
    int fim = inicio + tamanho - 1;

    Segmento* novo = criar_segmento(nome, inicio, fim); // Cria o segmento do processo

    // Se sobrou espaço no buraco (dividimos o buraco em dois)
    if (fim < escolhido->fim) {
        escolhido->inicio = fim + 1; // Move o início do buraco
        novo->prox = escolhido;

        // Atualiza ponteiros
        if (anterior == NULL)
            memoria = novo;
        else
            anterior->prox = novo;
    }
    // Se o processo usou o buraco inteiro
    else {
        novo->prox = escolhido->prox;

        if (anterior == NULL)
            memoria = novo;
        else
            anterior->prox = novo;

        free(escolhido); // Remove o buraco
    }
}

// Libera a memória de um processo, transformando em "Unused"
void liberar_memoria(const char* nome) {
    Segmento* atual = memoria;
    while (atual != NULL) {
        if (strcmp(atual->nome, nome) == 0) {
            strcpy(atual->nome, "Unused"); // Marca o segmento como buraco
        }
        atual = atual->prox;
    }
}

// Junta os processos no início da memória e unifica os buracos ao final
void compactar_memoria() {
    Segmento* nova_lista = NULL; // Nova lista de segmentos
    Segmento* fim_lista = NULL;  // Ponteiro para o último da nova lista
    int atual_inicio = 0;        // Marca o próximo endereço disponível

    Segmento* atual = memoria;
    while (atual != NULL) {
        if (strcmp(atual->nome, "Unused") != 0) {
            int tamanho = atual->fim - atual->inicio + 1;
            Segmento* novo = criar_segmento(atual->nome, atual_inicio, atual_inicio + tamanho - 1);
            atual_inicio += tamanho;

            // Insere na nova lista
            if (nova_lista == NULL)
                nova_lista = novo;
            else
                fim_lista->prox = novo;
            fim_lista = novo;
        }
        atual = atual->prox;
    }

    // Cria um buraco com o restante da memória
    if (atual_inicio < TAM_MEMORIA) {
        Segmento* buraco = criar_segmento("Unused", atual_inicio, TAM_MEMORIA - 1);
        if (fim_lista == NULL)
            nova_lista = buraco;
        else
            fim_lista->prox = buraco;
    }

    // Libera a lista antiga
    atual = memoria;
    while (atual != NULL) {
        Segmento* temp = atual;
        atual = atual->prox;
        free(temp);
    }

    memoria = nova_lista; // Atualiza para a nova lista compactada
}