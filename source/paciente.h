#pragma once

#include <iostream>
#include <string>
#include <vector>


class Paciente{    

    private:      
        double duracao_visita;
    
        public:
        int id;  //cada nó do mapa = paciente
        Paciente();
        Paciente(int i);
        double get_duracao(void);
};

