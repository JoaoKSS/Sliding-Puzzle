import sys
import pandas as pd
import openpyxl
from openpyxl.styles import Font, PatternFill, Alignment

def gerar_arquivo_excel(arquivo_txt, arquivo_saida):
    print(f"A ler dados de '{arquivo_txt}'...")
    
    try:
        # 1. Ler o ficheiro txt linha por linha
        with open(arquivo_txt, 'r', encoding='utf-8') as f:
            linhas = f.readlines()
            
        dados_limpos = []
        for linha in linhas:
            # Ignora a linha de separação decorativa (ex: ---+---+---)
            if '---' in linha:
                continue
                
            # Divide as colunas pelo caractere '|' e remove os espaços em branco
            partes = [parte.strip() for parte in linha.split('|')]
            
            # Só adiciona se a linha realmente tiver dados válidos
            if len(partes) > 1:
                dados_limpos.append(partes)
                
        if not dados_limpos:
            print("Erro: Não foram encontrados dados válidos no ficheiro.")
            sys.exit(1)
                
        # Separar o cabeçalho dos dados
        cabecalho = dados_limpos[0]
        linhas_dados = dados_limpos[1:]
        
        # 2. Criar um DataFrame (Tabela)
        df = pd.DataFrame(linhas_dados, columns=cabecalho)
        
        # Converter as colunas para formato numérico
        df['Inst'] = pd.to_numeric(df['Inst'])
        df['Custo'] = pd.to_numeric(df['Custo'])
        df['Estados Avaliados'] = pd.to_numeric(df['Estados Avaliados'])
        df['Tempo (s)'] = pd.to_numeric(df['Tempo (s)'])
        
        print("A gerar ficheiro Excel e a aplicar formatação...")
        
        # 3. Exportar para Excel usando o openpyxl como motor
        writer = pd.ExcelWriter(arquivo_saida, engine='openpyxl')
        df.to_excel(writer, index=False, sheet_name='Dados do Experimento')
        
        # 4. Aplicar formatações básicas (que o Google Sheets vai importar e manter)
        workbook = writer.book
        worksheet = writer.sheets['Dados do Experimento']
        
        # Estilo do cabeçalho: Fundo azul escuro e texto branco em negrito
        estilo_fonte = Font(bold=True, color="FFFFFF")
        estilo_fundo = PatternFill(start_color="2C3E50", end_color="2C3E50", fill_type="solid")
        alinhamento_centro = Alignment(horizontal="center", vertical="center")
        
        for celula in worksheet["1:1"]:
            celula.font = estilo_fonte
            celula.fill = estilo_fundo
            celula.alignment = alinhamento_centro
            
        # Ajustar automaticamente a largura das colunas
        for col in worksheet.columns:
            tamanho_maximo = 0
            letra_coluna = col[0].column_letter
            for celula in col:
                try:
                    if len(str(celula.value)) > tamanho_maximo:
                        tamanho_maximo = len(str(celula.value))
                except:
                    pass
            worksheet.column_dimensions[letra_coluna].width = tamanho_maximo + 4
            
        # Gravar o ficheiro
        writer.close()
        
        print("="*50)
        print(f"SUCESSO! O ficheiro '{arquivo_saida}' foi gerado.")
        print("="*50)

    except FileNotFoundError:
        print(f"Erro: O ficheiro '{arquivo_txt}' não foi encontrado.")
        sys.exit(1)
    except Exception as e:
        print(f"Ocorreu um erro inesperado: {e}")
        sys.exit(1)

if __name__ == "__main__":
    # Verifica se o utilizador passou pelo menos o ficheiro de entrada
    if len(sys.argv) < 2:
        print("Uso incorreto. Tem de indicar o ficheiro de entrada.")
        print("Exemplo: python gerar_planilha.py dados_estrela.txt")
        sys.exit(1)
        
    # O primeiro argumento (índice 1) é o nome do ficheiro de texto
    NOME_ENTRADA = sys.argv[1]
    
    # Se o utilizador passar um segundo argumento, usamos como nome de saída
    if len(sys.argv) >= 3:
        NOME_SAIDA = sys.argv[2]
        # Garante que tem a extensão .xlsx
        if not NOME_SAIDA.endswith('.xlsx'):
            NOME_SAIDA += '.xlsx'
    else:
        # Nome padrão caso não seja fornecido
        nome_base = NOME_ENTRADA.split('.')[0] # Tira a extensão .txt
        NOME_SAIDA = f"{nome_base}_Formatado.xlsx"
    
    gerar_arquivo_excel(NOME_ENTRADA, NOME_SAIDA)