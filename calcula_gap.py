import numpy as np

def gap(f_ils, f_guloso):
    f_ils = np.asarray(f_ils, dtype=float)
    f_guloso = np.asarray(f_guloso, dtype=float)
    return (f_ils - f_guloso) / f_guloso * 100


# Caso 1: várias execuções do ILS em uma instância, um único valor do guloso
f_ils_execucoes = [123.722, 
123.608, 
123.378, 
123.659,
123.285, 
123.150, 
123.522, 
123.327, 
123.591, 
123.431]
f_guloso = 123.518

gaps = gap(f_ils_execucoes, f_guloso)
print("GAP de cada execução (%):", np.round(gaps, 4))
# Caso 2: uma instância por posição, com um vetor de ILS e outro de guloso