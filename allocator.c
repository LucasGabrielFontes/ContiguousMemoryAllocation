#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Segmento {
    char nome[20];       // Nome do processo ou "Unused"
    int inicio;          // Início do segmento
    int fim;             // Fim do segmento
    struct Segmento* prox;
} Segmento;

Segmento* memoria = NULL;
int TAM_MEMORIA = 0;

// Função para criar um novo segmento
Segmento* criar_segmento(const char* nome, int inicio, int fim) {
    Segmento* novo = (Segmento*)malloc(sizeof(Segmento));
    strcpy(novo->nome, nome);
    novo->inicio = inicio;
    novo->fim = fim;
    novo->prox = NULL;
    return novo;
}

// Inicializa a memória como um único bloco livre
void inicializar_memoria(int tamanho) {
    memoria = criar_segmento("Unused", 0, tamanho - 1);
}

// Exibe o status atual da memória
void mostrar_memoria() {
    Segmento* atual = memoria;
    while (atual != NULL) {
        printf("Nome: %-10s | Início: %-6d | Fim: %-6d | Tamanho: %d\n",
               atual->nome, atual->inicio, atual->fim,
               atual->fim - atual->inicio + 1);
        atual = atual->prox;
    }
}

// Insere o processo segundo a estratégia escolhida
void requisitar_memoria(const char* nome, int tamanho, char estrategia) {
    Segmento* atual = memoria;
    Segmento* escolhido = NULL;
    Segmento* anterior = NULL;
    Segmento* anterior_escolhido = NULL;

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
        anterior = anterior_escolhido;
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

    if (escolhido == NULL) {
        printf("Erro: memória insuficiente para alocar %s\n", nome);
        return;
    }

    int inicio = escolhido->inicio;
    int fim = inicio + tamanho - 1;

    Segmento* novo = criar_segmento(nome, inicio, fim);

    // Atualiza buraco remanescente
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
}

// Libera a memória de um processo
void liberar_memoria(const char* nome) {
    Segmento* atual = memoria;
    while (atual != NULL) {
        if (strcmp(atual->nome, nome) == 0) {
            strcpy(atual->nome, "Unused");
        }
        atual = atual->prox;
    }
}

// Compacta a memória unindo os segmentos "Unused" e realocando os processos
void compactar_memoria() {
    Segmento* nova_lista = NULL;
    Segmento* fim_lista = NULL;
    int atual_inicio = 0;

    // Realoca processos
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

    // Cria buraco final
    if (atual_inicio < TAM_MEMORIA) {
        Segmento* buraco = criar_segmento("Unused", atual_inicio, TAM_MEMORIA - 1);
        if (fim_lista == NULL)
            nova_lista = buraco;
        else
            fim_lista->prox = buraco;
    }

    // Libera lista antiga
    atual = memoria;
    while (atual != NULL) {
        Segmento* temp = atual;
        atual = atual->prox;
        free(temp);
    }

    memoria = nova_lista;
}

// Função principal com loop de comandos
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s <tamanho_memoria>\n", argv[0]);
        return 1;
    }

    TAM_MEMORIA = atoi(argv[1]);
    inicializar_memoria(TAM_MEMORIA);

    char comando[100];
    printf("Simulador iniciado. Tamanho: %d bytes\n", TAM_MEMORIA);
    while (1) {
        printf("allocator> ");
        fgets(comando, sizeof(comando), stdin);

        if (strncmp(comando, "RQ", 2) == 0) {
            char nome[20], modo;
            int tam;
            sscanf(comando, "RQ %s %d %c", nome, &tam, &modo);
            requisitar_memoria(nome, tam, modo);
        }
        else if (strncmp(comando, "RL", 2) == 0) {
            char nome[20];
            sscanf(comando, "RL %s", nome);
            liberar_memoria(nome);
        }
        else if (strncmp(comando, "C", 1) == 0) {
            compactar_memoria();
        }
        else if (strncmp(comando, "STAT", 4) == 0) {
            mostrar_memoria();
        }
        else if (strncmp(comando, "X", 1) == 0) {
            break;
        }
        else {
            printf("Comando inválido.\n");
        }
    }

    return 0;
}
