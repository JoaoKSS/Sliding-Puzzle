#ifndef IDA_ESTRELA_H
#define IDA_ESTRELA_H

#include "estado.h"
#include <vector>
#include <string>

using namespace std;

struct ResultadoIDAEstrela {
    bool solucao_encontrada;
    int custo_solucao;
    long long estados_avaliados;
    double tempo_execucao;
    vector<Estado> caminho;
    string mensagem;
};

ResultadoIDAEstrela buscarIDAEstrela(const Estado& estado_inicial, int limite_estados = 0, double timeout_segundos = 120.0);

#endif
