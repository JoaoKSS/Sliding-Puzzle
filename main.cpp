/**
 * main.cpp - Programa principal para resolver 8-puzzle e 15-puzzle
 *
 * Le instancias de arquivos e executa A* e IDA* para cada uma.
 * Gera arquivos de saida separados por instancia e por algoritmo.
 *
 * Uso:
 *   ./puzzle <arquivo_instancias> <tamanho_lado> <algoritmo> [timeout_s]
 *
 * Referencia:
 *   Korf, R.E. (1985). Depth-First Iterative-Deepening.
 *   Hart, Nilsson & Raphael (1968). A Formal Basis for Heuristic Search.
 *   Hansson, Mayer & Yung (1992). Criticizing Solutions to Relaxed Models.
 */

#include "estado.h"
#include "a_estrela.h"
#include "ida_estrela.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <filesystem>

using namespace std;

void criarDiretorio(const std::string& caminho) {
    std::filesystem::create_directories(caminho);
}

vector<vector<uint8_t>> lerInstancias(const string& arquivo, int n_pecas) {
    vector<vector<uint8_t>> instancias;
    ifstream fin(arquivo);
    if (!fin.is_open()) {
        cerr << "Erro ao abrir arquivo: " << arquivo << endl;
        return instancias;
    }

    string linha;
    while (getline(fin, linha)) {
        if (linha.empty()) continue;
        istringstream iss(linha);
        vector<uint8_t> tab;
        int val;
        while (iss >> val) {
            tab.push_back((uint8_t)val);
        }
        if ((int)tab.size() == n_pecas) {
            instancias.push_back(tab);
        }
    }
    fin.close();
    return instancias;
}

void escreverResultado(const string& arquivo, int num_instancia, const vector<uint8_t>& tab_original, int tamanho, bool eh_a_estrela,
                       bool solucao_encontrada, int custo, int estados,
                       double tempo, const string& mensagem,
                       const vector<Estado>& caminho) {
    ofstream fout(arquivo);
    if (!fout.is_open()) {
        cerr << "Erro ao criar arquivo: " << arquivo << endl;
        return;
    }

    string nome_algo = eh_a_estrela ? "A*" : "IDA*";

    fout << "============================================================" << endl;
    fout << "  Algoritmo: " << nome_algo << endl;
    fout << "  Puzzle: " << (tamanho == 3 ? "8" : "15") << "-puzzle" << endl;
    fout << "  Instancia: " << num_instancia << endl;
    fout << "============================================================" << endl;
    fout << endl;

    // Estado inicial — 0 para o espaço vazio
    fout << "--- Estado Inicial ---" << endl;
    for (int i = 0; i < (int)tab_original.size(); i++) {
        fout << "  " << (int)tab_original[i];
        if ((i + 1) % tamanho == 0) fout << endl;
    }
    fout << endl;

    // Estado objetivo: 0 1 2 ... N-1
    fout << "--- Estado Objetivo ---" << endl;
    for (int i = 0; i < tamanho * tamanho; i++) {
        fout << "  " << i;
        if ((i + 1) % tamanho == 0) fout << endl;
    }
    fout << endl;

    fout << "--- Resultado ---" << endl;
    fout << "Status: " << mensagem << endl;
    fout << "Solucao encontrada: " << (solucao_encontrada ? "Sim" : "Nao") << endl;

    if (solucao_encontrada) {
        fout << "Custo da solucao (movimentos): " << custo << endl;
    }

    fout << "Estados avaliados: " << estados << endl;
    fout << "Tempo de execucao: " << tempo << " segundos" << endl;
    fout << endl;

    if (solucao_encontrada && !caminho.empty()) {
        fout << "--- Solucao Passo a Passo ---" << endl;
        for (int i = 0; i < (int)caminho.size(); i++) {
            if (i == 0) {
                fout << "Passo 0 (Inicial):" << endl;
            } else {
                fout << "Passo " << i << " - Mover espaco vazio para: "
                     << nomeDirecao(caminho[i].ultimo_movimento) << endl;
            }
            fout << caminho[i].paraString() << endl;
        }
    }

    fout << "============================================================" << endl;
    fout.close();
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "Uso: ./puzzle <arquivo_instancias> <tamanho_lado> <algoritmo> [timeout_s]" << endl;
        cout << "  tamanho_lado: 3 (8-puzzle) ou 4 (15-puzzle)" << endl;
        cout << "  algoritmo: a_estrela, ida_estrela, ambos" << endl;
        cout << "  timeout_s: limite de tempo por instancia (padrao: 30s)" << endl;
        cout << endl;
        cout << "Exemplo:" << endl;
        cout << "  ./puzzle 8puzzle_instances.txt 3 ambos" << endl;
        cout << "  ./puzzle 15puzzle_instances.txt 4 ambos 60" << endl;
        return 1;
    }

    string arquivo = argv[1];
    int tamanho = stoi(argv[2]);
    string algoritmo = argv[3];
    double timeout = (argc >= 5) ? stod(argv[4]) : 120.0;
    int n_pecas = tamanho * tamanho;

    string nome_puzzle = (tamanho == 3) ? "8puzzle" : "15puzzle";

    bool rodar_a = (algoritmo == "a_estrela" || algoritmo == "ambos");
    bool rodar_ida = (algoritmo == "ida_estrela" || algoritmo == "ambos");

    // Criar diretórios de saída
    criarDiretorio("saidas");
    string dir_a = "saidas/" + nome_puzzle + "_a_estrela";
    string dir_ida = "saidas/" + nome_puzzle + "_ida_estrela";
    if (rodar_a)   criarDiretorio(dir_a);
    if (rodar_ida) criarDiretorio(dir_ida);

    // Ler instâncias
    vector<vector<uint8_t>> instancias = lerInstancias(arquivo, n_pecas);
    cout << "Instancias lidas: " << instancias.size() << endl;
    cout << "Puzzle: " << nome_puzzle << endl;
    cout << "Algoritmo(s): " << algoritmo << endl;
    cout << "Timeout por instancia: " << timeout << "s" << endl;
    cout << endl;

    // Arquivos de resumo
    ofstream resumo_a, resumo_ida;
    if (rodar_a) {
        resumo_a.open(dir_a + "/resumo.txt");
        resumo_a << "Inst | Solucao | Custo | Estados Avaliados | Tempo (s)" << endl;
        resumo_a << "-----+---------+-------+-------------------+----------" << endl;
    }
    if (rodar_ida) {
        resumo_ida.open(dir_ida + "/resumo.txt");
        resumo_ida << "Inst | Solucao | Custo | Estados Avaliados | Tempo (s)" << endl;
        resumo_ida << "-----+---------+-------+-------------------+----------" << endl;
    }

    for (int i = 0; i < (int)instancias.size(); i++) {
        int num = i + 1;
        Estado estado(instancias[i], tamanho);

        cout << "--- Instancia " << num << " ---" << endl;

        // A*
        if (rodar_a) {
            cout << "  A*: " << flush;
            ResultadoAEstrela res = buscarAEstrela(estado, timeout);

            if (res.solucao_encontrada) {
                cout << "Solucao em " << res.custo_solucao << " movimentos, "
                     << res.estados_avaliados << " estados, "
                     << res.tempo_execucao << "s" << endl;
            } else {
                cout << res.mensagem << " (" << res.estados_avaliados
                     << " estados, " << res.tempo_execucao << "s)" << endl;
            }

            string arq_saida = dir_a + "/instancia_" + to_string(num) + ".txt";
            escreverResultado(arq_saida, num, instancias[i], tamanho, true,
                              res.solucao_encontrada, res.custo_solucao,
                              res.estados_avaliados, res.tempo_execucao,
                              res.mensagem, res.caminho);

            resumo_a << num << " | "
                     << (res.solucao_encontrada ? "Sim" : "Nao") << " | "
                     << res.custo_solucao << " | "
                     << res.estados_avaliados << " | "
                     << res.tempo_execucao << endl;
        }

        // IDA*
        if (rodar_ida) {
            cout << "  IDA*: " << flush;
            ResultadoIDAEstrela res = buscarIDAEstrela(estado, 0, timeout);

            if (res.solucao_encontrada) {
                cout << "Solucao em " << res.custo_solucao << " movimentos, "
                     << res.estados_avaliados << " estados, "
                     << res.tempo_execucao << "s" << endl;
            } else {
                cout << res.mensagem << " (" << res.estados_avaliados
                     << " estados, " << res.tempo_execucao << "s)" << endl;
            }

            string arq_saida = dir_ida + "/instancia_" + to_string(num) + ".txt";
            escreverResultado(arq_saida, num, instancias[i], tamanho, false,
                              res.solucao_encontrada, res.custo_solucao,
                              res.estados_avaliados, res.tempo_execucao,
                              res.mensagem, res.caminho);

            resumo_ida << num << " | "
                       << (res.solucao_encontrada ? "Sim" : "Nao") << " | "
                       << res.custo_solucao << " | "
                       << res.estados_avaliados << " | "
                       << res.tempo_execucao << endl;
        }
    }

    if (rodar_a)   resumo_a.close();
    if (rodar_ida) resumo_ida.close();

    cout << endl;
    cout << "Resultados salvos em saidas/" << endl;

    return 0;
}
