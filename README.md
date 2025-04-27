# Simulador de Alocação Contígua de Memória

## Do que se trata o projeto

Este projeto é um simulador de **alocação contígua de memória**, desenvolvido para a disciplina de **Sistemas Operacionais** da UFPB, com base no Capítulo 9 (Memória Principal). Ele implementa as três principais estratégias de alocação contígua:

- **First Fit (F)**: Aloca no primeiro espaço de memória disponível que caiba o processo.
- **Best Fit (B)**: Aloca no menor espaço de memória que caiba o processo.
- **Worst Fit (W)**: Aloca no maior espaço de memória disponível.

### Funcionalidades
- **Alocação de processos**: Suporta as estratégias F, B e W com validação de entrada.
- **Liberação de processos**: Libera memória e mescla buracos adjacentes automaticamente para reduzir fragmentação.
- **Compactação de memória**: Realoca processos para o início e unifica buracos ao final.
- **Visualização no terminal**: Exibe tabela de segmentos, mapa gráfico com cores e estatísticas detalhadas (memória alocada, livre, número de buracos, maior buraco, índice de fragmentação).
- **Interface gráfica (SDL2)**: Mostra processos (retângulos verdes) e buracos (retângulos amarelos) em uma barra horizontal.
- **Registro de operações**: Todas as ações (alocação, liberação, compactação, erros) são salvas em `memory.log` com carimbos de data/hora.
- **Testes automatizados**: Suporta entrada de comandos via arquivos de teste.

É uma ferramenta didática que demonstra, de forma interativa, os efeitos das estratégias de alocação e o impacto da fragmentação externa, com interfaces amigáveis para visualização.

---

## Imagem da Interface

### Terminal
![Exemplo da Interface no Terminal](exemplo-terminal.png)

*A saída do terminal mostra a tabela de segmentos, um mapa gráfico (# para processos, . para buracos) e estatísticas, com cores para facilitar a leitura.*

### Gráfica (SDL2)
![Exemplo da Interface Gráfica](exemplo-gui.png)

*A interface gráfica exibe processos como retângulos verdes e buracos como retângulos amarelos, atualizando em tempo real após cada comando.*

---

## Estrutura do Projeto
- `src/`: Código-fonte.
  - `allocator.c`: Interface de terminal.
  - `gui.c`: Interface gráfica com SDL2.
  - `memory.c`: Funções de gerenciamento de memória.
- `include/`: Cabeçalhos.
  - `memory.h`: Definições de estruturas e funções.
- `testes/`: Arquivos de teste.
  - `teste.txt`: Exemplo de teste automatizado.
  - `saida.log`: Saída gerada pelos testes.
- `memory.log`: Arquivo de log gerado com registros de operações.
- `README.md`: Este documento.

---

## Como Compilar e Executar (Linux)

### Requisitos
- **Sistema Operacional**: Linux (testado no Ubuntu).
- **Compilador**: GCC.
- **Biblioteca SDL2**: Necessária para a interface gráfica.
  - Instale com: `sudo apt-get install libsdl2-dev`

### Compilação
No diretório do projeto, compile os executáveis manualmente:

1. **Simulador de terminal**:
   ```bash
   gcc src/allocator.c src/memory.c -Iinclude -o allocator
   ```
2. **Simulador gráfico**:
   ```bash
   gcc src/gui.c src/memory.c -Iinclude -o gui -lSDL2
   ```

### Execução
1. **Terminal**:
   ```bash
   ./allocator <tamanho_memoria>
   ```
   Exemplo: `./allocator 1000`
2. **Gráfico**:
   ```bash
   ./gui <tamanho_memoria>
   ```
   Exemplo: `./gui 1000`

---

## Instruções de Uso

Você interage com o programa por meio de comandos no prompt `allocator>`. Os comandos são os mesmos para as interfaces de terminal e gráfica.

### Comandos
1. **`RQ <nome> <tamanho> <F|B|W>`**
   - **Descrição**: Solicita alocação de memória para um processo.
   - **Parâmetros**:
     - `<nome>`: Nome do processo (máximo 19 caracteres).
     - `<tamanho>`: Tamanho em bytes (positivo).
     - `<F|B|W>`: Estratégia (First Fit, Best Fit, Worst Fit).
   - **Exemplo**: `RQ P1 100 F`
     - Aloca 100 bytes para o processo P1 usando First Fit.

2. **`RL <nome>`**
   - **Descrição**: Libera a memória de um processo, mesclando buracos adjacentes.
   - **Parâmetro**: `<nome>` (nome do processo).
   - **Exemplo**: `RL P1`
     - Libera a memória do processo P1.

3. **`C`**
   - **Descrição**: Compacta a memória, movendo processos para o início e unificando buracos.
   - **Exemplo**: `C`
     - Executa a compactação.

4. **`STAT`**
   - **Descrição**: Exibe o estado da memória, incluindo:
     - Tabela de segmentos (nome, início, fim, tamanho).
     - Mapa gráfico (# para processos, . para buracos).
     - Estatísticas (memória alocada, livre, número de buracos, maior buraco, fragmentação).
   - **Exemplo**: `STAT`
     - Mostra o estado atual.

5. **`X`**
   - **Descrição**: Encerra o simulador.
   - **Exemplo**: `X`
     - Fecha o programa.

### Exemplo de Sessão
```bash
allocator> RQ P1 300 F
allocator> RQ P2 400 B
allocator> STAT
allocator> RL P1
allocator> C
allocator> STAT
allocator> X
```

### Arquivo de Log
- Todas as operações são registradas em `memory.log` no diretório de execução.
- Formato: `[timestamp] operação: detalhes`
- Exemplo:
  ```
  [Wed Apr 26 15:30:45 2025] Inicialização: Memória inicializada com tamanho 1000 bytes
  [Wed Apr 26 15:30:50 2025] Alocação: Alocado P1, tamanho 300, estratégia F
  ```

---

## Testes

### Teste Automatizado
Um teste pré-pronto está disponível em `testes/teste.txt`. Para executá-lo:

```bash
./allocator 1000 < testes/teste.txt > testes/saida.log
```

Para visualizar o resultado:
```bash
cat testes/saida.log
```

### Criando Novos Testes
Crie um arquivo de texto com comandos (um por linha) e redirecione para o programa:
```bash
./allocator 1000 < seu_teste.txt
```

---

## Contribuidores
- **Antônio Augusto Dantas Neto**: Revisou e modularizou o código-fonte, validou o uso de memória e realizou testes de execução.
- **Deivily Breno Silva Carneiro**: Implementou as estratégias de alocação (First Fit, Best Fit, Worst Fit), liberação, compactação, mesclagem de buracos, validação de entrada e logging. Atuou na depuração e otimização.
- **Lucas Gabriel Fontes da Silva**: Estruturou o projeto no GitHub, criou o README, organizou diretórios (`src`, `include`, `testes`), implementou a interface gráfica (SDL2), adicionou métricas de fragmentação e coordenou a integração das tarefas.

---

## Notas
- **Validação**: O programa verifica tamanhos positivos, nomes com menos de 20 caracteres, estratégias válidas (F, B, W) e formato correto dos comandos.
- **Fragmentação**: O índice de fragmentação externa é exibido no comando `STAT`.
- **Imagens**: Substitua `exemplo-terminal.png` e `exemplo-gui.png` por capturas de tela reais da execução.
- **Log**: Verifique `memory.log` para rastrear operações.
- **SDL2**: Certifique-se de que a biblioteca está instalada para a interface gráfica.