#include "instancia.h"
#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <vector>
#include <algorithm> //Necessário para usar o std::replace
#include <cstdlib>
#include <ctime>
     
Instancia::Instancia(){
    
}

int Instancia::ler_instancia (string caminho){
    
    // Tentando abrir o arquivo da instancia
    ifstream arquivo;
    arquivo.open(caminho.c_str());

    if(!arquivo){
        cout << "Não foi possivel abrir o arquivo da instancia" << endl; 
        return -1;
    }

    std::string linha_arq;
    int i = 0;
    while (std::getline(arquivo, linha_arq)) {
        cout << i << endl;
        // Pula linhas em branco extras
        if (linha_arq.empty()) {   
            continue; 
        }
        std::stringstream ss(linha_arq);
        int valor_int;
        double valor;

        ss >> valor_int;
        numero_agentes = valor_int;
        ss >> valor_int;
        qtd_dias_planejamento = valor_int;
        ss >> valor;
        duracao_visita_h = valor;
    
        std::vector<double> linha_matriz;
        
        //se estivermos coletando a primeira linha, é a linha de pacientes prioritarios
        if(i == 0){                         
            while (ss >> valor_int) {
                clientes_prioritarios.push_back(valor_int);
            }

        }else{
            //c.c. é uma linha da matriz que tem vários valores 
            while (ss >> valor) {
                linha_matriz.push_back(valor);
            }
            this->matriz_tempo.push_back(linha_matriz);
        }

        i++; 
    }

    //definindo a qtd de clientes
    quantidade_clientes = matriz_tempo.size();

    //adicionando um vetor com os pacientes que restaram (nao prioritarios)
    
    for (int i = 1; i < matriz_tempo.size(); i++) {
		int eh_priori = 0;
		for(int j = 0; j < this->clientes_prioritarios.size(); j++){
			if (i == this->clientes_prioritarios[j]){
				eh_priori = 1;
                break;
			}
		}
		if(eh_priori == 0){
			clientes_nao_prioritarios.push_back(i);
		}
    }


    return 0;
}

void Instancia::exibir(){

    cout << "exibindo lista de indices de clientes prioritarios:\n";
    for (int i = 0; i < this->clientes_prioritarios.size(); i++) {
        std::cout << clientes_prioritarios[i] << " ";
    }

    cout << "\nexibindo lista de indices de clientes n prioritarios:\n";
    for (int i = 0; i < this->clientes_nao_prioritarios.size(); i++) {
        std::cout << clientes_nao_prioritarios[i] << " ";
    }
    cout<<endl;
    /*
    cout << "\n exibindo a matriz de distancias: \n";
    for (int i = 0; i < this->matriz_distancias.size(); i++) {
        cout << "------linha " << i+1 << ": " << endl;
        int k = 0;
        for (int j = 0; j < this->matriz_distancias[i].size(); j++) {
            std::cout << std::setw(6) << std::fixed << std::setprecision(3) << this->matriz_distancias[i][j];
            k++;
            if(k == 21){
                cout << endl;
                k = 0;
            }
        }
        cout << endl;
    }
    */
}
