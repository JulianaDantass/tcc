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
        //todos começam no posto de saude (nó 0)
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

int Rotas_diarias::adicionar_visita(Paciente &p, int &agente, double &deslocamento, double &t_volta_posto) {
    
    double t_chegada;
 
    //retorno q significa que o agente encerrou o dia
    if(turno[agente] == 3){
        return 3; 
    }
    
    //se o ultimo nó for 0, significa que veio do posto
    if (rotas[agente].sequencia.back().id == 0){
        
        if(turno[agente] == 1){
            t_chegada = INICIO_AM+deslocamento;

        }else if(turno[agente] == 2){
            t_chegada = INICIO_PM+deslocamento;
        } 
        
    //nao veio do posto ent pega o horario de termino da ultima visita
    }else{
        t_chegada = (rotas[agente].sequencia.back().get_t_termino_visita()) + deslocamento;   
    }

    //ve se é possivel visitar e voltar pro posto
    
    int eh_impossivel = visita_possivel(agente, t_chegada, t_volta_posto, p.get_duracao());

    //se não é possivel, tem retorno 1 ou 2
    if(eh_impossivel == 1 or eh_impossivel == 2){
        return 1;
    }else{

        //somando o tempo gasto
        rotas[agente].tempo_total += deslocamento + p.get_duracao(); 

        //colocando o horario de chegada e o de saída da casa do paciente
        double t_termino = t_chegada+ p.get_duracao();
        p.set_inicio_visita(t_chegada);
        p.set_termino_visita(t_termino);
        //colocando o paciente na rota
        rotas[agente].sequencia.push_back(p);

        if(turno[agente] == 1){
            rotas[agente].qtd_pacientes_am += 1;
            rotas[agente].tempo_am += deslocamento + p.get_duracao();

        }else if(turno[agente] == 2){
            rotas[agente].tempo_pm += deslocamento + p.get_duracao();
        } 

        return 0;
    } 
}

int Rotas_diarias::visita_possivel(int agente, double h_chegada_agente, double tempo_volta_posto, double duracao){

    if(turno[agente] == 1){
               
        if(h_chegada_agente+duracao+tempo_volta_posto > FIM_AM){

            troca_turno(agente, tempo_volta_posto);
            return 1; 
        }else{
            return 0;
        }
    }

    if(turno[agente]  == 2){
        if(h_chegada_agente+duracao+tempo_volta_posto > FIM_PM){

            troca_turno(agente, tempo_volta_posto);
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

void Rotas_diarias::finaliza_rota(int ag, double valor){
    
    rotas[ag].tempo_total += valor; 
    rotas[ag].sequencia.push_back(0);

     if(turno[ag] == 1){
        rotas[ag].tempo_am += valor;

    }else if(turno[ag] == 2){
        rotas[ag].tempo_pm += valor;
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