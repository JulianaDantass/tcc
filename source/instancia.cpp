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
    q_clientes = 0;
}

int Instancia::ler_instancia (string caminho){
    
    // Tentando abrir o arquivo da instancia
    ifstream arquivo;
    arquivo.open(caminho.c_str());

    if(!arquivo){
        cout << "Não foi possivel abrir o arquivo da instancia" << endl; 
        return -1;
    }

    nome_instancia = caminho.erase(0,11);

    std::string linha_arq;
    int i = 0;
    while (std::getline(arquivo, linha_arq)) {
       
        // Pula linhas em branco extras
        if (linha_arq.empty()) {   
            continue; 
        }

        // Troca qualquer simbolo a seguir por espaço ' ' (caso o último seja um fecha-colchetes)
        std::replace(linha_arq.begin(), linha_arq.end(), '[', ' ');
        std::replace(linha_arq.begin(), linha_arq.end(), ']', ' ');   
        std::replace(linha_arq.begin(), linha_arq.end(), ',', ' ');
        
        std::vector<double> linha_matriz;
        std::stringstream ss(linha_arq);
        int valor_int;
        double valor;

        //se estivermos coletando a primeira linha, é a linha de pacientes prioritarios
        if(i == 0){                         
            while (ss >> valor_int) {
                clientes_prioritarios.push_back(valor_int);
            }

        //c.c. é uma linha da matriz que tem vários valores 
        }else{
            while (ss >> valor) {
                
                //como os dados fornecem a distancia, será transformar em tempo considerando uma velociadade de 3km/h
                double tempo = valor/3000.0;    

                linha_matriz.push_back(tempo);
            }
            this->matriz_distancias.push_back(linha_matriz);
        }

        i++; 
    }

    //definindo a qtd de clientes
    q_clientes = matriz_distancias.size();

    //adicionando um vetor com os pacientes que restaram (nao prioritarios)
    
    for (int i = 1; i < matriz_distancias.size(); i++) {
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


    //matriz de debug
    std::ofstream arquivo_saida("instancia_processada.txt");
    if (arquivo_saida.is_open()) {

        for(vector<double> v: matriz_distancias){
            for(double valor : v) {
                arquivo_saida << valor << " "; // Imprime o número e um espaço para separar as colunas
            }
            arquivo_saida << "\n";
        }
        arquivo.close();
    }
    return 1;
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
