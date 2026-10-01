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

std::vector<Rotas_diarias>& Solucao::get_dias_planejamento(){
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

std::vector<Rotas_diarias> Solucao::get_dias_planejamento_estatico(){
    return dias_planejamento;
}


