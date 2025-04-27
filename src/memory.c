#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "../include/memory.h"

Segmento* memoria = NULL;
int TAM_MEMORIA = 0;

// Códigos de cores ANSI
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"

// Registra operações em um arquivo de log com formatação variável
void log_operacao(const char* operacao, const char* formato, ...) {
    FILE* log_file = fopen("memory.log", "a");
    if (log_file) {
        time_t now = time(NULL);
        char* timestamp = ctime(&now);
        timestamp[strlen(timestamp) - 1] = '\0'; // Remove \n

        va_list args;
        va_start(args, formato);
        fprintf(log_file, "[%s] %s: ", timestamp, operacao);
        vfprintf(log_file, formato, args);
        fprintf(log_file, "\n");
        va_end(args);

        fclose(log_file);
    }
}

// Cria e retorna um novo segmento de memória
Segmento* criar_segmento(const char* nome, int inicio, int fim) {
    Segmento* novo = (Segmento*)malloc(sizeof(Segmento));
    strcpy(novo->nome, nome);
    novo->inicio = inicio;
    novo->fim = fim;
    novo->prox = NULL;
    return novo;
}

// Inicializa a memória com um único grande buraco
void inicializar_memoria(int tamanho) {
    TAM_MEMORIA = tamanho;
    memoria = criar_segmento("Unused", 0, tamanho - 1);
    log_operacao("Inicialização", "Memória inicializada com tamanho %d bytes", tamanho);
}

// Exibe todos os segmentos da memória com visualização gráfica e métricas
void mostrar_memoria() {
    printf("\n=== Estado da Memória ===\n");
    Segmento* atual = memoria;
    int total_alocado = 0, maior_buraco = 0, num_buracos = 0, total_buracos = 0;

    // Calcula estatísticas
    while (atual != NULL) {
        int tamanho = atual->fim - atual->inicio + 1;
        if (strcmp(atual->nome, "Unused") == 0) {
            num_buracos++;
            total_buracos += tamanho;
            if (tamanho > maior_buraco) maior_buraco = tamanho;
        } else {
            total_alocado += tamanho;
        }
        atual = atual->prox;
    }

    // Exibe tabela de segmentos
    atual = memoria;
    printf("Segmentos:\n");
    while (atual != NULL) {
        int tamanho = atual->fim - atual->inicio + 1;
        printf("%sNome: %-10s | Início: %-6d | Fim: %-6d | Tamanho: %d%s\n",
               strcmp(atual->nome, "Unused") == 0 ? ANSI_COLOR_YELLOW : ANSI_COLOR_GREEN,
               atual->nome, atual->inicio, atual->fim, tamanho, ANSI_COLOR_RESET);
        atual = atual->prox;
    }

    // Exibe estatísticas
    printf("\nEstatísticas:\n");
    printf("Total alocado: %d bytes (%.2f%%)\n", total_alocado, (float)total_alocado / TAM_MEMORIA * 100);
    printf("Total livre: %d bytes (%.2f%%)\n", TAM_MEMORIA - total_alocado, (float)(TAM_MEMORIA - total_alocado) / TAM_MEMORIA * 100);
    printf("Número de buracos: %d\n", num_buracos);
    printf("Maior buraco: %d bytes\n", maior_buraco);
    float fragmentacao = num_buracos > 0 ? (float)total_buracos / TAM_MEMORIA : 0;
    printf("Índice de Fragmentação Externa: %.2f%%\n", fragmentacao * 100);
    printf("========================\n\n");

    log_operacao("Estado", "Exibido estado da memória");
}

// Função principal de alocação: insere um processo segundo a estratégia
void requisitar_memoria(const char* nome, int tamanho, char estrategia) {
    Segmento* atual = memoria;
    Segmento* escolhido = NULL;
    Segmento* anterior = NULL;
    Segmento* anterior_escolhido = NULL;

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
    } else if (estrategia == 'B') {
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
        anterior = anterior_escolhido;
    } else if (estrategia == 'W') {
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

    if (escolhido == NULL) {
        printf("Erro: memória insuficiente para alocar %s\n", nome);
        log_operacao("Erro", "Memória insuficiente para %s", nome);
        return;
    }

    int inicio = escolhido->inicio;
    int fim = inicio + tamanho - 1;

    Segmento* novo = criar_segmento(nome, inicio, fim);

    if (fim < escolhido->fim) {
        escolhido->inicio = fim + 1;
        novo->prox = escolhido;
        if (anterior == NULL)
            memoria = novo;
        else
            anterior->prox = novo;
    } else {
        novo->prox = escolhido->prox;
        if (anterior == NULL)
            memoria = novo;
        else
            anterior->prox = novo;
        free(escolhido);
    }

    log_operacao("Alocação", "Alocado %s, tamanho %d, estratégia %c", nome, tamanho, estrategia);
}

// Libera a memória de um processo, transformando em "Unused" e mesclando buracos
void liberar_memoria(const char* nome) {
    Segmento* atual = memoria;
    int liberado = 0;
    while (atual != NULL) {
        if (strcmp(atual->nome, nome) == 0) {
            strcpy(atual->nome, "Unused");
            liberado = 1;
        }
        atual = atual->prox;
    }

    // Mesclar buracos adjacentes
    atual = memoria;
    while (atual != NULL && atual->prox != NULL) {
        if (strcmp(atual->nome, "Unused") == 0 && strcmp(atual->prox->nome, "Unused") == 0) {
            atual->fim = atual->prox->fim;
            Segmento* temp = atual->prox;
            atual->prox = temp->prox;
            free(temp);
        } else {
            atual = atual->prox;
        }
    }

    if (liberado) {
        log_operacao("Liberação", "Processo %s liberado", nome);
    } else {
        log_operacao("Erro", "Processo %s não encontrado para liberação", nome);
    }
}

// Junta os processos no início da memória e unifica os buracos ao final
void compactar_memoria() {
    Segmento* nova_lista = NULL;
    Segmento* fim_lista = NULL;
    int atual_inicio = 0;

    Segmento* atual = memoria;
    while (atual != NULL) {
        if (strcmp(atual->nome, "Unused") != 0) {
            int tamanho = atual->fim - atual->inicio + 1;
            Segmento* novo = criar_segmento(atual->nome, atual_inicio, atual_inicio + tamanho - 1);
            atual_inicio += tamanho;
            if (nova_lista == NULL)
                nova_lista = novo;
            else
                fim_lista->prox = novo;
            fim_lista = novo;
        }
        atual = atual->prox;
    }

    if (atual_inicio < TAM_MEMORIA) {
        Segmento* buraco = criar_segmento("Unused", atual_inicio, TAM_MEMORIA - 1);
        if (fim_lista == NULL)
            nova_lista = buraco;
        else
            fim_lista->prox = buraco;
    }

    atual = memoria;
    while (atual != NULL) {
        Segmento* temp = atual;
        atual = atual->prox;
        free(temp);
    }

    memoria = nova_lista;
    log_operacao("Compactação", "Memória compactada");
}