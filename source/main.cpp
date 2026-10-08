#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector> 
#include "instancia.h"
#include "heuristica.h"
#include "resultados.h"


int main(int argc, char** argv){
    
    if(argc < 2){
        printf("Comando: ./'nome do executavel' instancias/'nome da instancia'\n");
        return 0;

    }else if(argc >= 2){
        
        std::string saida = "resultados/solucao_manual_instancia1.txt";
        std::string entrada = "instancias/microareas.csv";
        
        Instancia dados;
        std::string filename = string(argv[1]);
        int ler_instancia = dados.ler_instancia(filename);
        //dados.exibir();
        Resultados resultado(&dados);
        //carrega os dados do csv
        resultado.gerar_solucao_manual(entrada, saida);

        
        
        if(ler_instancia != -1){
            //Heuristica heuristica(&dados);
            //dados.exibir();
            //heuristica.ILS_RVND();
            //heuristica.verifica_fo();
        }
    }

    return 0;
}
