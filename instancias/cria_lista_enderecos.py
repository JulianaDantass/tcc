import osmnx as ox
import networkx as nx
import overpy
import requests
import time

ruas_com_numero = [
('Rua Etelvina Macedo de Mendonça', [49, 57, 101, 103, 113, 143, 93, 118, 114, 96, 54, 179, 183, 193, 197, 207, 223, 231, 237, 243, 311, 317, 321, 361, 369, 373, 377, 413, 419, 435, 472, 474, 500, 368, 356, 336, 308, 304, 232, 222, 630]), 
('Travessa Barão da Passagem', [29, 31, 200, 115, 103, 1163, 1145, 1109, 1095, 1106, 1150, 1174, 566, 600, 626, 638, 656, 625, 87, 97, 673, 679, 713, 725, 747, 753, 761, 42, 43, 37, 36, 789, 797, 805, 1498, 1432]),
('Rua Dom Santino Coutinho', [109, 73, 216, 230, 246, 73, 265, 251, 191, 181, 178, 168, 140, 91]), 
('Rua Professor Eliseu Maul', [20, 30, 42, 50, 62, 72, 80, 88, 120, 139, 159, 171, 185, 197, 181, 196, 393, 407, 467, 515, 525, 370, 351, 140, 242, 89, 59, 49]), 
('Rua José Severino Massa Spinelli', [98, 99, 100, 540, 548, 570, 832, 848, 874, 880, 918, 928, 950, 405, 405, 397, 379, 365, 353, 345, 329, 311, 847, 831, 249, 101]),
('Avenida Rui Barbosa', [1010, 1008]),
('Avenida Geminiano da França', [71, 81, 91, 112, 106, 96, 86, 82, 76, 70, 62, 56, 52, 50, 42, 147, 157, 165, 171, 179, 193, 207, 200, 160, 130]),
('Rua Desembargador Pedro Batista da Silva', [21, 25, 35, 49, 57, 75, 85, 137, 147, 141, 139, 149, 181, 183, 195, 205, 211, 231, 249, 269, 285, 303, 317, 319, 320, 318, 300, 270, 268, 256, 254, 244, 240, 212, 200, 184, 146, 140, 138, 92, 76, 50, 44, 42, 32, 26, 20, 12]),
('Rua Caturité', [1102, 1138, 1228]), 
('Rua Feliciano Dourado', [1403, 1424, 1430, 1458, 1464, 1480, 1481, 1327, 1399, 1393, 1381]), #tem + casas
('Avenida Juarez Távora', [3000, 3319, 3107, 3099, 3077, 3004]), #tem + casas(
('Avenida Adolfo Cime', [10, 11,56,936, 957, 960, 962, 965, 1680, 1620, 1608, 1604, 1590, 157, 1558, 1556, 170]),
('Rua Agostinho Figueiredo', [972, 939, 971, 969, 940, 968, 945, 970, 970, 963]),
('Rua João Nobre', [457, 328])
]

#o segundo número é a quantidade de casas
ruas_por_id = [ 
(115226185, 23), #Rua Severino Elias Justino:
(787639564, 54), #Rua Beatriz Maria E. Santo:
(115226186, 64), #Rua José Alves de Lima
(69288619, 3),   #R. Severino Patrício da Silva
(787639569, 25), #R. Mauricio Salustiano
(115226613, 9)  #rua tavares cavalcante
]   


tuplas_original = []
deram_erro = []

complemento_cidade = "João Pessoa, Paraíba, Brazil"
endereço_usf = "Rua José Severino Massa Spinelli, 835, João Pessoa, Paraíba, Brazil"


mapa_jp = ox.graph_from_place(f"{complemento_cidade}", network_type="walk")

with open("enderecos_clientes.txt", "w", encoding="utf-8") as arquivo:
    arquivo.write(endereço_usf)
    arquivo.write("\n")

for rua, numeros in ruas_com_numero:
    for n in numeros:
        endereco_longo = f"{rua}, {n}, {complemento_cidade}"   
        with open("enderecos_clientes.txt", "a", encoding="utf-8") as arquivo:
            arquivo.write(endereco_longo)
            arquivo.write("\n")


with open('enderecos_clientes.txt', 'r', encoding='utf-8') as arquivo:
    linhas = [linha.strip() for linha in arquivo]  


for endereco in linhas:  
    try:
        lat, lon = ox.geocode(endereco)
        tuplas_original.append((lat, lon))
        #verificando endereço fora do mapa
        #for i in nos_fora_mapa:
        #    if tuplas_original[-1] == i:
        #        print(f"Endereço {endereco}")
        #        break
    except Exception as e:
        deram_erro.append(endereco)
    time.sleep(1)


url = "https://overpass-api.de/api/interpreter"

meus_headers = {
    "Accept": "application/json", # Força o servidor a entender que você quer JSON
    "User-Agent": "ProjetoTCC (julianadantas498@gmail.com)" 
}


for id_no, qtd in ruas_por_id: 
    query = f"""
    [out:json];
    way({id_no});
    out geom;
    """

    resposta = requests.get(url, params={'data': query}, headers=meus_headers)

    # Verificação de segurança (boa prática!)
    if resposta.status_code == 200:
        dados = resposta.json()

        bounds = dados['elements'][0]['bounds'] 
        min_lat = bounds['minlat'];
        max_lat = bounds['maxlat'];
        min_lon = bounds['minlon'];
        max_lon = bounds['maxlon'];

        #tirando a média da lat e long da rua
        media_lon = (min_lon+max_lon) / 2.0
        media_lat = (min_lat+max_lat) / 2.0 

        tuplas_original.append((media_lat, media_lon))
    else:
        print(resposta.text)

with open('coordenadas.txt', mode='w') as arquivo:
    for tupla in tuplas_original:
        linha = f"{tupla}\n"
        arquivo.write(linha)

print("Arquivo criado com sucesso!")





#falta fazer ::::::::::::::::::::::::::::::::::::::::::::::::::

#falta Minervino Bione:
#('Avenida Dom Pedro II', [2227, 2381, 2391, 2410, 2439, 2467]),
#('Avenida Ministro José Américo de Almeida', [1166, 1167, 1168])
#travessa juarez távora

#fig, ax = ox.plot_graph(mapa_torre)   #Desenha o mapa na tela!