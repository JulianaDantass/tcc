#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

using namespace std;

struct Microarea{
    int id;            
    vector<int> clientes;
    int contador = 0;
};

class Instancia{
    public: 
        const double eps = 1e-3;
        int numero_agentes;
        int qtd_dias_planejamento;
        double duracao_visita_h; 
        vector<int> clientes_prioritarios;
        vector<int> clientes_nao_prioritarios;
        vector<vector<double>> matriz_tempo;
        int quantidade_clientes; 
        std::vector<Microarea> microareas;
        Instancia();
        int ler_csv(string microareas);
        int ler_instancia(string caminho_instancia);
        void exibir();
        int todos_servidos_microarea(int microarea);
};

