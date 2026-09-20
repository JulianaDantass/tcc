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
        
        std::string salvar_solucao_manual = "resultados/solucao_manual_instancia1.csv";
        std::string local_instancia1 = "instancias/instancia1.csv";
        
        Instancia dados;
        Resultados resultado;

        //carrega os dados do csv
        resultado.carregar_csv(local_instancia1);
        //salva solucao manual em csv
        resultado.salvar_solucao_manual_csv(salvar_solucao_manual);

        std::string filename = string(argv[1]);
        int retorno = dados.ler_instancia(filename);
        
        if(retorno != -1){
            Heuristica heuristica(&dados);
            //dados.exibir();
            heuristica.ILS_RVND();
            //heuristica.verifica_fo();
        }
    }

    return 0;
}
