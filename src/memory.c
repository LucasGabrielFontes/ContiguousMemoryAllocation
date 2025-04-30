#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "../include/memory.h"

// Declara a variável global 'memoria', um ponteiro para o início da lista encadeada de segmentos.
// Inicialmente NULL, será alocado dinamicamente ao inicializar a memória.
Segmento* memoria = NULL;
// Declara a variável global 'TAM_MEMORIA', que armazena o tamanho total da memória em bytes.
// Inicializada como 0, será definida na função inicializar_memoria.
int TAM_MEMORIA = 0;

// Define códigos de cores ANSI para formatação no terminal.
// Verde para processos alocados, amarelo para buracos (memória livre), e reset para padrão.
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"

// Registra operações em um arquivo de log chamado "memory.log" com formatação variável.
void log_operacao(const char* operacao, const char* formato, ...) {
    // Abre o arquivo "memory.log" em modo append para adicionar novas entradas.
    FILE* log_file = fopen("memory.log", "a");
    // Verifica se o arquivo foi aberto com sucesso.
    if (log_file) {
        // Obtém o tempo atual em segundos desde a epoch.
        time_t now = time(NULL);
        // Converte o tempo para uma string legível (ex.: "Wed Apr 30 10:00:00 2025").
        char* timestamp = ctime(&now);
        // Remove o caractere de nova linha (\n) do timestamp.
        timestamp[strlen(timestamp) - 1] = '\0';

        // Inicializa a lista de argumentos variáveis para processar 'formato'.
        va_list args;
        va_start(args, formato);
        // Escreve no arquivo: timestamp, tipo de operação e mensagem formatada.
        fprintf(log_file, "[%s] %s: ", timestamp, operacao);
        // Escreve a mensagem formatada usando argumentos variáveis.
        vfprintf(log_file, formato, args);
        // Adiciona uma nova linha ao final da entrada.
        fprintf(log_file, "\n");
        // Libera os recursos associados aos argumentos variáveis.
        va_end(args);

        // Fecha o arquivo de log.
        fclose(log_file);
    }
}

// Cria e retorna um novo segmento de memória (processo ou buraco).
Segmento* criar_segmento(const char* nome, int inicio, int fim) {
    // Aloca memória dinamicamente para um novo segmento.
    Segmento* novo = (Segmento*)malloc(sizeof(Segmento));
    // Copia o nome fornecido para o campo 'nome' do segmento.
    strcpy(novo->nome, nome);
    // Define a posição inicial do segmento.
    novo->inicio = inicio;
    // Define a posição final do segmento.
    novo->fim = fim;
    // Inicializa o ponteiro para o próximo segmento como NULL.
    novo->prox = NULL;
    // Retorna o ponteiro para o novo segmento.
    return novo;
}

// Inicializa a memória com um único grande buraco (Unused).
void inicializar_memoria(int tamanho) {
    // Define o tamanho total da memória na variável global.
    TAM_MEMORIA = tamanho;
    // Cria um segmento que ocupa toda a memória, marcado como "Unused".
    memoria = criar_segmento("Unused", 0, tamanho - 1);
    // Registra a operação de inicialização no log.
    log_operacao("Inicialização", "Memória inicializada com tamanho %d bytes", tamanho);
}

// Exibe o estado atual da memória no terminal com uma tabela de segmentos e estatísticas.
void mostrar_memoria() {
    // Imprime um cabeçalho para a saída.
    printf("\n=== Estado da Memória ===\n");
    // Ponteiro para percorrer a lista de segmentos.
    Segmento* atual = memoria;
    // Inicializa variáveis para estatísticas.
    int total_alocado = 0;   // Bytes ocupados por processos.
    int maior_buraco = 0;    // Tamanho do maior buraco.
    int num_buracos = 0;     // Número de buracos.
    int total_buracos = 0;   // Bytes em buracos.

    // Calcula estatísticas percorrendo todos os segmentos.
    while (atual != NULL) {
        // Calcula o tamanho do segmento (fim - início + 1).
        int tamanho = atual->fim - atual->inicio + 1;
        // Se o segmento é um buraco:
        if (strcmp(atual->nome, "Unused") == 0) {
            // Incrementa o contador de buracos.
            num_buracos++;
            // Soma o tamanho ao total de bytes livres.
            total_buracos += tamanho;
            // Atualiza o maior buraco, se necessário.
            if (tamanho > maior_buraco) maior_buraco = tamanho;
        } else {
            // Se é um processo, soma ao total alocado.
            total_alocado += tamanho;
        }
        // Avança para o próximo segmento.
        atual = atual->prox;
    }

    // Exibe a tabela de segmentos.
    atual = memoria;
    printf("Segmentos:\n");
    while (atual != NULL) {
        // Calcula o tamanho do segmento.
        int tamanho = atual->fim - atual->inicio + 1;
        // Imprime detalhes do segmento com formatação alinhada, usando cores:
        // Amarelo para buracos, verde para processos.
        printf("%sNome: %-10s | Início: %-6d | Fim: %-6d | Tamanho: %d%s\n",
               strcmp(atual->nome, "Unused") == 0 ? ANSI_COLOR_YELLOW : ANSI_COLOR_GREEN,
               atual->nome, atual->inicio, atual->fim, tamanho, ANSI_COLOR_RESET);
        // Avança para o próximo segmento.
        atual = atual->prox;
    }

    // Exibe estatísticas.
    printf("\nEstatísticas:\n");
    // Total de bytes alocados e percentual.
    printf("Total alocado: %d bytes (%.2f%%)\n", total_alocado, (float)total_alocado / TAM_MEMORIA * 100);
    // Total de bytes livres e percentual.
    printf("Total livre: %d bytes (%.2f%%)\n", TAM_MEMORIA - total_alocado, (float)(TAM_MEMORIA - total_alocado) / TAM_MEMORIA * 100);
    // Número de buracos.
    printf("Número de buracos: %d\n", num_buracos);
    // Tamanho do maior buraco.
    printf("Maior buraco: %d bytes\n", maior_buraco);
    printf("========================\n\n");

    // Registra a exibição do estado no log.
    log_operacao("Estado", "Exibido estado da memória");
}

// Aloca memória para um processo usando a estratégia especificada (F, B, W).
void requisitar_memoria(const char* nome, int tamanho, char estrategia) {
    // Ponteiro para percorrer a lista de segmentos.
    Segmento* atual = memoria;
    // Ponteiro para o segmento escolhido para alocação.
    Segmento* escolhido = NULL;
    // Ponteiro para o segmento anterior ao atual.
    Segmento* anterior = NULL;
    // Ponteiro para o anterior ao segmento escolhido (usado em Best/Worst-Fit).
    Segmento* anterior_escolhido = NULL;

    // First-Fit: Escolhe o primeiro buraco grande o suficiente.
    if (estrategia == 'F') {
        while (atual != NULL) {
            // Verifica se é um buraco e tem tamanho suficiente.
            if (strcmp(atual->nome, "Unused") == 0 &&
                atual->fim - atual->inicio + 1 >= tamanho) {
                escolhido = atual;
                // Para ao encontrar o primeiro buraco válido.
                break;
            }
            // Atualiza o anterior e avança.
            anterior = atual;
            atual = atual->prox;
        }
    }
    // Best-Fit: Escolhe o menor buraco que comporta o processo.
    else if (estrategia == 'B') {
        // Inicializa 'menor' com um valor maior que qualquer buraco.
        int menor = TAM_MEMORIA + 1;
        while (atual != NULL) {
            // Calcula o tamanho do segmento.
            int tam = atual->fim - atual->inicio + 1;
            // Verifica se é um buraco, tem tamanho suficiente e é menor que 'menor'.
            if (strcmp(atual->nome, "Unused") == 0 &&
                tam >= tamanho && tam < menor) {
                menor = tam;
                escolhido = atual;
                anterior_escolhido = anterior;
            }
            // Atualiza o anterior e avança.
            anterior = atual;
            atual = atual->prox;
        }
        // Define o anterior como o anterior do segmento escolhido.
        anterior = anterior_escolhido;
    }
    // Worst-Fit: Escolhe o maior buraco disponível.
    else if (estrategia == 'W') {
        // Inicializa 'maior' com -1 (nenhum buraco encontrado).
        int maior = -1;
        while (atual != NULL) {
            // Calcula o tamanho do segmento.
            int tam = atual->fim - atual->inicio + 1;
            // Verifica se é um buraco, tem tamanho suficiente e é maior que 'maior'.
            if (strcmp(atual->nome, "Unused") == 0 &&
                tam >= tamanho && tam > maior) {
                maior = tam;
                escolhido = atual;
                anterior_escolhido = anterior;
            }
            // Atualiza o anterior e avança.
            anterior = atual;
            atual = atual->prox;
        }
        // Define o anterior como o anterior do segmento escolhido.
        anterior = anterior_escolhido;
    }

    // Se não encontrou buraco adequado, exibe erro e registra no log.
    if (escolhido == NULL) {
        printf("Erro: memória insuficiente para alocar %s\n", nome);
        log_operacao("Erro", "Memória insuficiente para %s", nome);
        return; // Encerra a função sem retornar valor, pois é void.
    }

    // Define o início do novo segmento (mesmo início do buraco).
    int inicio = escolhido->inicio;
    // Define o fim do novo segmento (início + tamanho - 1).
    int fim = inicio + tamanho - 1;

    // Cria um novo segmento para o processo.
    Segmento* novo = criar_segmento(nome, inicio, fim);

    // Se o buraco é maior que o tamanho necessário:
    if (fim < escolhido->fim) {
        // Ajusta o buraco para começar após o novo segmento.
        escolhido->inicio = fim + 1;
        // Conecta o novo segmento ao buraco ajustado.
        novo->prox = escolhido;
        // Se o buraco era o primeiro segmento, atualiza o início da lista.
        if (anterior == NULL)
            memoria = novo;
        // Caso contrário, conecta o anterior ao novo segmento.
        else
            anterior->prox = novo;
    }
    // Se o buraco é usado completamente:
    else {
        // Conecta o novo segmento ao próximo do buraco.
        novo->prox = escolhido->prox;
        // Se o buraco era o primeiro segmento, atualiza o início da lista.
        if (anterior == NULL)
            memoria = novo;
        // Caso contrário, conecta o anterior ao novo segmento.
        else
            anterior->prox = novo;
        // Libera o buraco original.
        free(escolhido);
    }

    // Registra a alocação no log.
    log_operacao("Alocação", "Alocado %s, tamanho %d, estratégia %c", nome, tamanho, estrategia);
}

// Libera a memória de um processo, marcando-o como "Unused" e mesclando buracos adjacentes.
void liberar_memoria(const char* nome) {
    // Ponteiro para percorrer a lista de segmentos.
    Segmento* atual = memoria;
    // Flag para indicar se o processo foi liberado.
    int liberado = 0;

    // Procura o processo e o marca como "Unused".
    while (atual != NULL) {
        // Se o segmento é o processo procurado:
        if (strcmp(atual->nome, nome) == 0) {
            // Marca como buraco.
            strcpy(atual->nome, "Unused");
            // Define a flag como verdadeira.
            liberado = 1;
        }
        // Avança para o próximo segmento.
        atual = atual->prox;
    }

    // Mescla buracos adjacentes.
    atual = memoria;
    while (atual != NULL && atual->prox != NULL) {
        // Se o segmento atual e o próximo são buracos:
        if (strcmp(atual->nome, "Unused") == 0 && strcmp(atual->prox->nome, "Unused") == 0) {
            // Estende o segmento atual até o fim do próximo.
            atual->fim = atual->prox->fim;
            // Armazena o próximo segmento.
            Segmento* temp = atual->prox;
            // Conecta o atual ao segmento após o próximo.
            atual->prox = temp->prox;
            // Libera o segmento próximo.
            free(temp);
        } else {
            // Avança para o próximo segmento.
            atual = atual->prox;
        }
    }

    // Registra o resultado no log.
    if (liberado) {
        log_operacao("Liberação", "Processo %s liberado", nome);
    } else {
        log_operacao("Erro", "Processo %s não encontrado para liberação", nome);
    }
}

// Compacta a memória, movendo processos para o início e unindo buracos no final.
void compactar_memoria() {
    // Ponteiro para a nova lista de segmentos.
    Segmento* nova_lista = NULL;
    // Ponteiro para o último segmento da nova lista.
    Segmento* fim_lista = NULL;
    // Posição atual do início do próximo segmento.
    int atual_inicio = 0;

    // Percorre a lista original.
    Segmento* atual = memoria;
    while (atual != NULL) {
        // Se o segmento é um processo:
        if (strcmp(atual->nome, "Unused") != 0) {
            // Calcula o tamanho do segmento.
            int tamanho = atual->fim - atual->inicio + 1;
            // Cria um novo segmento reposicionado.
            Segmento* novo = criar_segmento(atual->nome, atual_inicio, atual_inicio + tamanho - 1);
            // Atualiza a posição do próximo segmento.
            atual_inicio += tamanho;
            // Se a nova lista está vazia, define o novo segmento como início.
            if (nova_lista == NULL)
                nova_lista = novo;
            // Caso contrário, conecta o último segmento ao novo.
            else
                fim_lista->prox = novo;
            // Atualiza o fim da lista.
            fim_lista = novo;
        }
        // Avança para o próximo segmento.
        atual = atual->prox;
    }

    // Se há espaço não utilizado:
    if (atual_inicio < TAM_MEMORIA) {
        // Cria um buraco para o espaço restante.
        Segmento* buraco = criar_segmento("Unused", atual_inicio, TAM_MEMORIA - 1);
        // Se a lista está vazia, define o buraco como início.
        if (fim_lista == NULL)
            nova_lista = buraco;
        // Caso contrário, conecta o último segmento ao buraco.
        else
            fim_lista->prox = buraco;
    }

    // Libera a lista original.
    atual = memoria;
    while (atual != NULL) {
        // Armazena o segmento atual.
        Segmento* temp = atual;
        // Avança para o próximo segmento.
        atual = atual->prox;
        // Libera o segmento atual.
        free(temp);
    }

    // Define a nova lista como a lista de memória ativa.
    memoria = nova_lista;
    // Registra a compactação no log.
    log_operacao("Compactação", "Memória compactada");
}