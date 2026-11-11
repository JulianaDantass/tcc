#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

using namespace std;

struct Microarea{
    int id;
    int primeiro_cliente;
    int ultimo_cliente;            
    vector<int> clientes_prioritarios;
    vector<int> clientes_nao_prioritarios;
    int contador_prioritarios = 0;
    int contador_nao_prioritarios = 0;
};

class Instancia{
    public: 
        const double eps = 1e-6;
        int numero_agentes;
        int qtd_dias_planejamento;
        double duracao_visita_h; 
        vector<vector<double>> matriz_tempo;
        int quantidade_clientes; 
        std::vector<Microarea> microareas;
        Instancia();
        int ler_microareas();
        int ler_instancia(string caminho_instancia);
        void exibir();
        void zera();
        int todos_servidos_microarea(int microarea, int eh_priori);
        
};

