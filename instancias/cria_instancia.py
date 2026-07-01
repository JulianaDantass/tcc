
#1 linha diz os id dos pacientes prioritarios
#3 linha em diante é a matriz de dist
#o primeiro endereço, ou seja o nó 0, é o endereço da USF
#-----------------------------------------------------------------

import osmnx as ox
import networkx as nx
import time
import ast
import pandas as pd
import random 

casas = []
nos = []
local_busca = "Torre, João Pessoa, Paraíba, Brazil"
#mapa_jp = ox.graph_from_place(f"{complemento_cidade}", network_type="walk")

mapa_torre = ox.graph_from_place(local_busca, network_type='walk')

#salvando num arquivo os nós proximos
with open('coordenadas.txt', 'r', encoding='utf-8') as arquivo:
    for linha in arquivo:
        coordenada = ast.literal_eval(linha)
        coord_redonda = round(coordenada[0], 6), round(coordenada[1], 6)
        casas.append(coord_redonda)  
    

for loc in casas:
    no = ox.distance.nearest_nodes(mapa_torre, X=loc[1], Y=loc[0])
    nos.append(no)

matriz = []
for no in nos:  #Calcular a distância de todos os nós para todos (NxN)
    linha = []

    for no2 in nos:
        if no == no2:
            linha.append(0.0) 
        else:
            distancia_metros = nx.shortest_path_length(mapa_torre, no, no2, weight='length')
            linha.append(round(distancia_metros, 2))   #limita pra 2 casas após a virgula        
    matriz.append(linha)


pacientes_prioritarios = random.sample(range(1, 274), 55)  # Sorteia 61 (20% considerando um total de 306 pacientes) números no intervalo de 1 a 306
#pacientes_prioritarios = random.sample(range(1, 307), 8)  #instancia brinquedo
pacientes_prioritarios.sort()                              # Organiza os números em ordem crescente (opcional, mas facilita a leitura)

with open('instancia.txt', 'w') as arquivo:
    arquivo.write(f"{pacientes_prioritarios}\n\n")
    for linha in matriz:
        linha_texto = ' '.join(str(valor) for valor in linha) 
        arquivo.write(linha_texto + '\n')                     













