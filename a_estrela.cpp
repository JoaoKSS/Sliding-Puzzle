/**
 * a_estrela.cpp - Implementação do algoritmo A*
 *
 * Implementação clássica do A* (Hart, Nilsson & Raphael, 1968)
 * com fila de prioridade (min-heap) e conjunto fechado (hash set).
 *
 * Heurística: Manhattan + Conflitos Lineares (Hansson et al., 1992),
 * admissível e consistente, garantindo optimalidade.
 *
 * Usa timeout por instância para evitar loops.
 */

#include "a_estrela.h"
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <algorithm>

using namespace std;

// Estrutura para a fila de prioridade
struct NoFila {
    int indice;
    int f_valor;

    bool operator>(const NoFila& outro) const {
        return f_valor > outro.f_valor;
    }
};

ResultadoAEstrela buscarAEstrela(const Estado& estado_inicial, double timeout_segundos) {
    ResultadoAEstrela resultado;
    resultado.solucao_encontrada = false;
    resultado.custo_solucao = -1;
    resultado.estados_avaliados = 0;
    resultado.tempo_execucao = 0.0;

    if (estado_inicial.ehObjetivo()) {
        resultado.solucao_encontrada = true;
        resultado.custo_solucao = 0;
        resultado.caminho.push_back(estado_inicial);
        resultado.mensagem = "Estado inicial ja eh o objetivo.";
        return resultado;
    }

    auto inicio = chrono::high_resolution_clock::now();

    // Vetor de todos os estados para reconstrução do caminho
    vector<Estado> todos_estados;
    todos_estados.reserve(500000);

    // Fila de prioridade (min-heap por f(n))
    priority_queue<NoFila, vector<NoFila>, greater<NoFila>> abertos;

    // Conjunto fechado
    unordered_set<uint64_t> fechados;

    // Melhor g conhecido para cada hash
    unordered_map<uint64_t, int> melhor_g;

    // Inserir estado inicial
    Estado inicial = estado_inicial;
    inicial.custo_g = 0;
    inicial.pai_idx = -1;
    todos_estados.push_back(inicial);
    abertos.push({0, inicial.f()});
    melhor_g[inicial.hash()] = 0;

    while (!abertos.empty()) {
        // Verificar timeout periodicamente
        if (resultado.estados_avaliados % 10000 == 0) {
            auto agora = chrono::high_resolution_clock::now();
            double elapsed = chrono::duration<double>(agora - inicio).count();
            if (elapsed > timeout_segundos) {
                resultado.tempo_execucao = elapsed;
                resultado.mensagem = "Timeout atingido (" + to_string((int)timeout_segundos) + "s, " + to_string(resultado.estados_avaliados) + " estados).";
                return resultado;
            }
        }

        NoFila topo = abertos.top();
        abertos.pop();

        int idx = topo.indice;
        Estado& atual = todos_estados[idx];

        uint64_t hash_atual = atual.hash();

        if (fechados.count(hash_atual)) continue;
        fechados.insert(hash_atual);

        resultado.estados_avaliados++;

        // Verificar se é objetivo
        if (atual.ehObjetivo()) {
            auto fim = chrono::high_resolution_clock::now();
            resultado.tempo_execucao = chrono::duration<double>(fim - inicio).count();
            resultado.solucao_encontrada = true;
            resultado.custo_solucao = atual.custo_g;

            // Reconstruir caminho
            vector<Estado> caminho;
            int i = idx;
            while (i != -1) {
                caminho.push_back(todos_estados[i]);
                i = todos_estados[i].pai_idx;
            }
            reverse(caminho.begin(), caminho.end());
            resultado.caminho = caminho;
            resultado.mensagem = "Solucao otima encontrada.";
            return resultado;
        }

        // Expandir filhos
        vector<Estado> filhos = atual.gerarFilhos();
        for (Estado& filho : filhos) {
            uint64_t hash_filho = filho.hash();

            if (fechados.count(hash_filho)) continue;

            auto it = melhor_g.find(hash_filho);
            if (it != melhor_g.end() && it->second <= filho.custo_g) continue;

            melhor_g[hash_filho] = filho.custo_g;

            filho.pai_idx = idx;
            int novo_idx = (int)todos_estados.size();
            todos_estados.push_back(filho);
            abertos.push({novo_idx, filho.f()});
        }
    }

    auto fim = chrono::high_resolution_clock::now();
    resultado.tempo_execucao = chrono::duration<double>(fim - inicio).count();
    resultado.mensagem = "Nenhuma solucao encontrada (espaco esgotado).";
    return resultado;
}
