# Simulador de Alocação Contígua de Memória

## Do que se trata o projeto

Este projeto é um simulador de **alocação contígua de memória**, desenvolvido para a disciplina de **Sistemas Operacionais** da UFPB.

Ele implementa as três principais estratégias de alocação contígua:
- **First Fit**: aloca no primeiro espaço de memória disponível.
- **Best Fit**: aloca no menor espaço de memória possível que caiba o processo.
- **Worst Fit**: aloca no maior espaço de memória disponível.

Além disso, o simulador permite:
- **liberar memória de processos**;
- **compactar a memória**, realocando processos e unificando espaços;
- **visualizar o estado atual da memória**.

É uma ferramenta didática que mostra, de forma clara e interativa, os efeitos de diferentes estratégias de alocação e o impacto da fragmentação.

---

## Imagem da interface (terminal)

![Exemplo da Interface no Terminal](exemplo-interface.png)

> A imagem acima mostra uma sessão no terminal com comandos sendo executados e a memória sendo exibida após alocação, liberação e compactação.

---

## Como compilar e executar (Linux)

### Requisitos:
- Sistema Operacional **Linux**
- Compilador `gcc` instalado

### Compilação com Makefile:

make         # compila
make run     # executa com 1MB (esse valor pode ser modificado dentro do arquivo makefile)
make clean   # remove o executável

### Instruções de uso

Você interage com o programa por meio de comandos no terminal:

Explicação de cada comando:

#### RQ <nome> <tamanho> <F|B|W>

RQ = Request (Solicitação de alocação de memória)

Esse comando serve para alocar um bloco de memória para um processo com o nome e o tamanho especificados, usando uma das três estratégias:

1. First Fit (F): usa o primeiro espaço de memória que couber

2. Best Fit (B): usa o menor espaço de memória que couber

3. Worst Fit (W): usa o maior espaço de memória disponível

Exemplo: RQ P1 10000 F

Aloca 10.000 bytes para o processo P1 usando a estratégia First Fit.

##### RL <nome>

RL = Release (Liberação de memória)

Libera a memória ocupada pelo processo com o nome especificado. Após isso, o espaço usado por ele se torna um espaço livre (Unused).

Exemplo: RL P1

Libera a memória do processo P1.

#### C

C = Compact (Compactar a memória)

Compacta a memória, ou seja:

- Move todos os processos para o início da memória (sem alterar seu tamanho);

- Junta todos os buracos em um só ao final.

Isso ajuda a reduzir a fragmentação externa, quando há espaços de memória disponíveis para serem alocadoa a determinado processo, mas não formam um espaço contíguo de memória.

Exemplo:

C

Executa a compactação da memória.

#### STAT

STAT = Status da memória

Mostra, de forma organizada, a lista de segmentos da memória atual, incluindo:

- Nome do processo (ou "Unused")

- Início e fim

- Tamanho de cada bloco

Exemplo:

STAT

Mostra o mapa da memória atual.

#### X

X = Exit

Finaliza o programa. Encerra o simulador de forma segura.

Exemplo:

X

Fecha o simulador.

### Exemplo de Sessão Completa

allocator> RQ P1 30000 F
allocator> RQ P2 40000 B
allocator> STAT
allocator> RL P1
allocator> C
allocator> STAT
allocator> X

👥 Quem fez o quê

Antônio Augusto Dantas Neto: implementação do Makefile, validação do uso de memória, revisão do código e suporte à documentação.

Deivily Breno Silva Carneiro: desenvolvimento do código principal em C, implementação das estratégias de alocação e compactação.

Lucas Gabriel Fontes da Silva: estruturação geral do projeto, criação do README.md, testes manuais no terminal e divisão de tarefas.
