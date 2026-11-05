#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector> 
#include <random>
#include "instancia.h"
#include "heuristica.h"
#include "manual.h"


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
        //dados.exibir();
        /*
        Manual manual(&dados);
        manual.gerar_solucao_manual(entrada_manual, saida_manual);
        */
        
        if(ler_instancia != -1){
            Heuristica heuristica(&dados);
            heuristica.ILS_RVND(saida_s_proposta);
            //heuristica.verifica_fo();
        }
        
    }

    return 0;
}
