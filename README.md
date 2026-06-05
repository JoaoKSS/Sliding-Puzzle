# Sliding Puzzle Solver

![C++](https://img.shields.io/badge/C%2B%2B-17-blue)
![Python](https://img.shields.io/badge/Python-%3E%3D3.8-blue)
![Build](https://img.shields.io/badge/Build-Makefile-orange)
![Status](https://img.shields.io/badge/Status-Completed-brightgreen)

Esta aplicação de linha de comando foi desenvolvida em C++17 com o objetivo de solucionar os problemas do 8-puzzle e 15-puzzle. O sistema implementa os algoritmos de busca heurística A* e IDA*, permitindo a avaliação comparativa de desempenho sob diferentes heurísticas de estimativa de custo.

---

## Sumário

- [Descrição Geral](#sliding-puzzle-solver)
- [Funcionalidades](#funcionalidades)
- [Compilação](#compilação)
- [Instruções de Uso](#instruções-de-uso)
- [Estrutura das Saídas](#estrutura-das-saídas)
- [Análise de Dados e Gráficos](#análise-de-dados-e-gráficos)

---

## Funcionalidades

- **Inferência Dinâmica de Tabuleiro:** Identificação automática da dimensão do problema ($3\times3$ para o 8-puzzle ou $4\times4$ para o 15-puzzle) a partir do arquivo de entrada, dispensando parametrização manual de tamanho.
- **Controle de Execução (Timeout):** Suporte à definição de limite de tempo por instância para evitar loops ou consumo excessivo de processamento.
- **Opções de Algoritmo:** Execução independente ou paralela do A*, IDA* ou ambos.
- **Mapeamento de Heurísticas:**
  - `nenhuma`: Busca de custo uniforme (heurística nula, $h(n) = 0$).
  - `manhattan`: Soma das distâncias de Manhattan individuais de cada peça até sua respectiva posição final.
  - `conflitos`: Distância de Manhattan acrescida da penalidade por conflitos lineares (heurística admissível e dominante).
- **Exportação de Logs:** Gravação detalhada do caminho da solução em arquivos individuais e tabelas de síntese de desempenho.

---

## Compilação

### Pré-requisitos
- Compilador compatível com a linguagem C++17 (como o `g++`).
- Utilitário GNU `make`.

### Executando a Compilação
Execute o comando abaixo no diretório raiz do repositório para compilar o executável `puzzle`:
```bash
make
```

Para remover os arquivos compilados e os arquivos de objetos temporários:
```bash
make clean
```

---

## Instruções de Uso

O programa deve ser executado via terminal passando os parâmetros conforme a sintaxe a seguir:

```bash
./puzzle <arquivo_instancias> <algoritmo> [heuristica] [timeout_s]
```

### Detalhamento dos Parâmetros
- `<arquivo_instancias>`: Caminho do arquivo de texto contendo os estados iniciais das instâncias (valores separados por espaço, onde `0` denota a lacuna).
- `<algoritmo>`: Algoritmo de busca a ser empregado (`a_estrela`, `ida_estrela` ou `ambos`).
- `[heuristica]`: (Opcional) Heurística de busca a ser aplicada (`nenhuma`, `manhattan` ou `conflitos` [padrão]).
- `[timeout_s]`: (Opcional) Limite máximo de tempo, em segundos, para a resolução de cada instância (padrão: `300` segundos).

### Exemplos de Execução
```bash
# Executar ambos os algoritmos em todas as instâncias do 8-puzzle usando a heurística padrão
./puzzle 8puzzle_instances.txt ambos

# Executar o IDA* com Manhattan simples e limite de 60 segundos por instância no 15-puzzle
./puzzle 15puzzle_instances.txt ida_estrela manhattan 60

# Executar o A* sem heurística (busca de custo uniforme) com limite de 120 segundos
./puzzle 8puzzle_instances.txt a_estrela nenhuma 120
```

---

## Estrutura das Saídas

Os logs e relatórios da execução do programa são gravados sob o diretório `saidas/`:

```
saidas/
├── 8puzzle_a_estrela_conflitos/
│   ├── resumo.txt
│   ├── instancia_1.txt
│   └── ...
└── 15puzzle_ida_estrela_manhattan/
    ├── resumo.txt
    ├── instancia_1.txt
    └── ...
```

Cada arquivo de log de instância (`instancia_n.txt`) apresenta:
1. O estado de tabuleiro inicial e objetivo.
2. O status final do processamento (Solucionado / Timeout / Erro).
3. O custo ótimo encontrado (número de movimentos da solução).
4. O número total de estados avaliados.
5. O tempo real de execução computado em segundos.
6. A sequência detalhada de movimentos das peças.

O arquivo `resumo.txt` em cada diretório traz uma tabela consolidada de todas as instâncias executadas naquele cenário.

---

## Análise de Dados e Gráficos

O projeto inclui ferramentas escritas em Python 3 para automação do processamento das tabelas geradas e renderização dos gráficos de análise.

### 1. Conversão para Planilhas Excel (.xlsx)
O script `gerar_planilha.py` analisa o log `resumo.txt` gerado e cria planilhas Excel formatadas:
```bash
python3 gerar_planilha.py <caminho_do_resumo.txt> <nome_da_planilha.xlsx>
```

### 2. Geração de Gráficos Comparativos
Para plotar as curvas de Estados Avaliados, Tempo de Execução e Perfil de Performance/Sobrevivência:
- **15-Puzzle:** Execute `python3 gerar_grafico.py` (consome as planilhas do 15-puzzle).
- **8-Puzzle:** Execute `python3 gerar_grafico_8puzzle.py` (consome as planilhas do 8-puzzle).

Os gráficos gerados são salvos diretamente em formato PNG na pasta raiz do repositório.
