#pragma once
#include <vector>
#include <string>
#include <map>
#include "rota_diaria.h"
#include "solucao.h"
#include "instancia.h"


class Manual {
    private:
        int deposito;
        Instancia* dados;
        std::string arquivo_entrada;
        std::string arquivo_saida;
        vector<bool>inseridos;
        //std::vector<Microarea> microareas;

    public:
        // Construtores
        Manual(Instancia *d);
        // Métodos
        std::string trim(const std::string& str);
        //int ler_csv(string caminho);
        bool atender_um (Rotas_diarias& dia, int a, bool prioritario);
        int gerar_solucao_manual(std::string nome_entrada, std::string nome_saida);
        int salvar_solucao_manual(Solucao &si, std::string nome_saida);
  
        void printa_microareas();
};