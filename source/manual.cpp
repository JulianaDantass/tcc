#include "manual.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <vector>
#include <cmath>

using namespace std;


Manual::Manual(Instancia *d) {
    deposito = 0;
    dados = d;
}

std::string Manual::trim(const std::string& str) {

    size_t primeiro = str.find_first_not_of(" \t\r\n");
    if (primeiro == std::string::npos) return "";
    size_t ultimo = str.find_last_not_of(" \t\r\n");
    return str.substr(primeiro, (ultimo - primeiro + 1));
}

void Manual::printa_microareas() {
   
    for(int i = 0; i < dados->microareas.size(); i++){
        std::cout << "id: " << dados->microareas[i].id << endl;
        
        for(int j = 0; j < dados->microareas[i].clientes.size(); j++){
            std::cout << dados->microareas[i].clientes[j] << " ";
        }
        std::cout << endl;
    }
}

/*
int Manual::ler_csv(string caminho){
    
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
*/
int Manual::salvar_solucao_manual(Solucao &si, std::string nome_saida) {

    std::ofstream arquivo(nome_saida);
    
    if (!arquivo.is_open()) {
        std::cerr << "Erro: Não foi possível criar o arquivo " << nome_saida << ".\n";
        return false;
    }
    for(int i=0; i<dados->microareas.size(); i++){
        std::cout << "microarea " << i << "clientes: " << dados->microareas[i].clientes.size() << std::endl;
    }
    std::vector<Rotas_diarias>& dias = si.get_dias_planejamento();
    int cont_dias_vazios = 0;
    for(int i=0; i < si.get_quantidade_dias(); i++){
        arquivo << "DIA " << i+1 << ":\n";
        
        int cont_rotas_vazias = 0;
        for(int j = 0; j < dias[i].rotas.size(); j++){   
            
            
            arquivo << "agente " << j << ":\n";
            arquivo << "    manhã: ";
            
            double tempo = 0;
            for(int k = 0; k < dias[i].rotas[j].sequencia_am.size(); k++){

                arquivo << dias[i].rotas[j].sequencia_am[k] << ", ";
                if(k == dias[i].rotas[j].sequencia_am.size()-1){
                    continue;
                }else{
                    int id = dias[i].rotas[j].sequencia_am[k];
                    int id_p = dias[i].rotas[j].sequencia_am[k+1];
                    tempo += dados->matriz_tempo[id][id_p];
                }
            }

            if(dias[i].rotas[j].sequencia_am.size() > 1){
                tempo += dados->duracao_visita_h * (dias[i].rotas[j].sequencia_am.size()-2);    
            }
            double dif = std::fabs(tempo- dias[i].rotas[j].tempo_am);
            if(dif > dados->eps){
                cout << "dia e rota: " << i << " e " << j << endl;
                cout << "deu errado am. calculado: " << tempo << " armazenado: " << dias[i].rotas[j].tempo_am; 
            }

            tempo = 0;
            arquivo << "\n    tarde: ";

            for(int k = 0; k < dias[i].rotas[j].sequencia_pm.size(); k++){
                arquivo << dias[i].rotas[j].sequencia_pm[k] << ", ";
                if(k == dias[i].rotas[j].sequencia_pm.size()-1){
                    continue;
                }else{
                    int id = dias[i].rotas[j].sequencia_pm[k];
                    int id_p = dias[i].rotas[j].sequencia_pm[k+1];
                    //cout << id << " e " << id_p << endl;
                    tempo += dados->matriz_tempo[id][id_p];
                }
            }
            if(dias[i].rotas[j].sequencia_pm.size() > 1){
                tempo += dados->duracao_visita_h * (dias[i].rotas[j].sequencia_pm.size()-2);    
            }
            dif = std::fabs(tempo- dias[i].rotas[j].tempo_pm);
            if(dif > dados->eps){
                cout << "dia e rota: " << i << " e " << j << endl;
                cout << "deu errado pm. calculado: " << tempo << " armazenado: " << dias[i].rotas[j].tempo_pm; 
            }
            arquivo << std::endl;
        }
        

        if(cont_rotas_vazias == 5){
            cont_dias_vazios++;
        }
    }

    int qtd_dias = (si.get_quantidade_dias() - cont_dias_vazios);
    int qtd_semanas = qtd_dias / 2; 
    if(qtd_dias % 2 > 0){
        qtd_semanas++;
    }
    arquivo << "FORAM NECESSARIOS EM SEMANAS (considerando que há 2 dias de visita): " << qtd_semanas << "\n";
    arquivo << "FORAM NECESSARIOS EM DIAS: " << qtd_dias << std::endl;
    arquivo << "TEMPO TOTAL (HORAS): " << si.get_fo() << std::endl; 
    
    arquivo.close();
    return true;
}

// Gera uma rota gulosa (Nearest Neighbor) a partir de um ponto de partida
std::vector<int> Manual::gerar_ordem_vizinho_mais_proximo(const std::vector<int>& clientes_microarea) {
    
    std::vector<int> nao_visitados = clientes_microarea;
    std::vector<int> rota_ordenada;
    rota_ordenada.reserve(clientes_microarea.size());

    int no_atual = deposito;

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


int Manual::gerar_solucao_manual(std::string nome_entrada, std::string nome_saida){
    
    //ler_csv(nome_entrada);
    //printa_microareas();
    
    Solucao si(dados->qtd_dias_planejamento);
    std::vector<Rotas_diarias> dias(dados->qtd_dias_planejamento, Rotas_diarias(dados->numero_agentes, dados->duracao_visita_h));

    int cont_servidos = 0;
        // 1. Converte o tamanho para int
    int num_microareas = static_cast<int>(dados->microareas.size());

    // 2. Pré-processamento das filas usando int
    std::vector<std::vector<int>> filas_microareas(num_microareas);
    std::vector<int> pos_cliente(num_microareas, 0);

    
    for (int i = 0; i < num_microareas; i++) {
        filas_microareas[i] = gerar_ordem_vizinho_mais_proximo(dados->microareas[i].clientes);
    }

    // Loop principal dos dias/agentes
    for (int d = 0; d < dados->qtd_dias_planejamento; d++) {
        
        if (cont_servidos >= dados->quantidade_clientes) {
            break;
        }

        Rotas_diarias& dia_atual = dias[d];

        for (int i = 0; i < num_microareas; i++) {
            
            // Armazena o tamanho uma única vez aqui para evitar casts e warnings de signed/unsigned
            if (pos_cliente[i] >= filas_microareas[i].size()) {
                continue;
            }

            bool trocou_turno_hoje = false;

            // Agora o while roda de forma limpa, sem casts poluindo a condição
            while (pos_cliente[i] < filas_microareas[i].size()) {

                int id_cliente = filas_microareas[i][pos_cliente[i]];
                
                int ult_loc = dia_atual.ultimo_no_rota(i);
                double tempo_viagem = dados->matriz_tempo[ult_loc][id_cliente];
                double tempo_retorno = dados->matriz_tempo[id_cliente][deposito]; 

                int status = dia_atual.adicionar_visita(id_cliente, i, tempo_viagem, tempo_retorno);

                if (status == 0) {
                    pos_cliente[i]++;
                    cont_servidos++;
                } else {
                    if (!trocou_turno_hoje) {
                        ult_loc = dia_atual.ultimo_no_rota(i);
                        tempo_retorno = dados->matriz_tempo[ult_loc][deposito];
                        dia_atual.troca_turno(i, tempo_retorno);
                        trocou_turno_hoje = true; 
                    } else {
                        break;
                    }
                }
            }

            int ult_loc = dia_atual.ultimo_no_rota(i);
            if(ult_loc != 0){
                dia_atual.finaliza_turno(i, dados->matriz_tempo[ult_loc][deposito]);
            }
        }
    
        si.adiciona_dia(dia_atual);
    }
    
    si.calcula_fo();
    salvar_solucao_manual(si, nome_saida);

    return 0; // Adicionado para satisfazer o retorno int da função
    
}

       
	
    
    

