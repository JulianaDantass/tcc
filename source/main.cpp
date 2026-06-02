#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector> 
#include "instancia.h"
#include "heuristica.h"

using namespace std;    


int main(int argc, char** argv){
    
    
    if(argc < 2){
        printf("Comando: ./'nome do executavel' instancias/'nome da instancia'\n");

        return 0;

    }else if(argc >= 2){
        
        Instancia dados;
        string filename = string(argv[1]);
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
