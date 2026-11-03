# Problema 13 — Distâncias de pontos 3D (versão paralela)

Implementação paralela em linguagem C com a biblioteca `pthread.h`, desenvolvida para a atividade de Sistemas Operacionais. O programa calcula a distância euclidiana de cada ponto 3D até um ponto de referência e compara o desempenho utilizando diferentes quantidades de threads.

## 1. Estrutura do projeto

Os comandos deste README devem ser executados **a partir da pasta `trabalhoPthreads`**.

```text
trabalhoPthreads/
├── docs/
│   ├── paralelo/
│   │   ├── 2 THREADS.txt
│   │   ├── 4 THREADS.txt
│   │   ├── 8 THREADS.txt
│   │   ├── AUTO THREADS.txt
│   │   └── resumo_paralelo.txt
│   └── sequencial/
│       ├── resultados_sequencial.txt
│       └── resumo_sequencial.txt
├── entradas/
│   ├── pequena.txt
│   ├── media.txt
│   └── grande.txt
├── problema13_paralelo/
│   ├── dist3d_paralelo.c
│   ├── dist3d_paralelo          # gerado após compilar
│   └── README.md
└── problema13_sequencial/
    ├── sequencial.c
    ├── sequencial               # gerado após compilar
    └── README_sequencial.md
```

## 2. Formato das entradas

A primeira linha informa a quantidade de pontos (`N`). A segunda informa as coordenadas do ponto de referência (`xr`, `yr`, `zr`). As linhas seguintes contêm as coordenadas dos pontos a serem analisados:

```text
N
xr yr zr
x1 y1 z1
x2 y2 z2
...
xN yN zN
```

Arquivos utilizados:

| Arquivo | Quantidade de pontos |
| --- | ---: |
| `entradas/pequena.txt` | 20.000 |
| `entradas/media.txt` | 120.000 |
| `entradas/grande.txt` | 300.000 |

## 3. Ambiente utilizado

- Sistema principal: Windows.
- Ambiente de compilação e execução: Ubuntu pelo WSL (Windows Subsystem for Linux).
- Processador: Intel Core i5-14600KF.
- Memória RAM: 32 GB.
- Configurações paralelas testadas: **2, 4, 8 e 20 threads** (20 foi o valor identificado na execução com `auto`).
- Compilador: GCC, com otimização `-O2`.

## 4. Acessar a pasta pelo Ubuntu WSL

Abra o terminal do Ubuntu e execute:

```bash
cd /mnt/c/Users/Gabriel/Desktop/Faculdade/SO/Trabalho/trabalhoPthreads
```

> **Importante:** mantenha o terminal nessa pasta ao compilar e executar, pois os caminhos dos arquivos `entradas/...` são relativos à raiz do projeto.

## 5. Compilar

```bash
gcc -O2 -Wall -Wextra -pthread problema13_paralelo/dist3d_paralelo.c -o problema13_paralelo/dist3d_paralelo -lm
```

Significado das principais opções:

- `-O2`: ativa otimizações do GCC.
- `-Wall -Wextra`: habilita avisos adicionais de compilação.
- `-pthread`: habilita o uso das threads POSIX.
- `-o`: define o nome e o local do executável.
- `-lm`: vincula a biblioteca matemática usada por `sqrt()`.

Depois de alterar o código-fonte, compile novamente antes de executar.

## 6. Executar

O programa recebe dois parâmetros obrigatórios:

```bash
./problema13_paralelo/dist3d_paralelo <arquivo_de_entrada> <quantidade_de_threads>
```

O valor `auto` faz o programa utilizar a quantidade de CPUs lógicas online detectada pelo ambiente. Nos testes realizados, essa configuração correspondeu a **20 threads**.

### Entrada grande — 300.000 pontos

```bash
./problema13_paralelo/dist3d_paralelo entradas/grande.txt auto
./problema13_paralelo/dist3d_paralelo entradas/grande.txt 2
./problema13_paralelo/dist3d_paralelo entradas/grande.txt 4
./problema13_paralelo/dist3d_paralelo entradas/grande.txt 8
```

### Entrada pequena — 20.000 pontos

```bash
./problema13_paralelo/dist3d_paralelo entradas/pequena.txt auto
./problema13_paralelo/dist3d_paralelo entradas/pequena.txt 2
./problema13_paralelo/dist3d_paralelo entradas/pequena.txt 4
./problema13_paralelo/dist3d_paralelo entradas/pequena.txt 8
```

### Entrada média — 120.000 pontos

```bash
./problema13_paralelo/dist3d_paralelo entradas/media.txt auto
./problema13_paralelo/dist3d_paralelo entradas/media.txt 2
./problema13_paralelo/dist3d_paralelo entradas/media.txt 4
./problema13_paralelo/dist3d_paralelo entradas/media.txt 8
```

## 7. Como o programa funciona

1. Abre o arquivo TXT e lê a quantidade de pontos e o ponto de referência.
2. Lê os pontos e os armazena em um vetor na memória.
3. Divide os pontos em faixas, conforme a quantidade de threads solicitada.
4. Cria as threads com `pthread_create()`. Cada uma calcula as distâncias da sua própria faixa.
5. Aguarda o término de todas as threads com `pthread_join()`.
6. Mostra o tempo de execução, algumas distâncias calculadas e os pontos mais próximo e mais distante.

A distância é calculada pela fórmula:

```text
d = sqrt((x - xr)^2 + (y - yr)^2 + (z - zr)^2)
```

Cada thread escreve em posições diferentes do vetor de distâncias, evitando que duas threads alterem o mesmo resultado.

## 8. Medição de tempo e resultados

O tempo é medido com `clock_gettime(CLOCK_MONOTONIC)` em **milissegundos (ms)**. Na versão paralela, a região cronometrada inclui a criação das threads, o cálculo das distâncias e a espera por sua conclusão. A leitura dos arquivos, a busca pelos pontos extremos e as impressões no terminal ficam fora da medição.

O programa apresenta, entre outras informações:

```text
Arquivo em analise: entradas/pequena.txt
Quantidade de threads: 4
Tempo de execucao: ... ms

Ponto mais proximo: ...
Coordenadas: (..., ..., ...)
Distancia: ...

Ponto mais distante: ...
Coordenadas: (..., ..., ...)
Distancia: ...

Primeiras 5 distancias:
Ponto 1: ...
...
```

O tempo varia conforme a execução, mesmo mantendo o mesmo arquivo e a mesma quantidade de threads.

### Registro manual (sem CSV)

Foram realizadas **5 execuções por combinação** de arquivo e número de threads:

- 3 tamanhos de entrada;
- 4 configurações paralelas (2, 4, 8 e 20 threads);
- 5 repetições em cada configuração;
- **60 execuções paralelas no total**.

Os tempos individuais e as saídas dos testes foram guardados nos arquivos `docs/paralelo/2 THREADS.txt`, `4 THREADS.txt`, `8 THREADS.txt` e `AUTO THREADS.txt`. As médias e o resumo dos resultados ficam em `docs/paralelo/resumo_paralelo.txt`.

A implementação sequencial e seus resultados estão em `problema13_sequencial/` e `docs/sequencial/`, respectivamente. A comparação usa os mesmos arquivos de entrada e a mesma otimização de compilação (`-O2`).

## 9. Observações para análise

A versão paralela não é necessariamente mais rápida. Criar e aguardar threads tem um custo, que pode superar o benefício de dividir cálculos simples, especialmente para entradas pequenas ou quantidades altas de threads.

Para avaliar os resultados, comparamos as médias dos tempos e calculamos:

```text
Speedup = tempo_medio_sequencial / tempo_medio_paralelo
Eficiencia = Speedup / numero_de_threads
```

Também é importante conferir se as distâncias produzidas pela versão paralela correspondem às da versão sequencial para a mesma entrada.
