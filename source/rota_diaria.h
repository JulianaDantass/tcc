#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "paciente.h"

struct Rota {
    int id_rota;
    double tempo_total;
    std::vector<Paciente> sequencia;
    int qtd_pacientes_am;
    double tempo_am;
    double tempo_pm;
};

//representa 1 dia de trabalho de TODOS agentes
class Rotas_diarias{    

    private: 
        inline static int contador_id = 0;
        //turno[i] informa em qual turno o agente i esteve por ultimo: 1= manha, 2 = tarde, 3 = dia encerrado;
        std::vector<int> turno;            
        

    public:
        Rotas_diarias(int i);

        //cada vetor representa a rota de um agente. logo, existem N vetores, sendo N a qtd de agentes;    
        std::vector<Rota> rotas;            

        int adicionar_visita(Paciente &paci, int &agente, double &distancia, double &volta_posto);  
        int visita_possivel(int agente, double h_chegada_agente, double tempo_volta_posto, double duracao);
        int ultimo_no_rota(int ag);
        int dia_encerrado();

        double get_tempo_total_dia();
        double tempo_disponivel_am(int ag);
        double tempo_disponivel_pm(int ag);

        void finaliza_rota(int ag, double valor);
        void troca_turno(int ag, double dist_volta_posto);
        void exibir_dia();
};

