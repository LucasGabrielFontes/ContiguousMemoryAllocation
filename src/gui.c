#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include "../include/memory.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 400
#define BAR_HEIGHT 100

void render_memory(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Fundo branco
    SDL_RenderClear(renderer);

    Segmento* atual = memoria;
    float scale = (float)WINDOW_WIDTH / TAM_MEMORIA;

    while (atual != NULL) {
        int tamanho = atual->fim - atual->inicio + 1;
        int x = atual->inicio * scale;
        int width = tamanho * scale;

        if (strcmp(atual->nome, "Unused") == 0) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Amarelo para buracos
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Verde para processos
        }

        SDL_Rect rect = {x, (WINDOW_HEIGHT - BAR_HEIGHT) / 2, width, BAR_HEIGHT};
        SDL_RenderFillRect(renderer, &rect);

        // Desenha bordas do segmento
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Bordas pretas
        SDL_RenderDrawRect(renderer, &rect);

        atual = atual->prox;
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s <tamanho_memoria>\n", argv[0]);
        return 1;
    }

    int tam_memoria = atoi(argv[1]);
    if (tam_memoria <= 0) {
        printf("Erro: tamanho da memória deve ser positivo.\n");
        return 1;
    }

    inicializar_memoria(tam_memoria);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Simulador de Alocação de Memória",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erro ao criar renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int quit = 0;
    SDL_Event event;
    char comando[100];

    printf("Simulador iniciado com GUI. Tamanho: %d bytes\n", TAM_MEMORIA);
    printf("Comandos: RQ <nome> <tamanho> <F|B|W>, RL <nome>, C, STAT, X\n");

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }

        printf("allocator> ");
        if (!fgets(comando, sizeof(comando), stdin)) {
            break;
        }
        comando[strcspn(comando, "\n")] = 0;

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
        } else if (strncmp(comando, "RL", 2) == 0) {
            char nome[20];
            if (sscanf(comando, "RL %s", nome) != 1) {
                printf("Erro: formato inválido. Use: RL <nome>\n");
                continue;
            }
            liberar_memoria(nome);
        } else if (strncmp(comando, "C", 1) == 0) {
            compactar_memoria();
        } else if (strncmp(comando, "STAT", 4) == 0) {
            mostrar_memoria();
        } else if (strncmp(comando, "X", 1) == 0) {
            break;
        } else if (strlen(comando) > 0) {
            printf("Comando inválido.\n");
        }

        render_memory(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    // Libera a memória
    Segmento* atual = memoria;
    while (atual != NULL) {
        Segmento* temp = atual;
        atual = atual->prox;
        free(temp);
    }

    return 0;
}