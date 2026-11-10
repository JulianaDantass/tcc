#include "instancia.h"
#include <iostream>
#include <fstream>
#include <string>
#include <string>
#include <vector>
#include <algorithm> //Necessário para usar o std::replace
     
Instancia::Instancia(){    
}


int Instancia::ler_microareas(){

   // Substitua "dados.csv" pelo nome real do seu arquivo
    std::ifstream arquivo("instancias/microareas.csv");
    
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo!" << std::endl;
        return 1;
    }

    std::string linha;
    std::string id_anterior = "";
    std::string ultima_linha_conteudo = "";
    int numero_linha = 0;
    int ultima_linha_num = 0;
    int microarea=0;
    microareas.resize(numero_agentes);
    
    while (std::getline(arquivo, linha)) {
        numero_linha++; // Incrementa no começo para registrar a linha exata

        // Ignora linhas vazias
        if (linha.empty()) continue;

        // Usa stringstream para pegar o que está antes da primeira vírgula
        std::stringstream ss(linha);
        std::string id_atual;
        std::getline(ss, id_atual, ','); // Lê até a primeira vírgula

        // Ignora o ID "0" se necessário
        if (id_atual == "0") {
            //std::cout << "entrou" << std::endl;
            continue;
        }

        // Se for a PRIMEIRA linha válida que estamos processando
        if (id_anterior.empty()) {
            microareas[microarea].primeiro_cliente = numero_linha;
            id_anterior = id_atual;
            continue; // Pula para a próxima iteração para não confundir com mudança de ID
        }

        // Se o ID mudou em relação ao anterior
        if (id_atual != id_anterior) {
            // A linha ANTERIOR foi o fim da microárea anterior
            microareas[microarea].ultimo_cliente = numero_linha - 1;
            
            // Avança para a próxima microárea
            microarea++;

            // Verifica se já preencheu todas as microáreas desejadas
            if (microarea >= numero_agentes) {
                break;
            }

            // O a linha ATUAL é o início da nova microárea
            microareas[microarea].primeiro_cliente = numero_linha;
            
            // Atualiza o id_anterior para o novo ID
            id_anterior = id_atual;
        }
    }
    microareas[0].primeiro_cliente = 1;
    microareas[numero_agentes-1].ultimo_cliente = quantidade_clientes;
    //cout << "tam: " <<  microareas.size();
    arquivo.close();
   

    return 0;        
}

int Instancia::ler_instancia (string caminho){
    
    // Tentando abrir o arquivo da instancia
    ifstream arquivo;
    arquivo.open(caminho.c_str());

    if(!arquivo){
        cout << "Não foi possivel abrir o arquivo da instancia" << endl; 
        return -1;
    }
    arquivo >> quantidade_clientes;
    arquivo >> numero_agentes;
    arquivo >> qtd_dias_planejamento;
    arquivo >> duracao_visita_h;

    ler_microareas();

    
        // Consome o '\n' restante na linha do duracao_h antes de usar getline
    std::string linha;
    std::getline(arquivo, linha);
	std::vector<bool> pacientes_priori(quantidade_clientes+1, false);

    for (int i = 0; i < numero_agentes; ++i) {
        if (!std::getline(arquivo, linha)) break;

        std::stringstream ss(linha);
        std::vector<int> linha_ints;
        int val_int;
        
        while (ss >> val_int) {
            if(val_int != 0){
            
                microareas[i].clientes_prioritarios.push_back(val_int);
                pacientes_priori[val_int] = true;
            }
        }
    }
    
    
    // 2. Leitura dos pacientes prioritários
    while (std::getline(arquivo, linha)) {
        if (linha.empty()) continue; 

        std::stringstream ss(linha);
        std::vector<double> linha_atual;

        double valor;
        while (ss >> valor) {
            linha_atual.push_back(valor);
        }

        if (!linha_atual.empty()) {
            matriz_tempo.push_back(linha_atual);
        }
    }
    arquivo.close();

    for(int i = 0; i < numero_agentes; i++) {
        for(int j = microareas[i].primeiro_cliente; j <= microareas[i].ultimo_cliente; j++){
            
            int id = j;
            if (pacientes_priori[id] == false) {
                microareas[i].clientes_nao_prioritarios.push_back(id);
            }
        }        
    }
    
    return 0;
}

int Instancia::todos_servidos_microarea(int m, int priori){

    if(priori == 1){
        //cout << "contador priori/ " << microareas[m].contador_prioritarios << "/" << microareas[m].clientes_prioritarios.size(); getchar();
        if(microareas[m].contador_prioritarios == microareas[m].clientes_prioritarios.size()){
            return 1;
        }else{
            return 0;
        }
    }else{
        if(microareas[m].contador_nao_prioritarios == microareas[m].clientes_nao_prioritarios.size()){
            return 1;
        }else{
            return 0;
        }
    }
    
}

void Instancia::zera(){
    for(Microarea &i: microareas){
        i.contador_nao_prioritarios = 0;
        i.contador_prioritarios = 0;
    }
}


void Instancia::exibir(){

    cout << "número de agentes: " << numero_agentes << endl;
    cout << "qtd total de dias de planejamento: " << qtd_dias_planejamento << endl;
    cout << "qtd clientes: " << quantidade_clientes << endl;
    cout << "duracao: " << duracao_visita_h << endl;
    cout << "exibindo lista de indices de clientes prioritarios:\n";

    int microarea = 0;
    for(Microarea i: microareas) {
        cout << "microarea "  << microarea << endl;
        cout << "primeiro cliente "  << i.primeiro_cliente << endl;
        cout << "ultimo cliente "  << i.ultimo_cliente << endl;
        cout << "prioritarios: ";
        for(int j:i.clientes_prioritarios){
            cout << j << " ";
    
        }        
        cout << "\nn prioritarios: ";
        for(int j:i.clientes_nao_prioritarios){
            cout << j << " ";
        }        
        microarea++;
    }
    
    /*
    cout << "\n exibindo a matriz de distancias: \n";
    for (int i = 0; i < this->matriz_tempo.size(); i++) {
        cout << "------linha " << i+1 << ": " << endl;
        int k = 0;
        for (int j = 0; j < this->matriz_tempo[i].size(); j++) {
            std::cout << std::setw(6) << std::fixed << std::setprecision(3) << this->matriz_tempo[i][j];
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

