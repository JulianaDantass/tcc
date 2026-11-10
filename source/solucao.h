#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "rota_diaria.h"

// o planejamento até o momento é somente para 1 mês
class Solucao{    
  
    private:
        std::vector<Rotas_diarias> dias_planejamento;
        double funcao_objetivo;
    public:
        Solucao();
        Solucao(int qtd_dias);
        void adiciona_dia(const Rotas_diarias& dia);
        void set_fo(double fo);
        
        int get_quantidade_dias() const;                  
        std::vector<Rotas_diarias>& get_dias_planejamento();
        std::vector<Rotas_diarias> get_dias_planejamento_estatico();
        Rotas_diarias& get_ultimo_dia();

        double calcula_fo();
        double get_fo() const;
        bool salva_solucao_arquivo(std::string nome);
};

