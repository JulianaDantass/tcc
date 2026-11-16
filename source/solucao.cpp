#include "solucao.h"
#include <iostream>
#include <fstream>
#include <string>
#include <climits>
#include <cmath>
#include <vector>

Solucao::Solucao(){
    funcao_objetivo = 9999999;

}
Solucao::Solucao(int qtd){
    dias_planejamento.reserve(qtd);
    funcao_objetivo = 9999999;

}

std::vector<Rotas_diarias>& Solucao::get_dias_planejamento(){
    return dias_planejamento;
}

Rotas_diarias& Solucao::get_ultimo_dia(){
    return dias_planejamento[dias_planejamento.size()-1];
}


int Solucao::get_quantidade_dias() const{
    
    int cont_dias_vazios = 0;

    for(int i=0; i < dias_planejamento.size(); i++){
        
        int cont_rotas_vazias = 0;
        for(int j = 0; j < dias_planejamento[i].rotas.size(); j++){   
            
            if(dias_planejamento[i].rotas[j].sequencia_am.size() <= 1 and dias_planejamento[i].rotas[j].sequencia_pm.size() == 0){
                cont_rotas_vazias++;
            }
        }    
        if(cont_rotas_vazias == 5){
            cont_dias_vazios++;
        }
    }

    return dias_planejamento.size() - cont_dias_vazios; 
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

bool Solucao::salva_solucao_arquivo(std::string arq, double tempo_ils, int melhor_iteracao, std::vector<bool>&priori){

    std::ofstream arquivo(arq);
 
    if (!arquivo.is_open()) {
        std::cerr << "Erro: Não foi possível criar o arquivo " << arq << ".\n";
        return false;
    }

    arquivo << "TEMPO TOTAL (h): " << this->funcao_objetivo/60.0 << std::endl;
    arquivo << "TEMPO ILS (s): " << tempo_ils << std::endl; 
    //arquivo << "MÈDIA DE TEMPO RVND+PERT (s): " << media_rvnd << std::endl;
    arquivo << "MELHOR ITERAÇÃO: " << melhor_iteracao    << std::endl;
    arquivo << "\n obs.: os clientes prioritários estão entre parenteses\n"; 
    std::vector<double>horas(dias_planejamento[0].rotas.size(), 0); 
    for(int i=0; i < dias_planejamento.size(); i++){
        arquivo << "DIA " << i+1 << ":\n";
        
        int cont_rotas_vazias = 0;
        for(int j = 0; j < dias_planejamento[i].rotas.size(); j++){            

            arquivo << "agente " << j+1 << " - ";
            arquivo << "manhã: ";
            for(int k = 0; k < dias_planejamento[i].rotas[j].sequencia_am.size(); k++){
                
                int id = dias_planejamento[i].rotas[j].sequencia_am[k];
                if(priori[id] == true)
                    arquivo << "(" << id << "), ";
                else
                    arquivo << id << ", ";
            }
            
            arquivo << "\n           tarde: ";
            for(int k = 0; k < dias_planejamento[i].rotas[j].sequencia_pm.size(); k++){
                
                int id = dias_planejamento[i].rotas[j].sequencia_pm[k];
                if(priori[id] == true)
                    arquivo << "(" << id << "), ";
                else
                    arquivo << id << ", ";
            }
            horas[j] += dias_planejamento[i].rotas[j].tempo_am + dias_planejamento[i].rotas[j].tempo_pm;
            arquivo << std::endl;
        }
    }
    
    int j=0;
    for(double i:horas){
        arquivo << "agente " << j+1 << " horas: " << i/60.0 << "\n";
        j++;
    }
    int qtd_dias = dias_planejamento.size();
    int qtd_semanas = qtd_dias / 2; 
    if(qtd_dias % 2 > 0){
        qtd_semanas++;
    }
    arquivo << "SEMANAS (considerando que há 2 dias de visita): " << qtd_semanas << "\n";
    arquivo << "DIAS: " << (dias_planejamento.size()) << std::endl;
    
    
    arquivo.close();
}
