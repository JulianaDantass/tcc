import time
import random 
from cria_matriz import calcular_matriz_dijkstra

N_AGENTES=5
DIAS_PLANEJAMENTO=30
DURACAO_H= 0.25
qtd_brinquedo=20
qtd_real=442

matriz = calcular_matriz_dijkstra()
#pacientes_prioritarios = random.sample(range(1, 274), 55)  # Sorteia 61 (20% considerando um total de 306 pacientes) números no intervalo de 1 a 306
pacientes_prioritarios = random.sample(range(1, qtd_brinquedo), 5)  #instancia brinquedo
pacientes_prioritarios.sort()                              # Organiza os números em ordem crescente (opcional, mas facilita a leitura)

with open('instanciab.txt', 'w') as f:
    f.write(f"{N_AGENTES}\n")
    f.write(f"{DIAS_PLANEJAMENTO}\n")
    f.write(f"{DURACAO_H}\n")
    f.write(" ".join(map(str, pacientes_prioritarios)) + "\n\n")
    i = 0
    for linha in matriz:
        if i == qtd_brinquedo:
            break
        f.write(" ".join(map(str, linha)) + "\n")                  
        i += 1












