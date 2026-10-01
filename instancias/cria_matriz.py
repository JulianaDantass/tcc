import csv
import os
import time
import networkx as nx
import osmnx as ox
import math
from geopy.geocoders import Nominatim
from geopy.exc import GeocoderTimedOut, GeocoderUnavailable


# CONFIGURAÇÕES E CONSTANTES
CIDADE_UF = "João Pessoa, Paraíba, Brazil"
G = ox.graph_from_place(CIDADE_UF, network_type="walk")
ARQUIVO_ENTRADA = "microareas.csv"
ARQUIVO_SAIDA = "matriz_tempo.txt"
#nao é exatamente um nó, é uma aresta
NOS = "nos.txt"





#Grava o dicionário de cache no arquivo .txt.
def salvar_nos(nos):
    with open(NOS, "w", encoding="utf-8") as f:
        for i in nos:
            f.write(f"{i}\n")

# 2. GEOCODIFICAÇÃO (OBTENÇÃO DE LAT/LON)
def obter_coordenada_endereco(geolocator, rua, numero):
    
    # 3. Tenta buscar no OpenStreetMap 
    busca = f"{rua}, {numero}, {CIDADE_UF}"
    try:
        time.sleep(1.2)  
        local = geolocator.geocode(busca, timeout=10)
        if local:
            return local.latitude, local.longitude

    except (GeocoderTimedOut, GeocoderUnavailable):
        time.sleep(2)

    return None


def achar_coordenadas_csv():
    geolocator = Nominatim(user_agent="tcc_roteamento_acss_v2")

    nos = []
    print("\n---Carregando Endereços e Geocodificando ---")

    contador = 0
    with open(caminho_csv, mode='r', encoding='utf-8') as arquivo:
        leitor = csv.reader(arquivo)
        
        for idx, linha in enumerate(leitor):
            if len(linha) >= 3:
                microarea = linha[0].strip()
                rua = linha[1].strip()
                numero = linha[2].strip()
                coord = obter_coordenada_endereco(geolocator, rua, numero)
                
                if coord:
                    lat, lon = coord
                    n = ox.distance.nearest_nodes(G, X=lon, Y=lat)
                    nos.append(n)
                else:
                    print(f" linha {idx + 1}: Não localizado -> {rua}, {numero}")

    salvar_nos(nos)

#CÁLCULO DA MATRIZ VIA DIJKSTRA LOCAL (NETWORKX)
def calcular_matriz_dijkstra():

    if os.path.exists(NOS) and os.path.getsize(NOS) > 0:
        print("Tentando carregar coordenadas... ---")
        nos = []
        with open(NOS, "r", encoding="utf-8") as f:
            for linha in f:
                no = int(linha.strip())
                nos.append(no)
    
    n = len(nos)
    matriz_distancias = []
    
    print(f"\n--- 3. Calculando Matriz---")
    
    cont=0
    for i in nos:
        linha = []
        for j in nos:
            if i == j:            
                distancia = 0.0
            else:
                distancia = nx.shortest_path_length(G, source=i, target=j, weight="length")
            tempo = distancia/3000.0;  
            linha.append(round(tempo, 3)) 
               
             
        matriz_distancias.append(linha)
        print(cont)
        cont+=1
                
    return matriz_distancias

def salvar_matriz(matriz):
    
    with open(ARQUIVO_SAIDA, "w", encoding="utf-8") as f:
       for linha in matriz:
            # Junta os elementos da linha com espaço e coloca UMA quebra ao final da linha
            f.write(" ".join(map(str, linha)) + "\n")


