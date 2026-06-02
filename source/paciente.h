#pragma once

#include <iostream>
#include <string>
#include <vector>


class Paciente{    

    private: 
        
        double t_inicio_visita;
        double t_termino_visita;          
        double duracao_visita;
    
        public:
        int id;  //cada nó do mapa = paciente
        Paciente();
        Paciente(int i);
        double get_duracao(void);
        double get_t_inicio_visita(void);
        double get_t_termino_visita(void);
        void set_inicio_visita(double h_chegada_agente);
        void set_termino_visita(double h_termino_agente);
};

