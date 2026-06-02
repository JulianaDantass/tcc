
#1 linha diz os id dos pacientes prioritarios
#3 linha em diante é a matriz de dist
#o primeiro endereço, ou seja o nó 0, é o endereço da USF
#-----------------------------------------------------------------

import osmnx as ox
import networkx as nx
import time
import pandas as pd
import random 

nos_fora_mapa = [
    (-7.1226564, -34.8795692),
    (-7.1226564, -34.8795692),
    (-7.1227879, -34.8587817),
    (-7.1244948, -34.858278),
    (-7.1244948, -34.858278),
    (-7.1375199, -34.8540944),
    (-7.1249589, -34.8657526),
    (-7.1254669, -34.864668)]

complemento_cidade = "João Pessoa, Paraíba, Brazil"

# Cria uma lista limpando os espaços/quebras de linha de cada linha
with open('enderecos_pacientes.txt', 'r', encoding='utf-8') as arquivo:
    linhas = [linha.strip() for linha in arquivo]  

lons = []
lats = []
tuplas_original = []
deram_erro = []


#Converter cada endereço em Latitude e Longitude
for endereco in linhas:  
    try:
        lat, lon = ox.geocode(endereco)
        tuplas_original.append((lat, lon))

        #verificando endereço fora do mapa
        for i in nos_fora_mapa:
            if tuplas_original[-1] == i:
                print(f"Endereço {endereco}")
                break

        lons.append(lon)
        lats.append(lat)
    except Exception as e:
        deram_erro.append(endereco)
    time.sleep(1)


#salvando as coordenadas originais em um arquivo
with open('coordenadas.txt', mode='w') as arquivo:
    for tupla in tuplas_original:
        linha = f"{tupla}\n"
        arquivo.write(linha)



mapa_jp = ox.graph_from_place(f"{complemento_cidade}", network_type="walk")

#Encontrar os "nós" (ID criado pelo OSM para representar certo ponto da rua) mais próximos das coordenadas
nos_exatos = ox.distance.nearest_nodes(mapa_jp, X=lons, Y=lats) 

#salvando num arquivo os nós proximos
with open('coordenadas.txt', mode='a') as arquivo:
    arquivo.write("\n")
    for no in nos_exatos:
        print(no)
        print("\n")
        linha = f"{no}\n"
        arquivo.write(linha)

"""
matriz = []
for no_origem in nos_exatos:  #Calcular a distância de todos os nós para todos (NxN)
    linha = []

    for no_destino in nos_exatos:
        if no_origem == no_destino:
            linha.append(0.0) 
        else:
            distancia_metros = nx.shortest_path_length(mapa_jp, no_origem, no_destino, weight='length')
            linha.append(round(distancia_metros, 2))   #limita pra 2 casas após a virgula
            
    matriz.append(linha)



pacientes_prioritarios = random.sample(range(1, 307), 61)  # Sorteia 61 (20% considerando um total de 306 pacientes) números no intervalo de 1 a 306
#pacientes_prioritarios = random.sample(range(1, 307), 8)  #instancia brinquedo
pacientes_prioritarios.sort()                              # Organiza os números em ordem crescente (opcional, mas facilita a leitura)

with open('instanciac.txt', 'w') as arquivo:
    arquivo.write(f"{pacientes_prioritarios}\n\n")
    for linha in matriz:
        linha_texto = ' '.join(str(valor) for valor in linha) 
        arquivo.write(linha_texto + '\n')                     
"""












