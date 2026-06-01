# Sliding Puzzle

Aplicação de linha de comando em C++ que resolve o **8‑puzzle** e o **15‑puzzle** usando os algoritmos **A*​** e **IDA*​**. O programa permite escolher a heurística a ser usada:

- `nenhuma` – sem heurística (h = 0), busca de custo uniforme.
- `manhattan` – soma das distâncias de Manhattan de cada peça.
- `conflitos` – distância de Manhattan **mais** conflito linear (padrão).

## Funcionalidades

- Inferência automática do tamanho do tabuleiro a partir do arquivo de instâncias (não é necessário informar o tamanho).
- Seleção independente do algoritmo (`a_estrela`, `ida_estrela` ou `ambos`).
- Escolha da heurística em tempo de execução.
- Timeout por instância para evitar execuções intermináveis.
- Saídas estruturadas em arquivos e tabelas de resumo.

## Pré‑requisitos

- Compilador compatível com C++17 (testado com `g++`).
- `make` (opcional, o `Makefile` fornece a compilação automática).

## Compilação

```bash
make
```

## Uso

```bash
./puzzle <arquivo_instancias> <algoritmo> [heuristica] [timeout_s]
```

- `<arquivo_instancias>` – caminho para um arquivo de texto contendo uma instância por linha.
- `<algoritmo>` – `a_estrela`, `ida_estrela` ou `ambos`.
- `[heuristica]` – opcional; `nenhuma`, `manhattan` ou `conflitos` (padrão).
- `[timeout_s]` – opcional, limite de tempo por instância (padrão 300 s).

### Exemplos

```bash
# Resolver todas as instâncias usando ambos algoritmos com a heurística padrão (conflitos)
./puzzle 8puzzle_instances.txt ambos

# Resolver apenas IDA* com distância de Manhattan e timeout de 60 s
./puzzle 15puzzle_instances.txt ida_estrela manhattan 60

# Resolver apenas A* sem heurística (busca de custo uniforme) e timeout de 120 s
./puzzle 8puzzle_instances.txt a_estrela nenhuma 120

# Resolver ambos algoritmos usando a heurística de conflitos explicitamente
./puzzle 15puzzle_instances.txt ambos conflitos 180
```

## Detalhes da Heurística

A heurística é definida pelo membro estático `Estado::tipo_heuristica`. Tanto **A*​** quanto **IDA*​** chamam `Estado::calcularHeuristica()`, portanto a heurística escolhida vale para ambos os algoritmos.

- **Nenhuma** – `h(n) = 0`. Converte a busca em custo uniforme.
- **Manhattan** – admissível, soma a distância de Manhattan de cada peça até sua posição objetivo.
- **Conflitos** – distância de Manhattan mais penalidade de conflito linear (mais informada, ainda admissível).

## Saída

Os resultados são gravados em `saidas/`:

```
saidas/<puzzle>_a_estrela_<heuristica>/instancia_<n>.txt
saidas/<puzzle>_ida_estrela_<heuristica>/instancia_<n>.txt
```

Cada arquivo contém:

- Estado inicial e estado objetivo.
- Status do resolvedor, custo da solução, número de estados avaliados e tempo de execução.
- Lista passo a passo dos movimentos (quando houver solução).

As tabelas de resumo (`resumo.txt`) listam essas métricas para todas as instâncias.
