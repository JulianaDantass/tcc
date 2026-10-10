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
    
    for(int i=0; i< microareas.size();i++){
        arquivo << "microarea" << microareas[i].id << " possui " << microareas[i].clientes.size() << " clientes.\n";
    }

    
    std::vector<Rotas_diarias>& dias = si.get_dias_planejamento();
    int cont_dias_vazios = 0;
    for(int i=0; i < si.get_quantidade_dias(); i++){
        arquivo << "DIA " << i+1 << ":\n";
        
        int cont_rotas_vazias = 0;
        for(int j = 0; j < dias[i].rotas.size(); j++){            

            arquivo << "agente " << j+1 << ": ";
            for(int k = 0; k < dias[i].rotas[j].sequencia.size(); k++){
                
                if(dias[i].rotas[j].sequencia.size() == 1){
                    cont_rotas_vazias++;
                }else{
                    arquivo << dias[i].rotas[j].sequencia[k] << ", ";
                }
               
            }
            arquivo << endl;
        }

        if(cont_rotas_vazias == 5){
            cont_dias_vazios++;
        }
    }

    arquivo << "FORAM NECESSARIOS EM DIAS: " << (dados->qtd_dias_planejamento - cont_dias_vazios) << "\n";
    arquivo << "TEMPO TOTAL GASTO EM HORAS: " << si.get_fo() << std::endl; 
    
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
            no_atual = proximo_cliente;
            nao_visitados.erase(nao_visitados.begin() + melhor_idx);
        }
    }

    return rota_ordenada;
}


int Resultados::gerar_solucao_manual(std::string nome_entrada, std::string nome_saida){
    
    ler_csv(nome_entrada);
    //printa_microareas();
    
    Solucao si(dados->qtd_dias_planejamento);
    std::vector<Rotas_diarias> dias(dados->qtd_dias_planejamento, Rotas_diarias(dados->numero_agentes, dados->duracao_visita_h));

    int cont_servidos = 0;
        // 1. Converte o tamanho para int
    int num_microareas = static_cast<int>(microareas.size());

    // 2. Pré-processamento das filas usando int
    std::vector<std::vector<int>> filas_microareas(num_microareas);
    std::vector<int> pos_cliente(num_microareas, 0);

    for (int i = 0; i < num_microareas; i++) {
        filas_microareas[i] = gerar_ordem_vizinho_mais_proximo(deposito, microareas[i].clientes);
    }

    // 3. No loop principal dos dias/agentes
    for (int d = 0; d < dados->qtd_dias_planejamento; d++) {
        
        if (cont_servidos >= dados->quantidade_clientes) {
            break;
        }

        Rotas_diarias& dia_atual = dias[d];

        // Loop dos agentes utilizando int
        for (int i = 0; i < num_microareas; i++) {
            
            if (pos_cliente[i] >= static_cast<int>(filas_microareas[i].size())) {
                continue;
            }

            bool trocou_turno_hoje = false;

            while (pos_cliente[i] < static_cast<int>(filas_microareas[i].size())) {
                int id_cliente = filas_microareas[i][pos_cliente[i]];
                
                int ult_loc = dia_atual.ultimo_no_rota(i);
                double tempo_viagem = dados->matriz_tempo[ult_loc][id_cliente];
                double tempo_retorno = dados->matriz_tempo[id_cliente][deposito];

                // Agora 'i' é int e entra perfeitamente na assinatura da função
                int status = dia_atual.adicionar_visita(id_cliente, i, tempo_viagem, tempo_retorno);

                if (status == 0) {
                    pos_cliente[i]++;
                    cont_servidos++;
                } else {
                    if (!trocou_turno_hoje) {
                        dia_atual.troca_turno(i, tempo_retorno);
                        trocou_turno_hoje = true; 
                    } else {
                        break;
                    }
                }
            }
        }

        si.adiciona_dia(dia_atual);
    }
    cout << "servidos/total: " << cont_servidos << "/ " << dados->matriz_tempo.size()-1 << endl;
    si.calcula_fo();
    salvar_solucao_manual(si, nome_saida);
}

       
	
    
    

