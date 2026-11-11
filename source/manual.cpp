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
    inseridos.resize(dados->quantidade_clientes);
    std::fill(inseridos.begin(), inseridos.end(), false);

}

void Manual::printa_microareas() {
   
    /*
    for(int i = 0; i < dados->microareas.size(); i++){
        std::cout << "id: " << dados->microareas[i].id << endl;
        
        for(int j = 0; j < dados->microareas[i].clientes.size(); j++){
            std::cout << dados->microareas[i].clientes[j] << " ";
        }
        std::cout << endl;
    }
    */
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
        std::cout << "microarea " << i << " clientes: " << dados->microareas[i].clientes_prioritarios.size() + dados->microareas[i].clientes_nao_prioritarios.size()<< std::endl;
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
bool Manual::atender_um (Rotas_diarias& dia, int a, bool prioritario) {
        
        auto& micro = dados->microareas[a];
        const std::vector<int>& lista = prioritario ? micro.clientes_prioritarios : micro.clientes_nao_prioritarios;
        int ult_loc = dia.ultimo_no_rota(a);

        // cliente ainda não inserido mais próximo do último nó da rota
        int id = -1;
        double dist = 0;
        for (int candidato : lista) {
            if (inseridos[candidato]) continue;
            double d = dados->matriz_tempo[ult_loc][candidato];
            if (id < 0 || d < dist) {
                id = candidato;
                dist = d;
            }
        }
        if (id < 0) return false;

        int ret = dia.adicionar_visita(id, a, dist, dados->matriz_tempo[id][0]);
        if (ret > 0) {  // não coube: volta ao posto e troca de turno
            int ult = dia.ultimo_no_rota(a);
            dia.troca_turno(a, dados->matriz_tempo[ult][0]);
            return true;
        }
        if (prioritario) micro.contador_prioritarios++;
        else micro.contador_nao_prioritarios++;
        inseridos[id] = true;
        return true;
};


int Manual::gerar_solucao_manual(std::string nome_entrada, std::string nome_saida){
    
    //ler_csv(nome_entrada);
    //printa_microareas();
    Solucao si(dados->qtd_dias_planejamento);
        //cout << dados->microareas[agente_atual].contador_nao_prioritarios; getchar();

    const int n = (int)dados->microareas.size();  // um agente por microárea
    
	std::vector<Rotas_diarias> dias(dados->qtd_dias_planejamento, Rotas_diarias(dados->numero_agentes, dados->duracao_visita_h));
    const int D = (int)dias.size();

    // Tenta inserir no dia 'dia' o cliente mais próximo do agente 'a' (determinístico).
    // false = sem candidatos; true = cliente inserido ou turno trocado por não caber.
    

    int ultimo_dia_usado = 0;
    for (int a = 0; a < n; a++) {  // agente a = microárea a
        int d = 0;                 // dia atual desse agente
        for (bool prioritario : {true, false}) {
            while (d < D && !dados->todos_servidos_microarea(a, prioritario)) {
                Rotas_diarias& dia = dias[d];
                if (dia.get_turno(a) == 3) {  // o dia desse agente terminou
                    d++;
                    continue;
                }

                if (atender_um(dia, a, prioritario) == false){
                    break;
                }
            }
        }
        ultimo_dia_usado = std::max(ultimo_dia_usado, std::min(d, D - 1));
    }

    for (int k = 0; k <= ultimo_dia_usado; k++){
         si.adiciona_dia(dias[k]);
    }

    // volta ao posto no fim de cada rota e cálculo da função objetivo
    for (Rotas_diarias& dia : si.get_dias_planejamento()) {
        for (int j = 0; j < (int)dia.rotas.size(); j++) {
            int id = dia.ultimo_no_rota(j);
            dia.finaliza_turno(j, dados->matriz_tempo[id][0]);
        }
    }
    si.calcula_fo();
    si.salva_solucao_arquivo("resultados/solucao_manual_instancia1.txt");
    int nao_atendidos = 0;  // ids de clientes vão de 1 a N (0 é o posto)
    for (size_t i = 1; i < inseridos.size(); i++)
        if (!inseridos[i]) nao_atendidos++;
    return nao_atendidos;
        

    return 0; // Adicionado para satisfazer o retorno int da função
    
}

       
	
    
    

