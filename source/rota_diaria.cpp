#include "rota_diaria.h"
#include <iostream>
#include <fstream>
#include <unordered_set>

   
#define MAX_HORAS_AM 4
#define MAX_HORAS_PM 4
#define EPSILON  0.0001

Rotas_diarias::Rotas_diarias(int n_agentes, double d){
    
    duracao_visita_h= d;
    turno.resize(n_agentes); //eu tenho um valor int associado a cada agente   
    
    for(int i = 0; i < n_agentes; i++){
        rotas.push_back({contador_id, std::vector<int>{0}, std::vector<int>{}});
        contador_id++;     
        turno[i] = 1;
    }
} 

/*
adiciona um paciente (inteiro p) à rota do agente a
d é a distancia entre o ultimo paciente da rota e o paciente p
t_volta_posto é a distancia de p ao posto
*/
int Rotas_diarias::adicionar_visita(int p, int a, double d, double t_volta_posto) {
    
    
    double t_chegada;
 
    //retorno q significa que o agente encerrou o dia
    if(turno[a] == 3){
        return 1; 
    }

    //ve se é possivel visitar e voltar pro posto
    double custo_total = d + duracao_visita_h + t_volta_posto;
    int eh_impossivel = visita_possivel(a, custo_total);

    //se não é possivel, tem retorno 1 ou 2
    if(eh_impossivel > 0){
        return 1;
    }else{        

        if(turno[a] == 1){
            //std::cout << "rota: " << a << " / adicionado em am: " << p << " com valor " << d << std::endl;
            rotas[a].sequencia_am.push_back(p);
            rotas[a].tempo_am +=  d + duracao_visita_h;
            

        }else if(turno[a] == 2){
            //std::cout << "rota: " << a << " / adicionado em pm: " << p << " com valor " << d << std::endl;
            rotas[a].sequencia_pm.push_back(p);
            rotas[a].tempo_pm +=  d + duracao_visita_h;
        } 

        return 0;
    } 
}

int Rotas_diarias::visita_possivel(int agente, double custo_total){

    if(turno[agente] == 1){
               
        if(get_tempo_disponivel_am(agente) - custo_total < double(EPSILON)){
            return 1; 
        }else{

            return 0;
        }
    } 

    if(turno[agente]  == 2){
        
        if(get_tempo_disponivel_pm(agente) - custo_total < double(EPSILON)){
            return 2;
        }else{
            return 0;
        }
    }
}

//retorna o ultimo paciente visitado pelo agente
int Rotas_diarias::ultimo_no_rota(int ag){ 
    if(turno[ag] == 1){
        return rotas[ag].sequencia_am.back();
    }else{
        return rotas[ag].sequencia_pm.back();
    }
      
}

int Rotas_diarias::troca_turno(int ag, double dist_volta_posto){

    //adicionando a volta ao posto
    if(turno[ag] == 3){
        //std::cout << "entrou no turno 3" << std::endl;
        return 2;

    }else if(turno[ag] == 1){
            
        //std::cout << "entrou no turno 1" << std::endl;
        rotas[ag].sequencia_am.push_back(0);
        rotas[ag].tempo_am += dist_volta_posto;
        rotas[ag].sequencia_pm.push_back(0);
    
    }else if(turno[ag] == 2){

        //std::cout << "agente " << ag << " entrou no turno 2" << " / ";
        rotas[ag].sequencia_pm.push_back(0);
        rotas[ag].tempo_pm += dist_volta_posto;
        //std::cout << " acrecentando 0: " << dist_volta_posto; 
    } 

    //trocando o turno 
    this->turno[ag] += 1;

    return 0;
}

int Rotas_diarias::dia_encerrado(){
  
    int flag_todos_agentes_encerrados = 1;

    for(int i = 0; i < turno.size(); i++){
        
        if(turno[i] != 3){
            flag_todos_agentes_encerrados = 0;
            break;
        }
    }

    return flag_todos_agentes_encerrados;
}

double Rotas_diarias::get_tempo_total_dia(){

    double t_total_dia = 0;

    for(int i = 0; i < rotas.size(); i++){
        //std::cout << i << "tt:" << rotas[i].tempo_total << std::endl;
        t_total_dia += rotas[i].tempo_am + rotas[i].tempo_pm;
    }

    return t_total_dia;
}

double Rotas_diarias::get_tempo_disponivel_am(int ag){

    return double(MAX_HORAS_AM) - rotas[ag].tempo_am;
}

double Rotas_diarias::get_tempo_disponivel_pm(int ag){

    return double(MAX_HORAS_PM) - rotas[ag].tempo_pm; 
}

void Rotas_diarias::finaliza_turno(int ag, double c_volta_posto){
    
    //std::cout << "finalizou";
    
     if(turno[ag] == 1){
        rotas[ag].sequencia_am.push_back(0);
        rotas[ag].tempo_am += c_volta_posto;

    }else if(turno[ag] == 2){
        rotas[ag].sequencia_pm.push_back(0);
        rotas[ag].tempo_pm += c_volta_posto;
    } 
    
}

int Rotas_diarias::get_turno(int ag){
    
    return turno[ag];
}

void Rotas_diarias::retroceder_turno(int ag){

    turno[ag]--;
}

void Rotas_diarias::avancar_turno(int ag){

    turno[ag]++;
}


void Rotas_diarias::printa_rota(int ag){

    const std::string VERMELHO = "\033[31m";
    const std::string AZUL = "\033[34m";
    const std::string RESET = "\033[0m";

    if(rotas[ag].sequencia_am.size() == 0){
        std::cout << "vazio";
    }else{
        std::cout << "sequencia am: ";
        for(auto i : rotas[ag].sequencia_am){
            std::cout <<  i << " ";
        }
    }
    std::cout << " / ";

    if(rotas[ag].sequencia_pm.size() == 0){
        std::cout << "vazio";
    }else{
        std::cout << "sequencia pm: ";
        for(auto i : rotas[ag].sequencia_pm){
            std::cout <<  i << " ";
        }
    }
    std::cout << std::endl;
}
