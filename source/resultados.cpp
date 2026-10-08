#include "resultados.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <vector>

using namespace std;


Resultados::Resultados(Instancia *d) {
    deposito = 0;
    dados = d;
}

std::string Resultados::trim(const std::string& str) {

    size_t primeiro = str.find_first_not_of(" \t\r\n");
    if (primeiro == std::string::npos) return "";
    size_t ultimo = str.find_last_not_of(" \t\r\n");
    return str.substr(primeiro, (ultimo - primeiro + 1));
}

void Resultados::printa_microareas() {
   
    for(int i = 0; i < microareas.size(); i++){
        std::cout << "id: " << microareas[i].id << endl;
        
        for(int j = 0; j < microareas[i].clientes.size(); j++){
            std::cout << microareas[i].clientes[j] << " ";
        }
        std::cout << endl;
    }
}

int Resultados::ler_csv(string caminho){
    
    std::ifstream arquivo(caminho);
    std::string linha;

    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo!\n";
        return 1;
    }
    
    int cliente_id = 0; // Representa o número da linha (linha 0, linha 1, ...)
    int cont = 1; 

    while (getline(arquivo, linha)) {
        if (linha.empty()) continue;

        size_t pos = linha.find(',');
        if (pos == string::npos) continue; // linha sem vírgula

        
        size_t primeira = linha.find(',');
    
        int id = stoi(linha.substr(0, primeira));
        
        if(id ==0){
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

    return 0;        
}


int Resultados::salvar_solucao_manual(Solucao &si, std::string nome_saida) {


    std::ofstream arquivo(nome_saida);
    
    if (!arquivo.is_open()) {
        std::cerr << "Erro: Não foi possível criar o arquivo " << nome_saida << ".\n";
        return false;
    }
    arquivo << "Solução manual-----------------------------------------------------\n";
    arquivo << "Foram necessários " << si.get_quantidade_dias() << "dias ao total. \n";
    arquivo << "O valor do tempo total (em h) foi de " << si.get_fo() << std::endl; 
    
    std::vector<Rotas_diarias>& dias = si.get_dias_planejamento();
    for(int i=0; i < si.get_quantidade_dias(); i++){
        for(int j = 0; j < dias[i].rotas.size(); j++){
            
            arquivo << "dia " << i+1 << " / agente " << j+1 << ":\n";
            for(int k = 0; k < dias[i].rotas[j].sequencia.size(); k++){
                arquivo << dias[i].rotas[j].sequencia[k] << ", ";
            }
            arquivo << "---\n";
        }
    }
    
    arquivo.close();
    return true;
}


// Gera uma rota gulosa (Nearest Neighbor) a partir de um ponto de partida
std::vector<int> Resultados::gerar_ordem_vizinho_mais_proximo(int no_inicial, const std::vector<int>& clientes_microarea) {
    

    std::vector<int> nao_visitados = clientes_microarea;
    std::vector<int> rota_ordenada;
    rota_ordenada.reserve(clientes_microarea.size());
    int no_atual = no_inicial;

    while (!nao_visitados.empty()) {
        double menor_tempo = std::numeric_limits<double>::max();
        int melhor_idx = -1;

        for (size_t i = 0; i < nao_visitados.size(); i++) {
            int candidato = nao_visitados[i];
            double tempo = dados->matriz_tempo[no_atual][candidato];

            if (tempo < menor_tempo) {
                menor_tempo = tempo;
                melhor_idx = static_cast<int>(i);
            }
        }

        if (melhor_idx != -1) {
            int proximo_cliente = nao_visitados[melhor_idx];
            rota_ordenada.push_back(proximo_cliente);
            no_atual = proximo_cliente; // O próximo vizinho será calculado a partir do cliente recém-adicionado

            // Remove o cliente já ordenado
            nao_visitados.erase(nao_visitados.begin() + melhor_idx);
        }
    }

    return rota_ordenada;
}

int Resultados::gerar_solucao_manual(std::string nome_entrada, std::string nome_saida){
    
    ler_csv(nome_entrada);
    Solucao si(dados->qtd_dias_planejamento);
    std::vector<Rotas_diarias> dias(dados->qtd_dias_planejamento, Rotas_diarias(dados->numero_agentes));

    int cont_servidos = 0;
    for (int i = 0; i < microareas.size(); i++) {
        
        std::vector<int> clientes_ordenados = gerar_ordem_vizinho_mais_proximo(deposito, microareas[i].clientes);
        int dia_atual = 0;

        for (int id_cliente : clientes_ordenados) {
            bool cliente_alocado = false;

            while (dia_atual < dados->qtd_dias_planejamento && !cliente_alocado) {
                
                Rotas_diarias& dia = dias[dia_atual];
                int ult_loc = dia.ultimo_no_rota(i);
                double tempo_viagem = dados->matriz_tempo[ult_loc][id_cliente];
                double tempo_retorno = dados->matriz_tempo[id_cliente][deposito];

                int status = dia.adicionar_visita(id_cliente, i, tempo_viagem, tempo_retorno);

                if (status == 0) {
                    cliente_alocado = true;
                    cont_servidos++;
                } else {
                    
                    bool trocou_turno = dia.troca_turno(i, tempo_retorno);
                    
                    if (!trocou_turno || dia.dia_encerrado()) {
                        si.adiciona_dia(dias[dia_atual]);
                        dia_atual++;
                    }
                }
            }
        }
    }
    si.calcula_fo();
    salvar_solucao_manual(si, nome_saida);
}

       
	
    
    

