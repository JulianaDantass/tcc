#include "rota_diaria.h"
#include <iostream>
#include <fstream>
#include <unordered_set>

   
#define MAX_HORAS_AM 3
#define MAX_HORAS_PM 4
#define DURACAO 0.25
#define EPSILON  0.0001

Rotas_diarias::Rotas_diarias(int n_agentes){

    
    turno.resize(n_agentes); //eu tenho um valor int associado a cada agente   
    
    for(int i = 0; i < n_agentes; i++){
        rotas.push_back({contador_id, 0, 0, 0.0, 0.0, 0.0, std::vector<int>{0}});
        contador_id++;     
        turno[i] = 1;
    }
} 

//deslocamento é a distancia entre o ultimo paciente da rota e o paciente que será adicionado
int Rotas_diarias::adicionar_visita(int &p, int &agente, double &deslocamento, double &t_volta_posto) {
    
    double t_chegada;
 
    //retorno q significa que o agente encerrou o dia
    if(turno[agente] == 3){
        return 1; 
    }

    //ve se é possivel visitar e voltar pro posto
    double custo_total = deslocamento + DURACAO + t_volta_posto;
    int eh_impossivel = visita_possivel(agente, custo_total);

    //se não é possivel, tem retorno 1 ou 2
    if(eh_impossivel > 0){
        return 2;
    }else{

        //somando o tempo gasto
        rotas[agente].tempo_total += custo_total - t_volta_posto; 

        //colocando o paciente na rota
        rotas[agente].sequencia.push_back(p);

        if(turno[agente] == 1){
            rotas[agente].qtd_pacientes_am += 1;
            rotas[agente].tempo_am += custo_total - t_volta_posto;
            

        }else if(turno[agente] == 2){
            rotas[agente].qtd_pacientes_pm += 1;
            rotas[agente].tempo_pm += custo_total - t_volta_posto;
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
    
    return rotas[ag].sequencia.back();  
}

int Rotas_diarias::troca_turno(int ag, double dist_volta_posto){

    //adicionando a volta ao posto
    if(ultimo_no_rota(ag) == 0 ){
        return 1;
        
    }else if(turno[ag] == 3){
        return 2;

    }else{

        //adicionando o tempo de volta ao posto
        rotas[ag].sequencia.push_back(0);
        rotas[ag].tempo_total += dist_volta_posto;

        if(turno[ag] == 1){
            rotas[ag].tempo_am += dist_volta_posto;

        }else if(turno[ag] == 2){
            rotas[ag].tempo_pm += dist_volta_posto;
        } 

        //trocando o turno 
        this->turno[ag] += 1;

        return 0;
    }
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
        t_total_dia += rotas[i].tempo_total;
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
    
    rotas[ag].tempo_total += c_volta_posto; 
    rotas[ag].sequencia.push_back(0);

     if(turno[ag] == 1){
        rotas[ag].tempo_am += c_volta_posto;

    }else if(turno[ag] == 2){
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

    turno[ag]--;
}

void Rotas_diarias::exibir_dia() const{
    
    for(int i = 0; i < rotas.size(); i++){
        std::cout << "id: " << rotas[i].id_rota << std::endl;
        std::cout << "qtd de nós: " << rotas[i].sequencia.size() << std::endl;
    
        for(int j = 0; j < rotas[i].sequencia.size(); j++){
            std::cout << rotas[i].sequencia[j] << ", ";
            
        }
        std::cout << std::endl;
    }
}