#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/memory.h"

// Função principal: interface do terminal com o usuário
int main(int argc, char* argv[]) {
    // Verifica se o argumento de tamanho de memória foi fornecido
    if (argc != 2) {
        printf("Uso: %s <tamanho_memoria>\n", argv[0]);
        return 1;
    }

    // Converte o argumento de tamanho para inteiro
    int tam_memoria = atoi(argv[1]);
    if (tam_memoria <= 0) {
        printf("Erro: tamanho da memória deve ser positivo.\n");
        return 1;
    }

    // Inicializa a memória com um único buraco do tamanho total
    inicializar_memoria(tam_memoria);

    // Buffer para armazenar o comando
    char comando[100];

    // Mensagem inicial do simulador
    printf("Simulador iniciado. Tamanho: %d bytes\n", TAM_MEMORIA);
    printf("Comandos: RQ <nome> <tamanho> <F|B|W>, RL <nome>, C, STAT, X\n");

    // Loop principal do simulador
    while (1) {
        // Exibe o prompt apenas no modo interativo
        if (isatty(fileno(stdin))) {
            printf("\nallocator> ");
        }

        // Lê o comando
        if (!fgets(comando, sizeof(comando), stdin)) {
            break; // Encerra se EOF for atingido
        }

        // Remove o caractere de nova linha
        comando[strcspn(comando, "\n")] = 0;

        // Processa comando: alocar memória
        if (strncmp(comando, "RQ", 2) == 0) {
            char nome[20], modo;
            int tam;
            if (sscanf(comando, "RQ %s %d %c", nome, &tam, &modo) != 3) {
                printf("Erro: formato inválido. Use: RQ <nome> <tamanho> <F|B|W>\n");
                continue;
            }
            if (tam <= 0) {
                printf("Erro: tamanho deve ser positivo.\n");
                continue;
            }
            if (modo != 'F' && modo != 'B' && modo != 'W') {
                printf("Erro: estratégia deve ser F, B ou W.\n");
                continue;
            }
            if (strlen(nome) >= 20) {
                printf("Erro: nome do processo muito longo.\n");
                continue;
            }
            requisitar_memoria(nome, tam, modo);
        }
        // Processa comando: liberar memória
        else if (strncmp(comando, "RL", 2) == 0) {
            char nome[20];
            if (sscanf(comando, "RL %s", nome) != 1) {
                printf("Erro: formato inválido. Use: RL <nome>\n");
                continue;
            }
            liberar_memoria(nome);
        }
        // Processa comando: compactar memória
        else if (strncmp(comando, "C", 1) == 0) {
            compactar_memoria();
        }
        // Processa comando: mostrar estado da memória
        else if (strncmp(comando, "STAT", 4) == 0) {
            mostrar_memoria();
        }
        // Processa comando: sair do simulador
        else if (strncmp(comando, "X", 1) == 0) {
            break;
        }
        // Qualquer outro comando não reconhecido
        else if (strlen(comando) > 0) {
            printf("Comando inválido.\n");
        }
    }

    // Libera a memória
    Segmento* atual = memoria;
    while (atual != NULL) {
        Segmento* temp = atual;
        atual = atual->prox;
        free(temp);
    }

    return 0;
}