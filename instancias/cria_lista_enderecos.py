import osmnx as ox
import networkx as nx
import time

complemento_cidade = "João Pessoa, Paraíba, Brazil"

mapa_jp = ox.graph_from_place(f"{complemento_cidade}", network_type="walk")

ruas_atendidas_usf = ['Rua Etelvina Macedo de Mendonça', 'Travessa Barão da Passagem', 'Rua Dom Santino Coutinho', 'Rua Professor Eliseu Maul', 
'Rua José Severino Massa Spinelli', 'Avenida Rui Barbosa', 'Avenida Geminiano da França', 'Rua Desembargador Pedro Batista da Silva', 'Rua Caturité', 
'Avenida Ministro José Américo de Almeida', 'Rua José Alves de Lima', 'Rua Severino Elias Justino', 'Rua Beatriz Maria do Espirito Santo', 
'Rua Feliciano Dourado', 'Avenida Juarez Távora', 'Avenida Adolfo Cime', 'Avenida Dom Pedro II', 'Rua Agostinho Figueiredo']

numeros_rua = [ 
(49, 57, 101, 103, 113, 143, 93, 118, 114, 96, 54, 179, 183, 193, 197, 207, 223, 231, 237, 243, 311, 317, 321, 361, 369, 373, 377, 413, 419, 435, 472, 474, 
500, 368, 356, 336, 308, 304, 232, 222, 630), 
(29, 31, 200, 115, 103, 1163, 1145, 1109, 1095, 1106, 1150, 1174, 566, 600, 626, 638, 656, 625, 87, 97, 673, 679, 713, 725, 747, 753, 761, 42, 43, 37, 
36, 789, 797, 805, 1498, 1432),
(109, 73, 216, 230, 246, 73, 265, 251, 191, 181, 178, 168, 140, 91),
(20, 30, 42, 50, 62, 72, 80, 88, 120, 139, 159, 171, 185, 197, 181, 196, 393, 407, 467, 515, 525, 370, 351, 140, 242, 89, 59, 49), 
(98, 99, 100, 540, 548, 570, 832, 848, 874, 880, 918, 928, 950, 405, 405, 397, 379, 365, 353, 345, 329, 311, 847, 831, 249, 101),
(1028, 1010, 997, 1008, 1009),
(71, 81, 91, 112, 106, 96, 86, 82, 76, 70, 62, 56, 52, 50, 42, 147, 157, 165, 171, 179, 193, 207, 200, 160, 130),
(21, 25, 35, 49, 57, 75, 85, 137, 147, 141, 139, 149, 181, 183, 195, 205, 211, 231, 249, 269, 285, 303, 317, 319, 320, 318, 300, 270, 268, 256, 254, 244, 
240, 212, 200, 184, 146, 140, 138, 92, 76, 50, 44, 42, 32, 26, 20, 12),
(1102, 1138, 1228),
(),
(),
(),
(1382, 1380, 1403, 1410, 1424, 1430, 1440, 1452, 1458, 1464, 1466, 1480, 1481, 1477, 1472, 1327, 1454, 1399, 1343, 1390, 1393, 1385, 1381),
(3000, 3379, 3335, 3319, 3297, 3255, 3189, 3181, 3171, 3165, 3157, 3149, 3131, 3125, 3117, 3107, 3099, 3077, 3263, 3004, 3050, 3013, 3007),
(10, 11,56,936, 957, 960, 962, 965, 1680, 1620, 1608, 1604, 1590, 157, 1558, 1556, 170),
(2227, 2381, 2391, 2410, 2439, 2467, 2581),
(972, 939, 971, 969, 940, 968, 945, 970, 970, 963)
]

endereço_usf = "Rua José Severino Massa Spinelli, 835, João Pessoa, Paraíba, Brazil"
locais = {}
locais["posto"] = endereço_usf

with open("enderecos_clientes.txt", "w", encoding="utf-8") as arquivo:
    arquivo.write(endereço_usf)
    arquivo.write("\n")

for rua, numeros_rua in zip(ruas_atendidas_usf, numeros_rua):

    if not numeros_rua: #caso que a tupla é vazia 
        continue
        
    for numero in numeros_rua:
        endereco_curto = f"{rua}, {numero}"
        endereco_longo = f"{rua}, {numero}, {complemento_cidade}"   # Junta a rua, o número e a cidade numa frase só
        locais[endereco_curto] = endereco_longo                     # Adiciona no dicionário
        with open("enderecos_clientes.txt", "a", encoding="utf-8") as arquivo:
            arquivo.write(endereco_longo)
            arquivo.write("\n")

print("Arquivo criado com sucesso!")



#falta fazer ::::::::::::::::::::::::::::::::::::::::::::::::::
#rua severino patricio, rua mauricio salustiano de medeiros, minervino bione, rua talvares cavalcante -> usar latitude e longitude
#travessa juarez távora 404              -> ''
#rua jose alves, severinos elias, beatriz maria, salustiano nao tem numeros especificos


#fig, ax = ox.plot_graph(mapa_torre)   #Desenha o mapa na tela!