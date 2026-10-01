#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

using namespace std;

class Instancia{
    public: 
        int numero_agentes;
        int qtd_dias_planejamento;
        double duracao_visita_h; 
        vector<int> clientes_prioritarios;
        vector<int> clientes_nao_prioritarios;
        vector<vector<double>> matriz_tempo;
        int quantidade_clientes;
       
    public: 
        Instancia();
        int ler_instancia(string caminho_instancia);
        void exibir();
};

