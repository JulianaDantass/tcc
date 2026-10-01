#pragma once
#include <vector>
#include <string>
#include <map>
#include "rota_diaria.h"
#include "solucao.h"
#include "instancia.h"


struct Cliente{
    int id;             //id na matriz de tempo
    int microarea;      // 
};

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
        int salvar_solucao_manual_csv(Solucao &si, std::string nome_saida);
        const std::map<int, std::vector<int>>& getClientesPorMicroarea() const;
        const std::vector<Cliente>& getTodosClientes() const;
};