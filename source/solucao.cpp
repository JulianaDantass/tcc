#include "solucao.h"
#include <iostream>
#include <fstream>
#include <string>
#include <climits>
#include <vector>


Solucao::Solucao( int qtd){
    dias_planejamento.reserve(qtd);
    funcao_objetivo = 9999999;

}

const std::vector<Rotas_diarias>& Solucao::get_dias_planejamento() const{
    return dias_planejamento;
}

int Solucao::get_quantidade_dias() const{
    return dias_planejamento.size();
}    

double Solucao::get_fo() const{

    return funcao_objetivo;
};


void Solucao::set_fo(double fo){

    this->funcao_objetivo = fo;
};

void Solucao::adiciona_dia(const Rotas_diarias& dia){
        
    dias_planejamento.push_back(dia);
}

double Solucao::calcula_fo(){

    double t_total_mes = 0; 

    for(int i = 0; i < dias_planejamento.size(); i++){
        t_total_mes += dias_planejamento[i].get_tempo_total_dia();
    }

    this->funcao_objetivo = t_total_mes;
    return t_total_mes;
}

void Solucao::exibe_solucao() const{

    std::cout << "Foram necessários " << dias_planejamento.size() << "dias" << std::endl;
    std::cout << "O valor do tempo total (em h) foi de " << funcao_objetivo << std::endl; 
    
    for(int i=0; i < dias_planejamento.size(); i++){
        std::cout << "inicio do dia:" << std::endl;
        dias_planejamento[i].exibir_dia();
        std::cout << "fim do dia" << std::endl;
    }
}


