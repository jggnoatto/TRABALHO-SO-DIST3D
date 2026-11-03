# Problema 13 — Distâncias de Pontos 3D (Sequencial)

Trabalho da disciplina de Sistemas Operacionais. Esta parte implementa em **C** o cálculo sequencial da distância euclidiana entre um ponto de referência e cada ponto de um arquivo de entrada.

## Estrutura do projeto

```text
trabalhoPthreads/
├── docs/
├── entradas/
│   ├── pequena.txt
│   ├── media.txt
│   └── grande.txt
├── problema13_paralelo/
└── problema13_sequencial/
    ├── sequencial.c
    └── sequencial          # executável gerado após compilar
```

## Requisitos

- Ubuntu via WSL no Windows
- GCC instalado

## Compilação

Abra o terminal do **Ubuntu (WSL)** e acesse a **raiz do projeto**:

```bash
cd /mnt/c/Users/Gabriel/Desktop/Faculdade/SO/Trabalho/trabalhoPthreads
```

Compile o arquivo da pasta `problema13_sequencial`:

```bash
gcc -O2 -Wall -Wextra problema13_sequencial/sequencial.c -o problema13_sequencial/sequencial -lm
```

- `-O2`: ativa otimizações de compilação;
- `-Wall -Wextra`: habilita avisos do compilador;
- `-o`: define o local e o nome do executável;
- `-lm`: vincula a biblioteca matemática usada por `sqrt()`.

## Execução

**Permaneça na raiz `trabalhoPthreads`** ao executar:

```bash
./problema13_sequencial/sequencial
```

Isso é importante porque o programa abre os arquivos pelo caminho relativo `entradas/...`.

## Como mudar o arquivo de entrada

O programa **não recebe o arquivo como argumento do terminal**. Para escolher outra entrada, abra `problema13_sequencial/sequencial.c` e localize as linhas do `fopen()`.

Apenas troque de grande para pequena ou media.

```c
const char *nomeArquivo = "entradas/grande.txt";
```

**Sempre compile novamente após alterar o código**, antes de executar.

| Arquivo | Quantidade de pontos |
|---|---:|
| `pequena.txt` | 20.000 |
| `media.txt` | 120.000 |
| `grande.txt` | 300.000 |

## O que o programa faz

1. Abre o arquivo TXT e lê a quantidade de pontos.
2. Lê as coordenadas do ponto de referência `(x, y, z)`.
3. Carrega os demais pontos em um vetor na memória.
4. Calcula sequencialmente a distância de cada ponto até a referência.
5. Mede o tempo do laço de cálculo com `clock_gettime(CLOCK_MONOTONIC)` e apresenta o resultado em milissegundos.
6. Identifica o ponto mais próximo e o mais distante e mostra algumas distâncias para conferência.
7. Libera a memória alocada.

A distância utilizada é:

```text
d = sqrt((x - xr)^2 + (y - yr)^2 + (z - zr)^2)
```

**Observação sobre a medição:** a leitura do arquivo, a busca pelos extremos e as impressões no terminal ficam fora do trecho cronometrado. Os testes paralelos devem usar um critério equivalente para permitir comparação justa.
