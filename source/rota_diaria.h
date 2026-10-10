#pragma once

#include <iostream>
#include <string>
#include <vector>

struct Rota {
    int id_rota;
    int qtd_pacientes_am;
    int qtd_pacientes_pm;
    double tempo_total;
    double tempo_am;
    double tempo_pm;
    std::vector<int> sequencia;

    Rota(int id, int am, int pm, double t_t, double t_am, double t_pm, std::vector<int> s): id_rota(id), qtd_pacientes_am(am), qtd_pacientes_pm(pm), tempo_total(t_t), tempo_am(t_am), tempo_pm(t_pm), sequencia(s) {}
};

//representa 1 dia de trabalho de TODOS agentes
class Rotas_diarias{    

    private: 
        inline static int contador_id = 0;
        //turno[i] informa em qual turno o agente i esteve por ultimo: 1= manha, 2 = tarde, 3 = dia encerrado;
        std::vector<int> turno;
        double duracao_visita_h;            

    public:
        Rotas_diarias(int i, double d);

        //cada vetor representa a rota de um agente. logo, existem N vetores, sendo N a qtd de agentes;    
        std::vector<Rota> rotas;            
        int adicionar_visita(int &paci, int &agente, double &distancia, double &volta_posto);  
        int visita_possivel(int agente, double custo_total);
        int ultimo_no_rota(int ag);
        int dia_encerrado();
        double get_tempo_total_dia();

        double get_tempo_disponivel_am(int ag);
        double get_tempo_disponivel_pm(int ag);
        int get_turno(int ag);
        void finaliza_turno(int ag, double volta_posto);
        int troca_turno(int ag, double dist_volta_posto);
        void retroceder_turno(int ag);
        void avancar_turno(int ag);
        
        void printa_rota(int ag, int a, int b);
        void exibir_dia() const;
};

