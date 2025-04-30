#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include "../include/memory.h"

// Define dimensões da janela e barra de memória.
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 400
#define BAR_HEIGHT 100

// Renderiza a memória como barra gráfica.
void render_memory(SDL_Renderer* renderer) {
    // Define fundo branco e limpa a tela.
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Percorre segmentos da memória.
    Segmento* atual = memoria;
    // Calcula escala para mapear memória na janela.
    float scale = (float)WINDOW_WIDTH / TAM_MEMORIA;

    while (atual != NULL) {
        // Calcula tamanho, posição x e largura do segmento.
        int tamanho = atual->fim - atual->inicio + 1;
        int x = atual->inicio * scale;
        int width = tamanho * scale;

        // Define cor: amarelo para buracos, verde para processos.
        if (strcmp(atual->nome, "Unused") == 0) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        }

        // Desenha retângulo do segmento.
        SDL_Rect rect = {x, (WINDOW_HEIGHT - BAR_HEIGHT) / 2, width, BAR_HEIGHT};
        SDL_RenderFillRect(renderer, &rect);

        // Desenha bordas pretas.
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);

        // Avança para o próximo segmento.
        atual = atual->prox;
    }

    // Atualiza a janela.
    SDL_RenderPresent(renderer);
}

// Função principal: interface gráfica do simulador.
int main(int argc, char* argv[]) {
    // Verifica se há argumento de tamanho da memória.
    if (argc != 2) {
        printf("Uso: %s <tamanho_memoria>\n", argv[0]);
        return 1;
    }

    // Converte argumento para inteiro e valida.
    int tam_memoria = atoi(argv[1]);
    if (tam_memoria <= 0) {
        printf("Erro: tamanho da memória deve ser positivo.\n");
        return 1;
    }

    // Inicializa memória.
    inicializar_memoria(tam_memoria);

    // Inicializa SDL para vídeo.
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Cria janela centrada.
    SDL_Window* window = SDL_CreateWindow(
        "Simulador de Alocação de Memória",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Cria renderizador acelerado.
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erro ao criar renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Inicializa flag de saída, eventos e buffer de comandos.
    int quit = 0;
    SDL_Event event;
    char comando[100];

    // Exibe mensagem inicial e comandos.
    printf("Simulador iniciado com GUI. Tamanho: %d bytes\n", TAM_MEMORIA);
    printf("Comandos: RQ <nome> <tamanho> <F|B|W>, RL <nome>, C, STAT, X\n");

    // Loop principal.
    while (!quit) {
        // Processa eventos SDL.
        while (SDL_PollEvent(&event)) {
            // Sai se janela for fechada.
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }

        // Exibe prompt e lê comando.
        printf("allocator> ");
        if (!fgets(comando, sizeof(comando), stdin)) {
            break;
        }
        // Remove nova linha.
        comando[strcspn(comando, "\n")] = 0;

        // Processa comando RQ (alocação).
        if (strncmp(comando, "RQ", 2) == 0) {
            char nome[20], modo;
            int tam;
            // Extrai parâmetros.
            if (sscanf(comando, "RQ %s %d %c", nome, &tam, &modo) != 3) {
                printf("Erro: formato inválido. Use: RQ <nome> <tamanho> <F|B|W>\n");
                continue;
            }
            // Valida tamanho.
            if (tam <= 0) {
                printf("Erro: tamanho deve ser positivo.\n");
                continue;
            }
            // Valida estratégia.
            if (modo != 'F' && modo != 'B' && modo != 'W') {
                printf("Erro: estratégia deve ser F, B ou W.\n");
                continue;
            }
            // Valida tamanho do nome.
            if (strlen(nome) >= 20) {
                printf("Erro: nome do processo muito longo.\n");
                continue;
            }
            // Aloca memória.
            requisitar_memoria(nome, tam, modo);
        }
        // Processa comando RL (liberação).
        else if (strncmp(comando, "RL", 2) == 0) {
            char nome[20];
            // Extrai nome.
            if (sscanf(comando, "RL %s", nome) != 1) {
                printf("Erro: formato inválido. Use: RL <nome>\n");
                continue;
            }
            // Libera memória.
            liberar_memoria(nome);
        }
        // Processa comando C (compactação).
        else if (strncmp(comando, "C", 1) == 0) {
            // Compacta memória.
            compactar_memoria();
        }
        // Processa comando STAT (estado).
        else if (strncmp(comando, "STAT", 4) == 0) {
            // Exibe estado da memória.
            mostrar_memoria();
        }
        // Processa comando X (sair).
        else if (strncmp(comando, "X", 1) == 0) {
            // Encerra loop.
            break;
        }
        // Processa comando inválido.
        else if (strlen(comando) > 0) {
            printf("Comando inválido.\n");
        }

        // Atualiza janela gráfica.
        render_memory(renderer);
    }

    // Libera renderizador.
    SDL_DestroyRenderer(renderer);
    // Libera janela.
    SDL_DestroyWindow(window);
    // Encerra SDL.
    SDL_Quit();

    // Libera segmentos de memória.
    Segmento* atual = memoria;
    while (atual != NULL) {
        Segmento* temp = atual;
        atual = atual->prox;
        free(temp);
    }

    // Retorna 0 (sucesso).
    return 0;
}