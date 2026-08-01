#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

using namespace std;

class Instancia{
    public: 
        string nome_instancia;
        int q_clientes;
        vector<int> clientes_prioritarios;
        vector<int> clientes_nao_prioritarios;
        vector<vector<double>> matriz_distancias;

    public: 
        Instancia();
        int ler_instancia(string caminho_instancia);
        void exibir();
};

