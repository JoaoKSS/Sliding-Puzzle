import pandas as pd
import matplotlib.pyplot as plt
import os

OUTPUT_DIR = "."
os.makedirs(OUTPUT_DIR, exist_ok=True)

# 1. Carregar as planilhas do 8-puzzle
print("Carregando planilhas do 8-puzzle...")
arquivos = {
    "A* (Manhattan)": "8a_estrela_manhattan.xlsx",
    "A* (Conflitos Lineares)": "8_a_estrela_conflitos.xlsx",
    "IDA* (Manhattan)": "8ida_estrela_manhattan.xlsx",
    "IDA* (Conflitos Lineares)": "8ida_estrela_conflitos.xlsx"
}

dfs = {}
for nome, path in arquivos.items():
    if os.path.exists(path):
        df = pd.read_excel(path)
        df = df[df['Solucao'].astype(str).str.lower().str.contains("sim|s")]
        df['Custo'] = pd.to_numeric(df['Custo'], errors='coerce')
        df['Estados Avaliados'] = pd.to_numeric(df['Estados Avaliados'], errors='coerce')
        df['Tempo (s)'] = pd.to_numeric(df['Tempo (s)'], errors='coerce')
        dfs[nome] = df.dropna(subset=['Custo', 'Estados Avaliados', 'Tempo (s)'])
        print(f"Carregado {nome}: {len(dfs[nome])} instâncias.")
    else:
        print(f"Erro: Planilha {path} não encontrada.")

# Estilo visual moderno
plt.rcParams['font.family'] = 'sans-serif'
plt.rcParams['font.sans-serif'] = ['DejaVu Sans', 'Arial', 'Liberation Sans']
plt.rcParams['text.color'] = '#1E293B'
plt.rcParams['axes.labelcolor'] = '#1E293B'
plt.rcParams['xtick.color'] = '#475569'
plt.rcParams['ytick.color'] = '#475569'

cores = {
    "A* (Manhattan)": "#3B82F6",
    "A* (Conflitos Lineares)": "#10B981",
    "IDA* (Manhattan)": "#F59E0B",
    "IDA* (Conflitos Lineares)": "#EF4444"
}

marcadores = {
    "A* (Manhattan)": "o",
    "A* (Conflitos Lineares)": "s",
    "IDA* (Manhattan)": "^",
    "IDA* (Conflitos Lineares)": "D"
}

fig_width, fig_height = 10, 6.5

def formatar_eixo(ax):
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.spines['left'].set_color('#E2E8F0')
    ax.spines['bottom'].set_color('#E2E8F0')
    ax.grid(True, which='both', linestyle='--', color='#E2E8F0', linewidth=0.7, alpha=0.8)
    ax.set_facecolor('#F8FAFC')

# ----------------- GRÁFICO 1: ESTADOS AVALIADOS -----------------
print("Gerando Gráfico 1 (Estados Avaliados - 8-Puzzle)...")
fig, ax = plt.subplots(figsize=(fig_width, fig_height))

for nome, df in dfs.items():
    ax.scatter(df['Custo'], df['Estados Avaliados'], color=cores[nome], alpha=0.15, s=25, label='_nolegend_')
    grouped = df.groupby('Custo')['Estados Avaliados'].median().reset_index()
    ax.plot(grouped['Custo'], grouped['Estados Avaliados'], 
             color=cores[nome], marker=marcadores[nome], linestyle='-', linewidth=2.5, 
             label=nome, markersize=8, markeredgecolor='white', markeredgewidth=1.2, alpha=0.95)

ax.set_yscale('log')
formatar_eixo(ax)
ax.set_title("Estados Avaliados vs. Custo Ótimo (8-Puzzle)", fontsize=14, fontweight='bold', pad=20, color='#0F172A')
ax.set_xlabel("Custo Ótimo da Solução (Movimentos)", fontsize=12, labelpad=12, fontweight='bold')
ax.set_ylabel("Quantidade de Estados Avaliados (Escala Logarítmica)", fontsize=12, labelpad=12, fontweight='bold')
ax.set_xticks(range(12, 30, 2))
ax.legend(loc="upper left", frameon=True, facecolor='white', edgecolor='#E2E8F0', framealpha=0.9, fontsize=10.5, shadow=True, borderpad=1)
plt.tight_layout()

caminho_estados = os.path.join(OUTPUT_DIR, "grafico_estados_8puzzle.png")
plt.savefig(caminho_estados, dpi=200, bbox_inches='tight')
plt.close()
print(f"Salvo: {caminho_estados}")

# ----------------- GRÁFICO 2: TEMPO DE EXECUÇÃO -----------------
print("Gerando Gráfico 2 (Tempo de Execução - 8-Puzzle)...")
fig, ax = plt.subplots(figsize=(fig_width, fig_height))

for nome, df in dfs.items():
    ax.scatter(df['Custo'], df['Tempo (s)'], color=cores[nome], alpha=0.15, s=25, label='_nolegend_')
    grouped = df.groupby('Custo')['Tempo (s)'].median().reset_index()
    ax.plot(grouped['Custo'], grouped['Tempo (s)'], 
             color=cores[nome], marker=marcadores[nome], linestyle='-', linewidth=2.5, 
             label=nome, markersize=8, markeredgecolor='white', markeredgewidth=1.2, alpha=0.95)

ax.set_yscale('log')
formatar_eixo(ax)
ax.set_title("Tempo de Execução vs. Custo Ótimo (8-Puzzle)", fontsize=14, fontweight='bold', pad=20, color='#0F172A')
ax.set_xlabel("Custo Ótimo da Solução (Movimentos)", fontsize=12, labelpad=12, fontweight='bold')
ax.set_ylabel("Tempo de Execução (Segundos, log)", fontsize=12, labelpad=12, fontweight='bold')
ax.set_xticks(range(12, 30, 2))
ax.legend(loc="upper left", frameon=True, facecolor='white', edgecolor='#E2E8F0', framealpha=0.9, fontsize=10.5, shadow=True, borderpad=1)
plt.tight_layout()

caminho_tempo = os.path.join(OUTPUT_DIR, "grafico_tempo_8puzzle.png")
plt.savefig(caminho_tempo, dpi=200, bbox_inches='tight')
plt.close()
print(f"Salvo: {caminho_tempo}")

# ----------------- GRÁFICO 3: PERFIL DE DESEMPENHO (SURVIVAL) -----------------
print("Gerando Gráfico 3 (Perfil de Desempenho - 8-Puzzle)...")
fig, ax = plt.subplots(figsize=(fig_width, fig_height))

# Achar o tempo máximo global para limitar o eixo X de forma bonita
max_tempo = 0.002 # Padrão para 2ms

for nome, df in dfs.items():
    tempos_resolvidos = sorted(df['Tempo (s)'].values)
    instancias_resolvidas = list(range(1, len(tempos_resolvidos) + 1))
    
    tempos_plot = [0.0] + tempos_resolvidos
    resolvidos_plot = [0] + instancias_resolvidas
    
    ax.step(tempos_plot, resolvidos_plot, where='post', 
             color=cores[nome], linewidth=2.8, label=f"{nome} (Resolvido: {len(tempos_resolvidos)}/100)", alpha=0.95)
    ax.fill_between(tempos_plot, resolvidos_plot, step='post', color=cores[nome], alpha=0.04)

formatar_eixo(ax)
ax.set_xlim(0, max_tempo)
ax.set_ylim(0, 105)

ax.set_title("Perfil de Performance - Resolução Acumulada no Tempo (8-Puzzle)", fontsize=14, fontweight='bold', pad=20, color='#0F172A')
ax.set_xlabel("Tempo Limite de Execução (Segundos)", fontsize=12, labelpad=12, fontweight='bold')
ax.set_ylabel("Instâncias Resolvidas (de 100)", fontsize=12, labelpad=12, fontweight='bold')
ax.legend(loc="lower right", frameon=True, facecolor='white', edgecolor='#E2E8F0', framealpha=0.9, fontsize=10.5, shadow=True, borderpad=1)
plt.tight_layout()

caminho_survival = os.path.join(OUTPUT_DIR, "grafico_survival_8puzzle.png")
plt.savefig(caminho_survival, dpi=200, bbox_inches='tight')
plt.close()
print(f"Salvo: {caminho_survival}")

print("Todos os gráficos do 8-puzzle foram gerados com sucesso!")
