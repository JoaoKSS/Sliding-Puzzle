/**
 * ida_estrela.cpp - Implementacao do IDA* (Korf, 1985)
 *
 * Busca em profundidade com limiar crescente de f(n)=g(n)+h(n).
 * A cada iteracao, o limiar eh o menor f que excedeu o limiar anterior.
 * Memoria O(d), ideal para puzzles grandes como o 15-puzzle.
 *
 * Heuristica: Manhattan + Conflitos Lineares (Hansson et al., 1992)
 * Usa timeout por instancia para evitar loops.
 */

#include "ida_estrela.h"
#include <chrono>
#include <algorithm>
#include <climits>

using namespace std;

// Valor sentinela: solucao encontrada
static const int ENCONTRADO = -1;

// Contadores globais para a recursao
static long long g_estados_avaliados;
static double g_timeout_segundos;
static bool g_timeout_atingido;
static chrono::high_resolution_clock::time_point g_inicio;

static void verificarTimeout() {
    if (g_estados_avaliados % 100000 == 0) {
        auto agora = chrono::high_resolution_clock::now();
        double elapsed = chrono::duration<double>(agora - g_inicio).count();
        if (elapsed > g_timeout_segundos) {
            g_timeout_atingido = true;
        }
    }
}

// Busca recursiva em profundidade com corte por limiar.

static int busca(Estado& atual, int limiar, vector<Estado>& caminho) {
    int f_val = atual.f();
    if (f_val > limiar) return f_val;

    if (atual.ehObjetivo()) {
        caminho.push_back(atual);
        return ENCONTRADO;
    }

    g_estados_avaliados++;
    verificarTimeout();
    if (g_timeout_atingido) return INT_MAX;

    int minimo = INT_MAX;
    Direcao direcoes[] = {CIMA, BAIXO, ESQUERDA, DIREITA};

    for (Direcao d : direcoes) {
        if (d == direcaoOposta(atual.ultimo_movimento)) continue;

        Estado filho;
        if (!atual.mover(d, filho)) continue;

        int t = busca(filho, limiar, caminho);

        if (t == ENCONTRADO) {
            caminho.push_back(atual);
            return ENCONTRADO;
        }

        if (g_timeout_atingido) return INT_MAX;

        if (t < minimo) minimo = t;
    }

    return minimo;
}

ResultadoIDAEstrela buscarIDAEstrela(const Estado& estado_inicial, int /*limite_estados*/, double timeout_segundos) {
    ResultadoIDAEstrela resultado;
    resultado.solucao_encontrada = false;
    resultado.custo_solucao = -1;
    resultado.estados_avaliados = 0;
    resultado.tempo_execucao = 0.0;

    if (!estado_inicial.temSolucao()) {
        resultado.mensagem = "Instancia sem solucao (inversoes invalidas).";
        return resultado;
    }

    if (estado_inicial.ehObjetivo()) {
        resultado.solucao_encontrada = true;
        resultado.custo_solucao = 0;
        resultado.caminho.push_back(estado_inicial);
        resultado.mensagem = "Estado inicial ja eh o objetivo.";
        return resultado;
    }

    g_inicio = chrono::high_resolution_clock::now();
    g_estados_avaliados = 0;
    g_timeout_segundos = timeout_segundos;
    g_timeout_atingido = false;

    int limiar = estado_inicial.heuristica;
    Estado raiz = estado_inicial;

    while (true) {
        vector<Estado> caminho;
        raiz.custo_g = 0;

        int t = busca(raiz, limiar, caminho);

        if (t == ENCONTRADO) {
            auto fim = chrono::high_resolution_clock::now();
            resultado.tempo_execucao = chrono::duration<double>(fim - g_inicio).count();
            resultado.solucao_encontrada = true;
            resultado.estados_avaliados = g_estados_avaliados;

            reverse(caminho.begin(), caminho.end());
            resultado.caminho = caminho;
            resultado.custo_solucao = (int)caminho.size() - 1;
            resultado.mensagem = "Solucao otima encontrada.";
            return resultado;
        }

        if (g_timeout_atingido || t == INT_MAX) {
            auto fim = chrono::high_resolution_clock::now();
            resultado.tempo_execucao = chrono::duration<double>(fim - g_inicio).count();
            resultado.estados_avaliados = g_estados_avaliados;
            resultado.mensagem = "Timeout atingido (" + to_string((int)timeout_segundos) + "s, " + to_string(g_estados_avaliados) + " estados).";
            return resultado;
        }

        limiar = t;
    }
}
