#include "paciente.h"
#include <iostream>
#include <fstream>

Paciente::Paciente(){

}

Paciente::Paciente(int i){
    
    this->id = i;
    this->t_inicio_visita = 999999999;
    this->t_termino_visita= 999999999;
    this->duracao_visita = 0.25;    //0.25 hora ou 15 minutos - foi colocado um tempo padrão 
}

double Paciente::get_duracao(){
    return duracao_visita;
}

double Paciente::get_t_inicio_visita(){
    return t_inicio_visita;
}

double Paciente::get_t_termino_visita(){
    return t_termino_visita;
}

void Paciente::set_inicio_visita(double h_chegada_agente) {

    this->t_inicio_visita = h_chegada_agente;
}

void Paciente::set_termino_visita(double h_fim_agente) {

    this->t_termino_visita = h_fim_agente;
}