#ifndef A_ESTRELA_H
#define A_ESTRELA_H

#include "estado.h"
#include <vector>
#include <string>

using namespace std;

struct ResultadoAEstrela {
    bool solucao_encontrada;
    int custo_solucao;
    long long estados_avaliados;
    double tempo_execucao;
    vector<Estado> caminho;
    string mensagem;
};

ResultadoAEstrela buscarAEstrela(const Estado& estado_inicial, double timeout_segundos = 120.0);

#endif // A_ESTRELA_H
