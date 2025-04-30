#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/memory.h"

int main(int argc, char* argv[]) {
    // Verifica se foi fornecido exatamente um argumento (além do nome do programa).
    // argv[0] é o nome do programa, argv[1] deve ser o tamanho da memória.
    if (argc != 2) {
        // Exibe a mensagem de uso correta e encerra o programa com código de erro 1.
        printf("Uso: %s <tamanho_memoria>\n", argv[0]);
        return 1;
    }

    // Converte o argumento de tamanho (string em argv[1]) para um inteiro.
    int tam_memoria = atoi(argv[1]);
    // Verifica se o tamanho é menor ou igual a zero.
    if (tam_memoria <= 0) {
        // Exibe mensagem de erro e encerra o programa com código de erro 1.
        printf("Erro: tamanho da memória deve ser positivo.\n");
        return 1;
    }

    // Inicializa a memória com um único buraco (Unused) do tamanho especificado.
    inicializar_memoria(tam_memoria);

    // Declara um buffer para armazenar os comandos digitados pelo usuário.
    // Tamanho máximo de 100 caracteres.
    char comando[100];

    // Exibe mensagem inicial do simulador, informando o tamanho da memória.
    printf("Simulador iniciado. Tamanho: %d bytes\n", TAM_MEMORIA);
    // Lista os comandos válidos para o usuário.
    printf("Comandos: RQ <nome> <tamanho> <F|B|W>, RL <nome>, C, STAT, X\n");

    // Inicia o loop principal do simulador, que processa comandos até ser encerrado.
    while (1) {
        // Verifica se a entrada padrão (stdin) é um terminal interativo.
        // isatty(fileno(stdin)) retorna verdadeiro se o programa está rodando em um terminal.
        if (isatty(fileno(stdin))) {
            // Exibe o prompt "allocator> " para indicar que o programa espera um comando.
            printf("\nallocator> ");
        }

        // Lê uma linha de entrada do usuário e armazena no buffer 'comando'.
        // fgets lê até 100 caracteres ou até encontrar uma nova linha.
        if (!fgets(comando, sizeof(comando), stdin)) {
            // Se fgets retorna NULL (ex.: EOF via Ctrl+D), encerra o loop.
            break;
        }

        // Remove o caractere de nova linha (\n) do final do comando.
        // strcspn retorna o índice do primeiro \n, que é substituído por \0.
        comando[strcspn(comando, "\n")] = 0;

        // Processa o comando de alocação de memória (RQ).
        if (strncmp(comando, "RQ", 2) == 0) {
            // Declara variáveis para armazenar o nome do processo, tamanho e estratégia.
            char nome[20], modo;
            int tam;
            // Extrai o nome, tamanho e modo do comando usando sscanf.
            // Espera o formato "RQ <nome> <tamanho> <F|B|W>".
            if (sscanf(comando, "RQ %s %d %c", nome, &tam, &modo) != 3) {
                // Se não foram lidos exatamente 3 valores, exibe erro e continua o loop.
                printf("Erro: formato inválido. Use: RQ <nome> <tamanho> <F|B|W>\n");
                continue;
            }
            // Verifica se o tamanho solicitado é menor ou igual a zero.
            if (tam <= 0) {
                // Exibe erro e continua o loop.
                printf("Erro: tamanho deve ser positivo.\n");
                continue;
            }
            // Verifica se a estratégia (modo) é válida (F, B ou W).
            if (modo != 'F' && modo != 'B' && modo != 'W') {
                // Exibe erro e continua o loop.
                printf("Erro: estratégia deve ser F, B ou W.\n");
                continue;
            }
            // Verifica se o nome do processo é muito longo (20 caracteres ou mais).
            if (strlen(nome) >= 20) {
                // Exibe erro e continua o loop.
                printf("Erro: nome do processo muito longo.\n");
                continue;
            }
            // Chama a função para alocar memória com os parâmetros fornecidos.
            requisitar_memoria(nome, tam, modo);
        }
        // Processa o comando de liberação de memória (RL).
        else if (strncmp(comando, "RL", 2) == 0) {
            // Declara variável para armazenar o nome do processo.
            char nome[20];
            // Extrai o nome do comando usando sscanf (formato "RL <nome>").
            if (sscanf(comando, "RL %s", nome) != 1) {
                // Se não foi lido exatamente 1 valor, exibe erro e continua o loop.
                printf("Erro: formato inválido. Use: RL <nome>\n");
                continue;
            }
            // Chama a função para liberar a memória do processo especificado.
            liberar_memoria(nome);
        }
        // Processa o comando de compactação de memória (C).
        else if (strncmp(comando, "C", 1) == 0) {
            // Chama a função para compactar a memória, movendo processos para o início.
            compactar_memoria();
        }
        // Processa o comando para mostrar o estado da memória (STAT).
        else if (strncmp(comando, "STAT", 4) == 0) {
            // Chama a função para exibir a tabela de segmentos e estatísticas da memória.
            mostrar_memoria();
        }
        // Processa o comando para sair do simulador (X).
        else if (strncmp(comando, "X", 1) == 0) {
            // Encerra o loop principal, saindo do programa.
            break;
        }
        // Processa comandos inválidos (não vazios).
        else if (strlen(comando) > 0) {
            // Exibe mensagem de erro para comandos desconhecidos e continua o loop.
            printf("Comando inválido.\n");
        }
        // Comandos vazios (apenas Enter) são ignorados, continuando o loop.
    }

    // Libera todos os segmentos de memória antes de encerrar o programa.
    Segmento* atual = memoria;
    while (atual != NULL) {
        // Armazena o segmento atual para liberar.
        Segmento* temp = atual;
        // Avança para o próximo segmento.
        atual = atual->prox;
        // Libera a memória do segmento atual.
        free(temp);
    }

    // Retorna 0, indicando que o programa terminou com sucesso.
    return 0;
}