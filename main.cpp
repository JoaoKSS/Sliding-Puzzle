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
#include <cmath>

using namespace std;

void criarDiretorio(const std::string& caminho) {
    std::filesystem::create_directories(caminho);
}

vector<vector<uint8_t>> lerInstancias(const string& arquivo) {
    vector<vector<uint8_t>> instancias;
    ifstream fin(arquivo);
    if (!fin.is_open()) {
        cerr << "Erro ao abrir arquivo: " << arquivo << endl;
        return instancias;
    }

    int expected_size = 0;
    string linha;
    while (getline(fin, linha)) {
        if (linha.empty()) continue;
        istringstream iss(linha);
        vector<uint8_t> tab;
        int val;
        while (iss >> val) {
            tab.push_back((uint8_t)val);
        }
        if (expected_size == 0 && tab.size() > 0) {
            expected_size = tab.size();
        }
        if ((int)tab.size() == expected_size) {
            instancias.push_back(tab);
        } else if (tab.size() > 0) {
            cerr << "Aviso: Linha ignorada pois o tamanho (" << tab.size() 
                 << ") difere do esperado (" << expected_size << ")." << endl;
        }
    }
    fin.close();
    return instancias;
}

void escreverResultado(const string& arquivo, int num_instancia, const vector<uint8_t>& tab_original, int tamanho, bool eh_a_estrela,
                       const string& heuristica_str,
                       bool solucao_encontrada, int custo, long long estados,
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
    fout << "  Heuristica: " << heuristica_str << endl;
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
    if (argc < 3) {
        cout << "Uso: ./puzzle <arquivo_instancias> <algoritmo> [heuristica] [timeout_s]" << endl;
        cout << "  algoritmo: a_estrela, ida_estrela, ambos" << endl;
        cout << "  heuristica: nenhuma, manhattan, conflitos (padrao: conflitos)" << endl;
        cout << "  timeout_s: limite de tempo por instancia (padrao: 300s)" << endl;
        cout << endl;
        cout << "Exemplo:" << endl;
        cout << "  ./puzzle 8puzzle_instances.txt ambos" << endl;
        cout << "  ./puzzle 15puzzle_instances.txt ambos manhattan 60" << endl;
        return 1;
    }

    string arquivo = argv[1];
    string algoritmo = argv[2];
    
    string heuristica_str = "conflitos";
    double timeout = 300;
    
    if (argc >= 4) {
        string arg3 = argv[3];
        if (arg3 == "nenhuma" || arg3 == "manhattan" || arg3 == "conflitos") {
            heuristica_str = arg3;
            if (argc >= 5) {
                timeout = stod(argv[4]);
            }
        } else {
            timeout = stod(arg3);
        }
    }
    
    if (heuristica_str == "nenhuma") Estado::tipo_heuristica = H_NENHUMA;
    else if (heuristica_str == "manhattan") Estado::tipo_heuristica = H_MANHATTAN;
    else Estado::tipo_heuristica = H_CONFLITOS;

    // Ler instâncias primeiro para deduzir o tamanho
    vector<vector<uint8_t>> instancias = lerInstancias(arquivo);
    if (instancias.empty()) {
        cerr << "Nenhuma instancia valida encontrada no arquivo." << endl;
        return 1;
    }

    int n_pecas = instancias[0].size();
    int tamanho = round(sqrt(n_pecas));

    if (tamanho * tamanho != n_pecas) {
        cerr << "Erro: Número de peças (" << n_pecas << ") não forma um tabuleiro quadrado perfeito." << endl;
        return 1;
    }

    string nome_puzzle = (tamanho == 3) ? "8puzzle" : "15puzzle";

    bool rodar_a = (algoritmo == "a_estrela" || algoritmo == "ambos");
    bool rodar_ida = (algoritmo == "ida_estrela" || algoritmo == "ambos");

    // Cria diretórios de saída
    criarDiretorio("saidas");
    string dir_a = "saidas/" + nome_puzzle + "_a_estrela_" + heuristica_str;
    string dir_ida = "saidas/" + nome_puzzle + "_ida_estrela_" + heuristica_str;
    if (rodar_a)   criarDiretorio(dir_a);
    if (rodar_ida) criarDiretorio(dir_ida);

    cout << "Instancias lidas: " << instancias.size() << endl;
    cout << "Puzzle: " << nome_puzzle << endl;
    cout << "Algoritmo(s): " << algoritmo << endl;
    cout << "Heuristica: " << heuristica_str << endl;
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
            escreverResultado(arq_saida, num, instancias[i], tamanho, true, heuristica_str,
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
            escreverResultado(arq_saida, num, instancias[i], tamanho, false, heuristica_str,
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
