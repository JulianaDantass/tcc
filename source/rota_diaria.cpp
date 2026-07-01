#include "rota_diaria.h"
#include <iostream>
#include <fstream>
#include <unordered_set>

#define INICIO_AM 8
#define INICIO_PM 13
#define FIM_AM 11
#define FIM_PM 16   
#define MAX_HORAS_AM 3
#define MAX_HORAS_PM 4

Rotas_diarias::Rotas_diarias(int n_agentes){

    //eu tenho um valor int associado a cada agente
    turno.resize(n_agentes);    
    
    for(int i = 0; i < n_agentes; i++){
        Rota r;
        r.id_rota = contador_id;
        contador_id++;
        //todos começam e terminam no posto de saude (nó 0)
        r.sequencia.push_back(0);
        r.qtd_pacientes_am = 0;

        //tempo total gasto incialmente é 0        
        r.tempo_total = 0;
        
        //tempos por turno
        r.tempo_am = 0;
        r.tempo_pm = 0;
        
        //colocar no vetor
        rotas.push_back(r);     
        
        turno[i] = 1;
    }
} 

//a posição pertence a [1,n-1] sendo n o tamanho da rota
int Rotas_diarias::adicionar_visita(Paciente &p, int &agente, double &custo_distancia, int posicao){
    
    //retorno q significa que o agente encerrou o dia
    if(turno[agente] == 3){
        return 3; 
    }

    double custo_total = custo_distancia + p.get_duracao();

    //estamos adicionando na manhã
    if(posicao <= rotas[agente].qtd_pacientes_am+1){
        
        int retorno = visita_possivel(agente, custo_total);
        if(retorno == 0){
            rotas[agente].qtd_pacientes_am += 1;
            rotas[agente].tempo_am += custo_total;
        }else{
            return retorno;
        }
             

    //estamos adicionando na tarde
    }else{
        
        int retorno = visita_possivel(agente, custo_total);
        if(retorno == 0){
            rotas[agente].tempo_am += custo_total; 
        }else{
            return retorno;
        }
    }

    rotas[agente].tempo_total += custo_total;
    rotas[agente].sequencia.insert(rotas[agente].sequencia.begin() + posicao, p);
    return 0;
}


int Rotas_diarias::adicionar_visita_fim(Paciente &p, int &agente, double &deslocamento, double &t_volta_posto) {
    
    double t_chegada;
 
    //retorno q significa que o agente encerrou o dia
    if(turno[agente] == 3){
        return 3; 
    }

    //ve se é possivel visitar e voltar pro posto
    double custo_total = deslocamento+p.get_duracao();
    int eh_impossivel = visita_possivel(agente, custo_total);

    //se não é possivel, tem retorno 1 ou 2
    if(eh_impossivel == 1 or eh_impossivel == 2){
        return 1;
    }else{

        //somando o tempo gasto
        rotas[agente].tempo_total += custo_total; 

        //colocando o paciente na rota
        rotas[agente].sequencia.push_back(p);

        if(turno[agente] == 1){
            rotas[agente].qtd_pacientes_am += 1;
            rotas[agente].tempo_am += custo_total;
            

        }else if(turno[agente] == 2){
            rotas[agente].tempo_pm += custo_total;
        } 

        return 0;
    } 
}

int Rotas_diarias::visita_possivel(int agente, double custo_total){

    if(turno[agente] == 1){
               
        if(tempo_disponivel_am(agente) < custo_total){
            return 1; 
        }else{
            return 0;
        }
    }

    if(turno[agente]  == 2){
        if(tempo_disponivel_pm(agente) < custo_total){
            return 2;
        }else{
            return 0;
        }
    }
}

//retorna o ultimo paciente visitado pelo agente
int Rotas_diarias::ultimo_no_rota(int ag){ 
    
    return rotas[ag].sequencia.back().id;  
}

void Rotas_diarias::troca_turno(int ag, double dist_volta_posto){

    //adicionando a volta ao posto
    rotas[ag].sequencia.push_back(0);

    //adicionando o tempo de volta ao posto
    rotas[ag].tempo_total += dist_volta_posto;

    if(turno[ag] == 1){
        rotas[ag].tempo_am += dist_volta_posto;

    }else if(turno[ag] == 2){
        rotas[ag].tempo_pm += dist_volta_posto;
    } 

    //trocando o turno 
    this->turno[ag] += 1;
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

double Rotas_diarias::tempo_disponivel_am(int ag){

    return double(MAX_HORAS_AM) - rotas[ag].tempo_am;
}

double Rotas_diarias::tempo_disponivel_pm(int ag){

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

void Rotas_diarias::exibir_dia(){
    
    for(int i = 0; i < rotas.size(); i++){
        std::cout << "id: " << rotas[i].id_rota << std::endl;
        std::cout << "qtd de pacientes am: " << rotas[i].qtd_pacientes_am << std::endl;
        
        // Definimos uma margem de erro bem pequena
        double epsilon = 0.0001; 
        double tempo_somado = rotas[i].tempo_am + rotas[i].tempo_pm;

        // Verificamos se a diferença entre o total e a soma é MAIOR que a margem de erro
        if (std::abs(rotas[i].tempo_total - tempo_somado) > epsilon) {
            std::cout << "Tempos não batem!" << std::endl;
        }
        
        for(int j = 0; j < rotas[i].sequencia.size(); j++){

            if(j != rotas[i].sequencia.size()-1 ){
                std::cout << rotas[i].sequencia[j].id << ", ";
            }else{
                std::cout << rotas[i].sequencia[j].id << "}, ";
            }    
            
        }
        std::cout << std::endl;
    }
}