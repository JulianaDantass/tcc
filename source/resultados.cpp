#include "resultados.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>

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
        std::cout << "qtd clientes: " << microareas[i].qtd_clientes << endl;
        
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

    for(int i=0; i < microareas.size(); i++){
        microareas[i].qtd_clientes = microareas[i].clientes.size();
    }

    return 0;        
}


int Resultados::salvar_solucao_manual_csv(Solucao &si, std::string nome_saida) {


    std::ofstream arquivo(nome_saida);
    
    if (!arquivo.is_open()) {
        std::cerr << "Erro: Não foi possível criar o arquivo " << nome_saida << ".\n";
        return false;
    }
    arquivo << "Solução manual--------------------\n";
    arquivo << "Foram necessários " << si.get_quantidade_dias() << "dias" << std::endl;
    arquivo << "O valor do tempo total (em h) foi de " << si.get_fo() << std::endl; 
    
    std::vector<Rotas_diarias>& dias = si.get_dias_planejamento();
    for(int i=0; i < si.get_quantidade_dias(); i++){

        arquivo << "Foram necessários " << si.get_quantidade_dias() << "dias" << std::endl;
        arquivo << "O valor do tempo total (em h) foi de " << si.get_fo() << std::endl; 
        
        for(int j = 0; j < dias[i].rotas.size(); j++){
            
            arquivo << "dia " << j+1 << ":\n";
            for(int k = 0; k < dias[i].rotas[j].sequencia.size(); k++){
                arquivo << dias[i].rotas[j].sequencia[k] << ", ";
            }
            arquivo << "---\n";
        }
    }
    
    arquivo.close();
    return true;
}


int Resultados::gerar_solucao_manual(std::string nome_entrada, std::string nome_saida){

    ler_csv(nome_entrada);
    //printa_microareas();
    
    Solucao si(dados->qtd_dias_planejamento);
    std::vector<Rotas_diarias> dias(dados->qtd_dias_planejamento, Rotas_diarias(dados->numero_agentes));
    int tam_vetor = (dados->quantidade_clientes+1);
	std::vector<bool> pacientes_inseridos(tam_vetor, false);

	//adicionando o paciente prioritario que oferece menor distancia ao agente considerando sua rota 
	int cont_dias=0;
    int cont_servidos=0;
    Rotas_diarias* dia = &(dias.at(cont_dias));
    int it=0;

    while(1){
        bool flag_servidos = 0;
		bool flag_dia_encerrado = 0;
        cout << "iteracao " << it << "/ servidos " << cont_servidos << endl;
        for (int i=0; i < microareas.size(); i++){
            
            if(cont_servidos >= dados->quantidade_clientes){
			    flag_servidos = 1;
			    break;
		    }
			
		    if(dia->dia_encerrado()){
			    flag_dia_encerrado = 1;
			    break;
		    }

            if(microareas[i].qtd_clientes <= 0){
                continue;
            }

            int ult_loc = dia->ultimo_no_rota(i);
            double menor_tempo = std::numeric_limits<double>::max();
            int melhor_id = -1;

            for (int j=0; j < microareas[i].clientes.size(); j++){
                int id_atual = microareas[i].clientes[j];

                if (pacientes_inseridos[id_atual] == false) {
                    double tempo = dados->matriz_tempo[ult_loc][id_atual];

                    if (tempo < menor_tempo) {
                        menor_tempo = tempo;
                        melhor_id = id_atual;
                    }
                }
            }
            cout << "melhor id: " << melhor_id << endl;
            
            double dist_volta_posto = dados->matriz_tempo[melhor_id][deposito];
            int retorno_adicionar = dia->adicionar_visita(melhor_id, i, menor_tempo, dist_volta_posto);

            //se o retorno 1, 2 ou 3 significa que nao deu certo a inserção pq violaria o horario e o agente teve que retornar ao posto
            if(retorno_adicionar > 0){
                dia->troca_turno(i, dist_volta_posto);
                continue;
            }
            microareas[i].qtd_clientes -= 1;
            cont_servidos++;
			pacientes_inseridos[melhor_id] = true;
        }

        //o dia de todos os agentes encerrou
		if(flag_dia_encerrado){
			si.adiciona_dia(dias[cont_dias]);
			cont_dias++;
			dia = &(dias.at(cont_dias));
			
		}
		//se todos os clientes prioritarios foram servidos 
		if(flag_servidos){
			break;
		}
        it++;
    }


		//std::cout << "encerrou o o laço---------------" << endl;
        //si.calcula_fo();
        //salvar_solucao_manual_csv(si, nome_saida);            //o dia de todos os agentes encerrou    

}
		
       
	
    
    

