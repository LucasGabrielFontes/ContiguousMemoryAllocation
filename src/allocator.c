#include <stdio.h>      // Para entrada e saída (printf, fgets, etc.)
#include <stdlib.h>     // Para malloc, free, atoi
#include <string.h>     // Para manipulação de strings (strcmp, strcpy, etc.)
#include <unistd.h>     // Para isatty()
#include "../include/memory.h"

// Função principal: interface do terminal com o usuário
int main(int argc, char* argv[]) {
    // Verifica se o argumento de tamanho de memória foi fornecido
    if (argc != 2) {
        printf("Uso: %s <tamanho_memoria>\n", argv[0]);
        return 1;
    }

    // Converte o argumento de tamanho para inteiro
    TAM_MEMORIA = atoi(argv[1]);

    // Inicializa a memória com um único buraco do tamanho total
    inicializar_memoria(TAM_MEMORIA);

    // Buffer para armazenar o comando digitado ou lido
    char comando[100];

    // Mensagem inicial do simulador
    printf("Simulador iniciado. Tamanho: %d bytes\n", TAM_MEMORIA);

    // Loop principal do simulador
    while (1) {
        // Exibe o prompt apenas se estiver rodando no modo interativo (teclado)
        if (isatty(fileno(stdin))) {
            printf("\nallocator> ");
        }

        // Lê o comando do usuário ou do arquivo (stdin)
        if (!fgets(comando, sizeof(comando), stdin)) {
            break; // Encerra o loop se EOF for atingido (fim do arquivo)
        }

        // Remove o caractere de nova linha ('\n') do final do comando, se existir
        comando[strcspn(comando, "\n")] = 0;

        // Processa comando: alocar memória
        if (strncmp(comando, "RQ", 2) == 0) {
            char nome[20], modo;
            int tam;
            sscanf(comando, "RQ %s %d %c", nome, &tam, &modo);
            requisitar_memoria(nome, tam, modo);
        }
        // Processa comando: liberar memória
        else if (strncmp(comando, "RL", 2) == 0) {
            char nome[20];
            sscanf(comando, "RL %s", nome);
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
        // Qualquer outro comando não reconhecido (exceto linha vazia)
        else if (strlen(comando) > 0) {
            printf("Comando inválido.\n");
        }
    }

    return 0;
}
