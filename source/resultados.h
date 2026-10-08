#pragma once
#include <vector>
#include <string>
#include <map>
#include "rota_diaria.h"
#include "solucao.h"
#include "instancia.h"


struct Microarea{
    int id;            
    vector<int> clientes;       
    
};

class Resultados {
    private:
        int deposito;
        Instancia* dados;
        std::string arquivo_entrada;
        std::string arquivo_saida;
        std::vector<Microarea> microareas;

    public:
        // Construtores
        Resultados(Instancia *d);
        // Métodos
        std::string trim(const std::string& str);
        int ler_csv(string caminho);
        int gerar_solucao_manual(std::string nome_entrada, std::string nome_saida);
        int salvar_solucao_manual(Solucao &si, std::string nome_saida);
        std::vector<int> gerar_ordem_vizinho_mais_proximo(int no_inicial, const std::vector<int>& clientes_microarea);
        void printa_microareas();
};