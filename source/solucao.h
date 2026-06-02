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
        
        Solucao(int qtd_dias);
        void adiciona_dia(const Rotas_diarias& dia);
        int get_quantidade_dias();                  
        std::vector<Rotas_diarias>& get_dias_planejamento();
        double calcula_fo();
        double get_fo();
        void set_fo(double fo);
        void exibe_solucao();
};

