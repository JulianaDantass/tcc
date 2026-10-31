#include "solucao.h"
#include <iostream>
#include <fstream>
#include <string>
#include <climits>
#include <vector>


Solucao::Solucao(int qtd){
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
        //std::cout << "i" << t_total_mes << std::endl;
    }

    this->funcao_objetivo = t_total_mes;
    return t_total_mes;
}

std::vector<Rotas_diarias> Solucao::get_dias_planejamento_estatico(){
    return dias_planejamento;
}

bool Solucao::salva_solucao_arquivo(std::string arq, std::vector<int>priori, std::vector<int>n_priori){

    std::ofstream arquivo(arq);
 
    if (!arquivo.is_open()) {
        std::cerr << "Erro: Não foi possível criar o arquivo " << arq << ".\n";
        return false;
    }

    arquivo << "Pacientes prioritários: ";
    for(int i=0; i<priori.size(); i++){
        
        arquivo << priori[i] << " "; 
    }
    arquivo << "\nTotal de prioritários: " << priori.size() << std::endl;
    arquivo << "\nTotal de não prioritários: " << n_priori.size() << std::endl;
    
    int cont_dias_vazios = 0;
    for(int i=0; i < dias_planejamento.size(); i++){
        arquivo << "DIA " << i+1 << ":\n";
        
        int cont_rotas_vazias = 0;
        for(int j = 0; j < dias_planejamento[i].rotas.size(); j++){            

            arquivo << "agente " << j+1 << " - ";
            arquivo << "manhã: ";
            for(int k = 0; k < dias_planejamento[i].rotas[j].sequencia_am.size(); k++){
                
                arquivo << dias_planejamento[i].rotas[j].sequencia_am[k] << ", ";
               
            }
            arquivo << "\n           tarde: ";
            for(int k = 0; k < dias_planejamento[i].rotas[j].sequencia_pm.size(); k++){
                
                arquivo << dias_planejamento[i].rotas[j].sequencia_pm[k] << ", ";
               
            }
            arquivo << std::endl;
        }
    }

    arquivo << "FORAM NECESSARIOS EM DIAS: " << (dias_planejamento.size()) << "\n";
    arquivo << "TEMPO TOTAL GASTO EM HORAS: " << funcao_objetivo << std::endl; 
    
    arquivo.close();
}
