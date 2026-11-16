#include <stdio.h>
#include <iostream>
#include <random>
#include "instancia.h"
#include "heuristica.h"
#include "vpm.h"


int main(int argc, char** argv){
    
    if(argc < 2){
        printf("Comando: ./'nome do executavel' instancias/'nome da instancia'\n");
        return 0;

    }else if(argc >= 2){
        
        std::string entrada_manual = "instancias/microareas.csv";
        std::string saida_manual = "resultados/solucao_manual_instancia1.txt";
        std::string saida_s_proposta = "resultados/solucao_proposta_instancia1.txt";

        Instancia dados;
        std::string filename = string(argv[1]);
        int ler_instancia = dados.ler_instancia(filename);
        dados.exibir();
        
        /*
       Manual manual(&dados);
       manual.gerar_solucao_manual(entrada_manual, saida_manual);
       */
       
        int num_execucoes=10;
       std::string nome_instancia = "resultados/solucao_proposta_";
       for (int i = 1; i <= num_execucoes; ++i) {
            std::string id_execucao = nome_instancia + "_tentativa_" + std::to_string(i) + ".txt";
            Heuristica heuristica(&dados);
            heuristica.ILS_RVND(id_execucao);
            //heuristica.verifica_fo();
        }

    }

    return 0;
}
