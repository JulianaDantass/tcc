import time
import random 
from cria_matriz import calcular_matriz_dijkstra

N_AGENTES=5
QTD_CLIENTES=442
DIAS_PLANEJAMENTO=30
DURACAO_H= 15
microarea1=[1,77]
microarea2=[78,145]
microarea3=[146,223]
microarea4=[224,400]
microarea5=[401, 442]

matriz = calcular_matriz_dijkstra()
#pacientes_prioritarios = random.sample(range(1, 274), 55)  # Sorteia 61 (20% considerando um total de 306 pacientes) números no intervalo de 1 a 306
pacientes_prioritarios1 = random.sample(range(microarea1[0], microarea1[1]), 8)  #instancia brinquedo
pacientes_prioritarios2 = random.sample(range(microarea2[0], microarea2[1]), 8)
pacientes_prioritarios3 = random.sample(range(microarea3[0], microarea3[1]), 8)
pacientes_prioritarios4 = random.sample(range(microarea4[0], microarea4[1]), 8)
pacientes_prioritarios5 = random.sample(range(microarea5[0], microarea5[1]), 8)
pacientes_prioritarios1.sort()
pacientes_prioritarios2.sort()
pacientes_prioritarios3.sort() 
pacientes_prioritarios4.sort()
pacientes_prioritarios5.sort()                             # Organiza os números em ordem crescente (opcional, mas facilita a leitura)

with open('instancia.txt', 'w') as f:
    f.write(f"{QTD_CLIENTES}\n")
    f.write(f"{N_AGENTES}\n")
    f.write(f"{DIAS_PLANEJAMENTO}\n")
    f.write(f"{DURACAO_H}\n")
    f.write(" ".join(map(str, pacientes_prioritarios1)) + "\n")
    f.write(" ".join(map(str, pacientes_prioritarios2)) + "\n")
    f.write(" ".join(map(str, pacientes_prioritarios3)) + "\n")
    f.write(" ".join(map(str, pacientes_prioritarios4)) + "\n")
    f.write(" ".join(map(str, pacientes_prioritarios5)) + "\n")
    i = 0
    for linha in matriz:
        f.write(" ".join(map(str, linha)) + "\n")                  
        i += 1












