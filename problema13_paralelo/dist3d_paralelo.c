#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    double x, y, z;
} Ponto3D;

typedef struct {
    const Ponto3D *pontos;
    Ponto3D referencia;
    double *distancias;
    size_t inicio;
    size_t fim;
} Tarefa;

static void *calcular_distancias(void *arg) {
    Tarefa *tarefa = (Tarefa *)arg;

    for (size_t i = tarefa->inicio; i < tarefa->fim; i++) {
        double dx = tarefa->pontos[i].x - tarefa->referencia.x;
        double dy = tarefa->pontos[i].y - tarefa->referencia.y;
        double dz = tarefa->pontos[i].z - tarefa->referencia.z;

        tarefa->distancias[i] = sqrt(dx * dx + dy * dy + dz * dz);
    }

    return NULL;
}

static double diferenca_ms(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) * 1000.0
         + (fim.tv_nsec - inicio.tv_nsec) / 1000000.0;
}

static int ler_ponto(FILE *arquivo, Ponto3D *ponto) {
    return fscanf(arquivo, "%lf %lf %lf",
                  &ponto->x, &ponto->y, &ponto->z) == 3;
}

int main(int argc, char **argv) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr,
                "Uso: %s <arquivo_entrada> <numero_threads|auto> [--distancias]\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    long cpus = sysconf(_SC_NPROCESSORS_ONLN);
    if (cpus < 1) cpus = 1;

    int mostrar_distancias = (argc == 4 && strcmp(argv[3], "--distancias") == 0);
    if (argc == 4 && !mostrar_distancias) {
        fprintf(stderr, "Opcao desconhecida: %s\n", argv[3]);
        return EXIT_FAILURE;
    }

    int num_threads;
    if (strcmp(argv[2], "auto") == 0) {
        if (cpus > 2147483647L) {
            fprintf(stderr, "Numero de CPUs fora do limite suportado.\n");
            return EXIT_FAILURE;
        }
        num_threads = (int)cpus;
    } else {
        char *fim = NULL;
        errno = 0;
        long valor = strtol(argv[2], &fim, 10);
        if (errno != 0 || fim == argv[2] || *fim != '\0' ||
            valor < 1 || valor > 2147483647L) {
            fprintf(stderr, "Numero de threads invalido: %s\n", argv[2]);
            return EXIT_FAILURE;
        }
        num_threads = (int)valor;
    }

    FILE *arquivo = fopen(argv[1], "r");
    if (!arquivo) {
        perror("Erro ao abrir arquivo de entrada");
        return EXIT_FAILURE;
    }

    size_t n;
    if (fscanf(arquivo, "%zu", &n) != 1 || n == 0) {
        fprintf(stderr, "Entrada invalida: N deve ser um inteiro positivo.\n");
        fclose(arquivo);
        return EXIT_FAILURE;
    }

    Ponto3D referencia;
    if (!ler_ponto(arquivo, &referencia)) {
        fprintf(stderr, "Entrada invalida: ponto de referencia ausente.\n");
        fclose(arquivo);
        return EXIT_FAILURE;
    }

    Ponto3D *pontos = malloc(n * sizeof(*pontos));
    double *distancias = malloc(n * sizeof(*distancias));
    if (!pontos || !distancias) {
        fprintf(stderr, "Falha ao alocar memoria para %zu pontos.\n", n);
        free(pontos);
        free(distancias);
        fclose(arquivo);
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < n; i++) {
        if (!ler_ponto(arquivo, &pontos[i])) {
            fprintf(stderr, "Entrada invalida: ponto %zu incompleto.\n", i + 1);
            free(pontos);
            free(distancias);
            fclose(arquivo);
            return EXIT_FAILURE;
        }
    }
    fclose(arquivo);

    /* Nao vale a pena criar mais threads do que pontos a processar. */
    if ((size_t)num_threads > n) num_threads = (int)n;

    pthread_t *threads = malloc((size_t)num_threads * sizeof(*threads));
    Tarefa *tarefas = malloc((size_t)num_threads * sizeof(*tarefas));
    if (!threads || !tarefas) {
        fprintf(stderr, "Falha ao alocar memoria para as threads.\n");
        free(threads);
        free(tarefas);
        free(pontos);
        free(distancias);
        return EXIT_FAILURE;
    }

    struct timespec inicio, fim;
    if (clock_gettime(CLOCK_MONOTONIC, &inicio) != 0) {
        perror("clock_gettime");
        free(threads); free(tarefas); free(pontos); free(distancias);
        return EXIT_FAILURE;
    }

    int criadas = 0;
    for (int i = 0; i < num_threads; i++) {
        size_t inicio_faixa = (n * (size_t)i) / (size_t)num_threads;
        size_t fim_faixa = (n * (size_t)(i + 1)) / (size_t)num_threads;

        tarefas[i].pontos = pontos;
        tarefas[i].referencia = referencia;
        tarefas[i].distancias = distancias;
        tarefas[i].inicio = inicio_faixa;
        tarefas[i].fim = fim_faixa;

        int erro = pthread_create(&threads[i], NULL, calcular_distancias, &tarefas[i]);
        if (erro != 0) {
            fprintf(stderr, "pthread_create falhou: %s\n", strerror(erro));
            for (int j = 0; j < criadas; j++) pthread_join(threads[j], NULL);
            free(threads); free(tarefas); free(pontos); free(distancias);
            return EXIT_FAILURE;
        }
        criadas++;
    }

    for (int i = 0; i < criadas; i++) {
        int erro = pthread_join(threads[i], NULL);
        if (erro != 0) {
            fprintf(stderr, "pthread_join falhou: %s\n", strerror(erro));
            free(threads); free(tarefas); free(pontos); free(distancias);
            return EXIT_FAILURE;
        }
    }

    if (clock_gettime(CLOCK_MONOTONIC, &fim) != 0) {
        perror("clock_gettime");
        free(threads); free(tarefas); free(pontos); free(distancias);
        return EXIT_FAILURE;
    }

    double tempo_ms = diferenca_ms(inicio, fim);

    /* A busca pelos extremos fica fora da medicao de tempo. */
    size_t indiceMaisProximo = 0;
    size_t indiceMaisDistante = 0;

    for (size_t i = 1; i < n; i++) {
        if (distancias[i] < distancias[indiceMaisProximo]) {
            indiceMaisProximo = i;
        }
        if (distancias[i] > distancias[indiceMaisDistante]) {
            indiceMaisDistante = i;
        }
    }

    printf("Arquivo em analise: %s\n", argv[1]);
    printf("Quantidade de threads: %d\n", num_threads);
    printf("Tempo de execucao: %.9f ms\n", tempo_ms);

    printf("Ponto mais proximo: %zu\n", indiceMaisProximo + 1);
    printf("Coordenadas: (%.4f, %.4f, %.4f)\n",
           pontos[indiceMaisProximo].x,
           pontos[indiceMaisProximo].y,
           pontos[indiceMaisProximo].z);
    printf("Distancia: %.4f\n\n", distancias[indiceMaisProximo]);

    printf("Ponto mais distante: %zu\n", indiceMaisDistante + 1);
    printf("Coordenadas: (%.4f, %.4f, %.4f)\n",
           pontos[indiceMaisDistante].x,
           pontos[indiceMaisDistante].y,
           pontos[indiceMaisDistante].z);
    printf("Distancia: %.4f\n", distancias[indiceMaisDistante]);

    printf("\nPrimeiras 5 distancias:\n");
    for (size_t i = 0; i < 5 && i < n; i++) {
        printf("Ponto %zu: %.4f\n", i + 1, distancias[i]);
    }

    printf("Quantidade de pontos: %zu\n", n);
    printf("Ponto de referencia: (%.2f, %.2f, %.2f)\n",
           referencia.x, referencia.y, referencia.z);
    printf("Primeiro ponto: (%.2f, %.2f, %.2f)\n",
           pontos[0].x, pontos[0].y, pontos[0].z);

    if (mostrar_distancias) {
        fprintf(stderr, "Distancias calculadas (uma por linha):\n");
        for (size_t i = 0; i < n; i++) {
            fprintf(stderr, "%.12f\n", distancias[i]);
        }
    }

    free(threads);
    free(tarefas);
    free(pontos);
    free(distancias);
    return EXIT_SUCCESS;
}
