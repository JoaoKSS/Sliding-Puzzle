/**
 * estado.h - Representação do estado do puzzle (8-puzzle e 15-puzzle)
 *
 * Referências:
 *   - Korf, R.E. (1985). "Depth-First Iterative-Deepening: An Optimal
 *     Admissible Tree Search". Artificial Intelligence, 27(1), 97-109.
 *   - Hansson, O., Mayer, A., Yung, M. (1992). "Criticizing Solutions to
 *     Relaxed Models Yields Powerful Admissible Heuristics".
 *
 * Estado objetivo:
 *   8-puzzle:  0 1 2 3 4 5 6 7 8
 *   15-puzzle: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
 *   (0 = espaço vazio na posição [0][0], restante ordenado)
 *
 * Heurística: Manhattan + Conflitos Lineares (admissível e consistente).
 */

#ifndef ESTADO_H
#define ESTADO_H

#include <vector>
#include <string>
#include <cstdint>

using namespace std;

// Direções de movimento do espaço vazio
enum Direcao {
    CIMA = 0,
    BAIXO,
    ESQUERDA,
    DIREITA,
    NENHUMA
};

enum TipoHeuristica {
    H_NENHUMA = 0,
    H_MANHATTAN,
    H_CONFLITOS
};

Direcao direcaoOposta(Direcao d);
string nomeDirecao(Direcao d);

class Estado {
public:
    static TipoHeuristica tipo_heuristica;
    uint8_t tabuleiro[16]; 
    int tamanho;
    int n_pecas;
    int pos_vazio;
    int heuristica; // h(n) dependente da escolha
    int custo_g; // g(n)
    Direcao ultimo_movimento;
    int pai_idx;

    Estado();
    Estado(const vector<uint8_t>& tab, int tam);

    // Calcula h(n) = Manhattan + Conflitos Lineares
    void calcularHeuristica();

    // Manhattan de uma peça na posição pos
    int manhattanPeca(int pos, uint8_t peca) const;

    // Conflitos lineares
    int calcularConflitosLineares() const;

    bool ehObjetivo() const;

    vector<Estado> gerarFilhos() const;
    bool mover(Direcao d, Estado& novo) const;

    int f() const;
    bool operator>(const Estado& outro) const;
    bool operator==(const Estado& outro) const;
    uint64_t hash() const;
    string paraString() const;
};

#endif // ESTADO_H
