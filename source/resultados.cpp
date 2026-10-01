#include "resultados.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

using namespace std;


Resultados::Resultados(Instancia *d) {
    deposito = 0;
    dados = d;
    microareas.resize(dados->numero_agentes+1);
}

std::string Resultados::trim(const std::string& str) {

    size_t primeiro = str.find_first_not_of(" \t\r\n");
    if (primeiro == std::string::npos) return "";
    size_t ultimo = str.find_last_not_of(" \t\r\n");
    return str.substr(primeiro, (ultimo - primeiro + 1));
}

int Resultados::ler_csv(string caminho){
    
    std::ifstream arquivo(caminho);
    std::string linha;
    std::vector<Microarea> microareas;

    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo!\n";
        return 1;
    }

    int i=0;
    while (std::getline(arquivo, linha)) {
        if (linha.empty()) continue;

        std::stringstream ss(linha);
        std::string str_id, rua, str_cliente;

        // Separa os valores por vírgula
        std::getline(ss, str_id, ',');
        std::getline(ss, rua, ',');           
        std::getline(ss, str_cliente, ',');

        int id_microarea = std::stoi(str_id);
        int id_cliente = std::stoi(str_cliente);

        // 1. Procura se a microárea já está cadastrada no vector
        bool encontrada = false;
        if(id_microarea ==0){
            deposito = 0;
            encontrada=true;
        }
        
        for (auto& ma : microareas) {
            if (ma.id == id_microarea) {
                ma.clientes.push_back(i);
                encontrada = true;
                break;
            }
        }

        // 2. Se não existir, cria uma nova
        if (!encontrada) {
            i++;
            microareas.push_back({id_microarea, {i}});
        }
    }

    arquivo.close();

    // Testando a leitura: imprime todas as microáreas e seus clientes
    for (const auto& ma : microareas) {
        std::cout << "Microarea " << ma.id << ": ";
        for (int cliente : ma.clientes) {
            std::cout << cliente << " ";
        }
        std::cout << "\n";
    }
    cout<< "okei" << endl;
    return 0;
}



int Resultados::salvar_solucao_manual_csv(Solucao &si, std::string nome_saida) {

     cout << "entrou" << endl;
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
    Solucao si(dados->qtd_dias_planejamento);

    std::vector<Rotas_diarias> dias(dados->qtd_dias_planejamento, Rotas_diarias(dados->numero_agentes));
    int tam_vetor = dados->quantidade_clientes+1;
	std::vector<bool> pacientes_inseridos(tam_vetor, false);
	int ult_loc = 0;   		 
	int cont_dias = 0;
	Rotas_diarias* dia = &(dias.at(cont_dias));

	//adicionando o paciente prioritario que oferece menor distancia ao agente considerando sua rota 
	int cont_servidos=0;
    int flag_servidos=0;
    int flag_dia_encerrado=0;

    while(1){
		if(cont_servidos >= dados->quantidade_clientes){
			flag_servidos = 1;
			break;
		}
		
		if(dia->dia_encerrado()){
				flag_dia_encerrado = 1;
				break;
		}
		 
        int i;
        for(int i=0; i < microareas.size();i++)//sabendo qual foi a ultima casa visitada pelo agente
		
            int ult_loc = dia->ultimo_no_rota(i);
			double menor_tempo = std::numeric_limits<double>::max();
            int melhor_id = -1;
        
			for (int j=1; j < microareas[i].clientes.size(); j++) {
				
				int id_atual = microareas[i].clientes[j];

				if (!pacientes_inseridos[id_atual]) {
                    double tempo = dados->matriz_tempo[ult_loc][id_atual];
                    if(ult_loc < menor_tempo){
                        menor_tempo = tempo;
                        melhor_id = id_atual;
                    }
				}

                double dist_volta_posto = dados->matriz_tempo[melhor_id][deposito];
                int retorno_adicionar = dia->adicionar_visita(melhor_id, i, menor_tempo, dist_volta_posto);

                //se o retorno 1, 2 ou 3 significa que nao deu certo a inserção pq violaria o horario e o agente teve que retornar ao posto
                if(retorno_adicionar > 0){
                    dia->troca_turno(i, dist_volta_posto);
                    continue;
                }

                cont_servidos++;
                pacientes_inseridos[melhor_id] = true;
			}
			
                //o dia de todos os agentes encerrou
            if(flag_dia_encerrado){
                si.adiciona_dia(dias[cont_dias]);
                cont_dias++;
                dia = &(dias.at(cont_dias));
                flag_dia_encerrado = 0;
            }
            //se todos os clientes prioritarios foram servidos 
            if(flag_servidos){
                break;
            }

		}
		
        si.calcula_fo();
        salvar_solucao_manual_csv(si, nome_saida);
	}
    
    

