# Simulador de Alocação Contígua de Memória

## Do que se trata o projeto

Este projeto é um simulador de **alocação contígua de memória**, desenvolvido para a disciplina de **Sistemas Operacionais** da UFPB.

Ele implementa as três principais estratégias de alocação contígua:
- **First Fit**: aloca no primeiro espaço de memória disponível.
- **Best Fit**: aloca no menor espaço de memória possível que caiba o processo.
- **Worst Fit**: aloca no maior espaço de memória disponível.

Além disso, o simulador permite:
- **Liberar memória de processos**;
- **Compactar a memória**, realocando processos e unificando espaços;
- **Visualizar o estado atual da memória**.

É uma ferramenta didática que mostra, de forma clara e interativa, os efeitos de diferentes estratégias de alocação e o impacto da fragmentação.

---

## Imagem da interface (terminal)

![Exemplo da Interface no Terminal](exemplo-interface.png)

> A imagem (ainda não tem kkk) acima mostra uma sessão no terminal com comandos sendo executados e a memória sendo exibida após alocação, liberação e compactação.

---

## Como compilar e executar (Linux)

### Requisitos:
- Sistema Operacional **Linux**
- Compilador `gcc` instalado

### Compilação com Makefile:

Abra o terminal no diretório do projeto e utilize os seguintes comandos:

```bash
make         # compila
make run     # executa com 1MB (esse valor pode ser modificado dentro do arquivo makefile)
make clean   # remove o executável
```

### Instruções de uso

Você interage com o programa por meio de comandos no terminal:

Explicação de cada comando:

### 1. `RQ <nome> <tamanho> <F|B|W>`

RQ = Request (Solicitação de alocação de memória)

- `<nome>`: nome do processo a ser alocado.
- `<tamanho>`: tamanho em bytes da memória a ser alocada.
- `<F|B|W>`: estratégia de alocação a ser utilizada:

  - First Fit (F): usa o primeiro espaço de memória que couber

  - Best Fit (B): usa o menor espaço de memória que couber

  - Worst Fit (W): usa o maior espaço de memória disponível

Esse comando serve para alocar um bloco de memória para um processo com o nome e o tamanho especificados, usando uma das três estratégias.

Exemplo de uso:

```bash
RQ P1 10000 F
```

Explicação: aloca 10.000 bytes para o processo P1 usando a estratégia First Fit.

### 2. RL <nome>

RL = Release (Liberação de memória)

`<nome>`: nome do processo cuja memória será liberada.

Libera a memória ocupada pelo processo com o nome especificado. Após isso, o espaço usado por ele se torna um espaço livre (Unused).

Exemplo de uso: 

```bash
RL P1
```

Explicação: libera a memória do processo P1.

### 3. C

C = Compact (Compactar a memória)

Compacta a memória, ou seja:

- Move todos os processos para o início da memória (sem alterar seu tamanho);

- Junta todos os buracos em um só ao final.

Isso ajuda a reduzir a fragmentação externa, quando há espaços de memória disponíveis para serem alocadoa a determinado processo, mas não formam um espaço contíguo de memória.

Exemplo de uso:

```bash
C
```

Explicação: executa a compactação da memória.

### 4. STAT

STAT = Status da memória

Mostra, de forma organizada, a lista de segmentos da memória atual, incluindo:

- Nome do processo (ou "Unused")

- Endereço de início

- Endereço de fim

- Tamanho de cada bloco

Exemplo de uso:

```bash
STAT
```

Explicação: mostra o mapa da memória atual.

### 5. X

X = Exit

Finaliza o programa. Encerra o simulador de forma segura.

Exemplo de uso:

```bash
X
```

Explicação: fecha o simulador.

### Exemplo de Sessão Completa

```bash
allocator> RQ P1 30000 F
allocator> RQ P2 40000 B
allocator> STAT
allocator> RL P1
allocator> C
allocator> STAT
allocator> X
```

👥 Contribuidores

- Antônio Augusto Dantas Neto: colaborou na implementação do Makefile, auxiliou na revisão e modularização do código-fonte, além de participar da validação de uso de memória e testes de execução.

- Deivily Breno Silva Carneiro: responsável pela maior parte da implementação em C, incluindo as estratégias de alocação (First Fit, Best Fit, Worst Fit), liberação e compactação da memória. Também atuou na depuração e otimização da lógica.

- Lucas Gabriel Fontes da Silva:  estruturou o projeto no GitHub, criou o README.md, organizou os diretórios (src, include, testes, obj), realizou testes manuais e automatizados, e coordenou a divisão e integração das tarefas entre os membros.
