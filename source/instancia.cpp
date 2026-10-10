#include "instancia.h"
#include <iostream>
#include <fstream>
#include <string>
#include <string>
#include <vector>
#include <algorithm> //Necessário para usar o std::replace
     
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

    arquivo >> numero_agentes;
    arquivo >> qtd_dias_planejamento;
    arquivo >> duracao_visita_h;

    // Consome o '\n' restante na linha do duracao_h antes de usar getline
    std::string linha;
    std::getline(arquivo, linha);

    // 2. Leitura dos pacientes prioritários
    // Se a quantidade de linhas for igual a N_AGENTES:
    for (int i = 0; i < numero_agentes; ++i) {
        
        std::stringstream ss(linha);
        std::vector<int> linha_pacientes;
        int val;
        while (ss >> val) {
            clientes_prioritarios.push_back(val);   
        }
        
    }
    
    if (std::getline(arquivo, linha)) {
        std::stringstream ss(linha);
        int id;
        while (ss >> id) {
            clientes_prioritarios.push_back(id);
        }
    }
    // 3. Leitura da matriz (lê todas as linhas restantes até o fim do arquivo)
    while (std::getline(arquivo, linha)) {
        if (linha.empty()) continue;

        std::stringstream ss(linha);
        std::vector<double> linha_matriz;
        double val;
        while (ss >> val) {
            linha_matriz.push_back(val);
        }
        
        if (!linha_matriz.empty()) {
            matriz_tempo.push_back(linha_matriz);
        }
    }

    quantidade_clientes = matriz_tempo.size();
    for(int i = 1; i < quantidade_clientes; i++) {
        
        if (std::find(clientes_prioritarios.begin(), clientes_prioritarios.end(), i) == clientes_prioritarios.end()) {
            clientes_nao_prioritarios.push_back(i);
        }
    }
    
    return 0;
}

void Instancia::exibir(){
    cout << "número de agentes: " << numero_agentes << endl;
    cout << "qtd total de dias de planejamento: " << qtd_dias_planejamento << endl;
    cout << "qtd clientes: " << quantidade_clientes << endl;
    cout << "duracao: " << duracao_visita_h << endl;
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
