/**
 * a_estrela.h - Algoritmo A* (Hart, Nilsson & Raphael, 1968)
 *
 * Heurística: Manhattan + Conflitos Lineares (Hansson et al., 1992).
 * Usa timeout por instância para evitar loops.
 */

#ifndef A_ESTRELA_H
#define A_ESTRELA_H

#include "estado.h"
#include <vector>
#include <string>

using namespace std;

struct ResultadoAEstrela {
    bool solucao_encontrada;
    int custo_solucao;
    int estados_avaliados;
    double tempo_execucao;
    vector<Estado> caminho;
    string mensagem;
};

ResultadoAEstrela buscarAEstrela(const Estado& estado_inicial, double timeout_segundos = 120.0);

#endif // A_ESTRELA_H
