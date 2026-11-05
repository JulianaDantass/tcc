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
    if (std::getline(arquivo, linha)) {
        std::stringstream ss(linha);
        int id;
        while (ss >> id) {
            if(id == 0){
                cout << "entrou aqui" << endl;
                break;
            }
            clientes_prioritarios.push_back(id);
        }
    }
    cout << "tam clientes priori: " << clientes_prioritarios.size() << endl;
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

    string m = "instancias/microareas.csv";
    ler_csv(m);
    return 0;
}

int Instancia::ler_csv(string caminho){
    
    std::ifstream arquivo(caminho);
    std::string linha;

    if (!arquivo.is_open()) {
        std::cerr << "Erro .csv!\n";
        return 1;
    }

    int deposito;
    int cont = 1; 

    while (getline(arquivo, linha)) {
        if (linha.empty()) continue;

        size_t pos = linha.find(',');
        if (pos == string::npos) continue; // linha sem vírgula

        
        size_t primeira = linha.find(',');
    
        int id = stoi(linha.substr(0, primeira));
        
        if(id == 0){
            deposito = id;
            continue;
        }
    
        auto it = find_if(microareas.begin(), microareas.end(),[id](const Microarea& m) { return m.id == id; });

        if (it != microareas.end()) {
            it->clientes.push_back(cont);        // já existe: só adiciona o número
        } else {
            Microarea m;
            m.id = id;
            m.clientes.push_back(cont);
            microareas.push_back(m); // não existe: cria a microárea
        }
        cont++;

        
    }
    for(int i = 0; i < microareas.size(); i++){
        std::cout << "id: " << microareas[i].id << endl;
        
        for(int j = 0; j < microareas[i].clientes.size(); j++){
            std::cout << microareas[i].clientes[j] << " ";
        }
        std::cout << endl;
    }
    getchar();
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

int Instancia::todos_servidos_microarea(int m){
    if(microareas[m].contador == microareas[m].clientes.size()){
        return 1;
    }else{
        return 0;
    }
}
