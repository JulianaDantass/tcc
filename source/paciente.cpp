#include "paciente.h"
#include <iostream>
#include <fstream>

Paciente::Paciente(){

}

Paciente::Paciente(int i){
    
    this->id = i;
    this->duracao_visita = 0.25;    //0.25 hora ou 15 minutos - foi colocado um tempo padrão 
}

double Paciente::get_duracao(){
    return duracao_visita;
}

