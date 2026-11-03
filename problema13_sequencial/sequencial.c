#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct {
    double x;
    double y;
    double z;
} Ponto;

double calcularDistancia(Ponto referencia, Ponto ponto) {
    double dx = ponto.x - referencia.x;
    double dy = ponto.y - referencia.y;
    double dz = ponto.z - referencia.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
}

int main() {
    FILE *arquivo;
    int quantidadePontos;
    Ponto referencia;
    Ponto *pontos;
    double *distancias;

    struct timespec inicio, fim;

    const char *nomeArquivo = "entradas/grande.txt";
    arquivo = fopen(nomeArquivo, "r");

    printf("Arquivo em análise: %s\n", nomeArquivo);

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo\n");
        return 1;
    }

    if (fscanf(arquivo, "%d", &quantidadePontos) != 1 || quantidadePontos <= 0) {
        printf("Quantidade de pontos inválida\n");
        fclose(arquivo);
        return 1;
    }

    if (fscanf(arquivo, "%lf %lf %lf", &referencia.x, &referencia.y, &referencia.z) != 3) {
        printf("Erro ao ler o ponto de referência\n");
        fclose(arquivo);
        return 1;
    }

    pontos = malloc(quantidadePontos * sizeof(Ponto));

    if (pontos == NULL) {
        printf("Erro ao alocar memória para os pontos\n");
        fclose(arquivo);
        return 1;
    }

    for (int i = 0; i < quantidadePontos; i++) {
        if (fscanf(arquivo, "%lf %lf %lf", &pontos[i].x, &pontos[i].y, &pontos[i].z) != 3) {
            printf("Erro ao ler o ponto %d\n", i + 1);
            free(pontos);
            fclose(arquivo);
            return 1;
        }
    }

    distancias = malloc(quantidadePontos * sizeof(double));

    if (distancias == NULL) {
        printf("Erro ao alocar memória para as distâncias\n");
        free(pontos);
        fclose(arquivo);
        return 1;
    }


    // LÓGICA DE CÁLCULO DAS DISTÂNCIAS =============================
    
    // Registra o horário antes da execução do cálculo
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (int i = 0; i < quantidadePontos; i++) {
        distancias[i] = calcularDistancia(referencia, pontos[i]);
    }
 
    // Registra o horário após a execução do cálculo
    clock_gettime(CLOCK_MONOTONIC, &fim);

    double tempoMs = (fim.tv_sec - inicio.tv_sec) * 1000.0
               + (fim.tv_nsec - inicio.tv_nsec) / 1000000.0;

    printf("Tempo de execucao: %.9f ms\n", tempoMs);

    

    
    // LÓGICA DE CÁLCULO DO PONTO MAIS PRÓXIMO E MAIS DISTANTE ======
    int indiceMaisProximo = 0;
    int indiceMaisDistante = 0;

    for (int i = 1; i < quantidadePontos; i++) {
        if (distancias[i] < distancias[indiceMaisProximo]) {
            indiceMaisProximo = i;
        }

        if (distancias[i] > distancias[indiceMaisDistante]) {
            indiceMaisDistante = i;
        }
    }

    printf("Ponto mais proximo: %d\n", indiceMaisProximo + 1);
    printf("Coordenadas: (%.4f, %.4f, %.4f)\n",
        pontos[indiceMaisProximo].x,
        pontos[indiceMaisProximo].y,
        pontos[indiceMaisProximo].z);
    printf("Distancia: %.4f\n", distancias[indiceMaisProximo]);

    printf("\nPonto mais distante: %d\n", indiceMaisDistante + 1);
    printf("Coordenadas: (%.4f, %.4f, %.4f)\n",
        pontos[indiceMaisDistante].x,
        pontos[indiceMaisDistante].y,
        pontos[indiceMaisDistante].z);
    printf("Distancia: %.4f\n", distancias[indiceMaisDistante]);

    // ==============================================================


    printf("\nPrimeiras 5 distancias:\n");

    for (int i = 0; i < 5 && i < quantidadePontos; i++) {
        printf("Ponto %d: %.4f\n", i + 1, distancias[i]);
    }

    free(distancias);

    printf("Quantidade de pontos: %d\n", quantidadePontos);
    printf("Ponto de referencia: (%.2f, %.2f, %.2f)\n", referencia.x, referencia.y, referencia.z );
    printf("Primeiro ponto: (%.2f, %.2f, %.2f)\n", pontos[0].x, pontos[0].y, pontos[0].z);

    fclose(arquivo);
    free(pontos);

    return 0;
}
