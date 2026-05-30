/**
 * estado.cpp - Implementação do estado do puzzle
 *
 * Estado objetivo: 0 1 2 3 4 5 6 7 8 (8-puzzle)
 *                  0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 (15-puzzle)
 * Onde 0 = espaço vazio na posição [0][0].
 *
 * Peça p tem posição objetivo = p (posição linear).
 *
 * Heurística: Manhattan + Conflitos Lineares (Hansson et al., 1992)
 */

#include "estado.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

// Funções auxiliares de direção
Direcao direcaoOposta(Direcao d) {
    switch (d) {
        case CIMA:     return BAIXO;
        case BAIXO:    return CIMA;
        case ESQUERDA: return DIREITA;
        case DIREITA:  return ESQUERDA;
        default:       return NENHUMA;
    }
}

string nomeDirecao(Direcao d) {
    switch (d) {
        case CIMA:     return "Cima";
        case BAIXO:    return "Baixo";
        case ESQUERDA: return "Esquerda";
        case DIREITA:  return "Direita";
        default:       return "Nenhuma";
    }
}

// Construtores
Estado::Estado()
    : tamanho(0), n_pecas(0), pos_vazio(-1), heuristica(0),
      custo_g(0), ultimo_movimento(NENHUMA), pai_idx(-1) {}

Estado::Estado(const vector<uint8_t>& tab, int tam)
    : tabuleiro(tab), tamanho(tam), n_pecas(tam * tam),
      custo_g(0), ultimo_movimento(NENHUMA), pai_idx(-1) {
    for (int i = 0; i < n_pecas; i++) {
        if (tabuleiro[i] == 0) {
            pos_vazio = i;
            break;
        }
    }
    calcularHeuristica();
}

// Heurística: Manhattan + Conflitos Lineares

/**
 * Posição objetivo da peça p é a própria posição p.
 * Objetivo: 0 1 2 3 4 5 6 7 8 (8-puzzle)
 *           0 1 2 ... 15 (15-puzzle)
 */
int Estado::manhattanPeca(int pos, uint8_t peca) const {
    if (peca == 0) return 0;
    // Posição objetivo da peça p é p
    int obj = (int)peca;
    int linha_atual = pos / tamanho;
    int col_atual = pos % tamanho;
    int linha_obj = obj / tamanho;
    int col_obj = obj % tamanho;
    return abs(linha_atual - linha_obj) + abs(col_atual - col_obj);
}

/**
 * Conflitos Lineares (Hansson, Mayer & Yung, 1992):
 *
 * Duas peças tj e tk estão em conflito linear se:
 *   1) Ambas estão na mesma linha (ou coluna)
 *   2) A posição objetivo de ambas é nessa mesma linha (ou coluna)
 *   3) Estão em ordem invertida em relação ao objetivo
 *
 * Cada conflito requer pelo menos 2 movimentos adicionais.
 */
int Estado::calcularConflitosLineares() const {
    int conflitos = 0;

    // Conflitos em linhas
    for (int linha = 0; linha < tamanho; linha++) {
        for (int j = 0; j < tamanho; j++) {
            int pos_j = linha * tamanho + j;
            uint8_t peca_j = tabuleiro[pos_j];
            if (peca_j == 0) continue;

            // Posição objetivo: peca_j
            int linha_obj_j = (int)peca_j / tamanho;
            if (linha_obj_j != linha) continue;

            for (int k = j + 1; k < tamanho; k++) {
                int pos_k = linha * tamanho + k;
                uint8_t peca_k = tabuleiro[pos_k];
                if (peca_k == 0) continue;

                int linha_obj_k = (int)peca_k / tamanho;
                if (linha_obj_k != linha) continue;

                // j < k mas objetivo de peca_j > objetivo de peca_k → conflito
                int col_obj_j = (int)peca_j % tamanho;
                int col_obj_k = (int)peca_k % tamanho;
                if (col_obj_j > col_obj_k) {
                    conflitos++;
                }
            }
        }
    }

    // Conflitos em colunas
    for (int col = 0; col < tamanho; col++) {
        for (int j = 0; j < tamanho; j++) {
            int pos_j = j * tamanho + col;
            uint8_t peca_j = tabuleiro[pos_j];
            if (peca_j == 0) continue;

            int col_obj_j = (int)peca_j % tamanho;
            if (col_obj_j != col) continue;

            for (int k = j + 1; k < tamanho; k++) {
                int pos_k = k * tamanho + col;
                uint8_t peca_k = tabuleiro[pos_k];
                if (peca_k == 0) continue;

                int col_obj_k = (int)peca_k % tamanho;
                if (col_obj_k != col) continue;

                int linha_obj_j = (int)peca_j / tamanho;
                int linha_obj_k = (int)peca_k / tamanho;
                if (linha_obj_j > linha_obj_k) {
                    conflitos++;
                }
            }
        }
    }

    return 2 * conflitos;
}

void Estado::calcularHeuristica() {
    int manhattan = 0;
    for (int i = 0; i < n_pecas; i++) {
        manhattan += manhattanPeca(i, tabuleiro[i]);
    }
    heuristica = manhattan + calcularConflitosLineares();
}

// Verificações

/**
 * Objetivo: tabuleiro[i] == i para todo i.
 * Ou seja: 0 1 2 3 4 5 6 7 8 (8-puzzle)
 */
bool Estado::ehObjetivo() const {
    for (int i = 0; i < n_pecas; i++) {
        if (tabuleiro[i] != (uint8_t)i) return false;
    }
    return true;
}

/**
 * Verifica se o puzzle é solucionável.
 *
 * Com objetivo tendo 0 na posição [0][0]:
 * - 8-puzzle (N ímpar): solucionável se inversões é par.
 * - 15-puzzle (N par): solucionável se (inversões + linha_do_vazio) é par.
 *   (linha_do_vazio contada de cima, 0-indexada)
 */
bool Estado::temSolucao() const {
    int inversoes = 0;
    for (int i = 0; i < n_pecas; i++) {
        if (tabuleiro[i] == 0) continue;
        for (int j = i + 1; j < n_pecas; j++) {
            if (tabuleiro[j] == 0) continue;
            if (tabuleiro[i] > tabuleiro[j]) {
                inversoes++;
            }
        }
    }

    if (tamanho % 2 == 1) {
        // N ímpar (8-puzzle): par de inversões = solucionável
        return (inversoes % 2 == 0);
    } else {
        // N par (15-puzzle): (inversões + linha do vazio) deve ser par
        int linha_vazio = pos_vazio / tamanho;
        return ((inversoes + linha_vazio) % 2 == 0);
    }
}

// Geração de filhos
vector<Estado> Estado::gerarFilhos() const {
    vector<Estado> filhos;
    filhos.reserve(4);

    Direcao direcoes[] = {CIMA, BAIXO, ESQUERDA, DIREITA};
    for (Direcao d : direcoes) {
        if (d == direcaoOposta(ultimo_movimento)) continue;
        Estado filho;
        if (mover(d, filho)) {
            filhos.push_back(filho);
        }
    }
    return filhos;
}

bool Estado::mover(Direcao d, Estado& novo) const {
    int linha = pos_vazio / tamanho;
    int col = pos_vazio % tamanho;
    int nova_linha = linha, nova_col = col;

    switch (d) {
        case CIMA:     nova_linha--; break;
        case BAIXO:    nova_linha++; break;
        case ESQUERDA: nova_col--;   break;
        case DIREITA:  nova_col++;   break;
        default: return false;
    }

    if (nova_linha < 0 || nova_linha >= tamanho ||
        nova_col < 0 || nova_col >= tamanho) {
        return false;
    }

    int nova_pos = nova_linha * tamanho + nova_col;

    novo = *this;
    novo.custo_g = custo_g + 1;
    novo.ultimo_movimento = d;

    // Efetuar a troca
    novo.tabuleiro[pos_vazio] = tabuleiro[nova_pos];
    novo.tabuleiro[nova_pos] = 0;
    novo.pos_vazio = nova_pos;

    // Recalcular heurística
    novo.calcularHeuristica();

    return true;
}

// Funções de comparação e hash
int Estado::f() const {
    return custo_g + heuristica;
}

bool Estado::operator>(const Estado& outro) const {
    return f() > outro.f();
}

bool Estado::operator==(const Estado& outro) const {
    return tabuleiro == outro.tabuleiro;
}

uint64_t Estado::hash() const {
    uint64_t h = 0;
    uint64_t base = 1;
    for (int i = 0; i < n_pecas; i++) {
        h += tabuleiro[i] * base;
        base *= 31;
    }
    return h;
}

// Impressão tabuleiro

string Estado::paraString() const {
    ostringstream oss;
    for (int i = 0; i < n_pecas; i++) {
        oss << setw(3) << (int)tabuleiro[i];
        if ((i + 1) % tamanho == 0) {
            oss << "\n";
        }
    }
    return oss.str();
}
