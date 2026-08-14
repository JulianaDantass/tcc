#include "heuristica.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <vector>
#include <time.h>
#include <cstdlib> 
#include <math.h>
#include <fstream>
#include <iomanip>
#include <fstream>
#include <random>

using namespace std;

#define MAX_ITER 300
#define QTD_DIAS_PLANEJAMENTO 30
#define QTD_VIZINHANCAS_INTER 3 
#define QTD_VIZINHANCAS_INTRA 3
#define ILS_EXECUCOES 1//50
#define NUMERO_AGENTES 5 
#define DURACAO 0.25
#define EPSILON  0.0001

Heuristica::Heuristica(Instancia* dados_atuais){
	
	this->dados = dados_atuais;

}

// o vizinho mais próximo é utilizado como guloso
Solucao Heuristica::solucao_inicial(){

	Solucao si(QTD_DIAS_PLANEJAMENTO);

	// Cria um vetor com N elementos, preenchendo todos eles com uma cópia de Rotas_diarias(NUMERO_AGENTES)
	std::vector<Rotas_diarias> dias(QTD_DIAS_PLANEJAMENTO, Rotas_diarias(NUMERO_AGENTES));
	
	//criando um vetor de booleano pra saber se o paciente já foi inserido
	int tam_vetor = (dados->q_clientes);
	//int tam_vetor = 307;
	std::vector<bool> pacientes_inseridos(tam_vetor, false);

	//criando um vetor com o numero do paciente e a distancia entre o posto e o paciente
	int ult_loc = 0;   		
	std::vector<std::pair<int, double>> distancias_posto_pacientes;
	
	for(int i = 0; i < dados->clientes_prioritarios.size(); i++){
		int posi = i;
		double distancia = dados->matriz_distancias[ult_loc][dados->clientes_prioritarios[posi]];
		distancias_posto_pacientes.push_back(std::make_pair(posi, distancia));
	}

	//ordenando o vetor de distancias na ordem decrescente		
	std::sort(distancias_posto_pacientes.begin(), distancias_posto_pacientes.end(), [](const auto& a, const auto& b) { return a.second > b.second; });  


	//inicializando no primeiro dia 
	int cont_dias = 0;
	Rotas_diarias* dia = &(dias.at(cont_dias));
	int cont_priori_servidos = 0;

	 //colocando o primeiro cliente (prioritario) de cada agente
	for (int agente = 0; agente < NUMERO_AGENTES; agente++){ 

		int p = dados->clientes_prioritarios[distancias_posto_pacientes.back().first];  

		if(pacientes_inseridos[p] == false){
			double dist_volta_posto = dados->matriz_distancias[p][0];
			int retorno = dia->adicionar_visita(dados->clientes_prioritarios[distancias_posto_pacientes.back().first], agente,
												distancias_posto_pacientes.back().second, dist_volta_posto) ;
			distancias_posto_pacientes.pop_back();

			//retirando o id se o cliente foi servido
			pacientes_inseridos[p] = true;
			cont_priori_servidos++;
		}
	}
	
	//alocar o resto dos pacientes prioritários	
	while(1){
		//pra saber qtos prioritarios ja foram servidos
		bool flag_priori_servidos = 0;
		bool flag_dia_encerrado = 0;

		//adicionando o paciente prioritario que oferece menor distancia ao agente considerando sua rota 
		for(int i = 0; i < NUMERO_AGENTES; i++){
			if(cont_priori_servidos >= dados->clientes_prioritarios.size()){
				flag_priori_servidos = 1;
				break;
			}
			
			if(dia->dia_encerrado()){
				flag_dia_encerrado = 1;
				break;
			}
			//sabendo qual foi a ultima casa visitada pelo agente
			ult_loc = dia->ultimo_no_rota(i);
			
			//criando um paciente p qualquer como o melhor
			double melhor_distancia = std::numeric_limits<double>::max(); 
			int melhor_indice = -1;
			
			for (int j = 0; j < dados->clientes_prioritarios.size(); j++) {
				
				int id = dados->clientes_prioritarios[j];

				if (!pacientes_inseridos[id]) {
					
					double distancia_atual = dados->matriz_distancias[ult_loc][id]; 

					if (distancia_atual < melhor_distancia) {
						melhor_distancia = distancia_atual;
						melhor_indice = j; // Guardamos só a posição, que é muito mais leve!
					}
				}
			}
			
			if(melhor_indice != -1){
			
				double dist_volta_posto = dados->matriz_distancias[dia->ultimo_no_rota(i)][0];
				int retorno_adicionar = dia->adicionar_visita(dados->clientes_prioritarios[melhor_indice], i, melhor_distancia, dist_volta_posto);

				//se o retorno 1, 2 ou 3 significa que nao deu certo a inserção pq violaria o horario e o agente teve que retornar ao posto
				if(retorno_adicionar > 0){
					dia->troca_turno(i, dist_volta_posto);
					continue;
				}

				//adicionando que o paciente foi servido
				cont_priori_servidos++;
				pacientes_inseridos[dados->clientes_prioritarios[melhor_indice]] = true;
			}
		}
		
		//o dia de todos os agentes encerrou
		if(flag_dia_encerrado){
			
			si.adiciona_dia(dias[cont_dias]);
			cont_dias++;
			dia = &(dias.at(cont_dias));
			
		}
		//se todos os clientes prioritarios foram servidos 
		if(flag_priori_servidos){
			break;
		}
	}
	//dia->exibir_dia();
	int cont_npriori_servidos = 0;
	//ALOCAR OS PACIENTES NAO PRIORITÁRIOS
	while(1){
		//é um contador pra saber qtos agentes tem o dia encerrado
		int flag_dia_encerrado = 0;
		bool flag_npriori_servidos = 0;
		
		//adicionando o paciente n prioritario que oferece menor distancia ao agente considerando sua rota 
		for(int i = 0; i < NUMERO_AGENTES; i++){

			if(cont_npriori_servidos >= dados->clientes_nao_prioritarios.size()){
				flag_npriori_servidos = 1;
				break;
			}
			
			if(dia->dia_encerrado()){
				flag_dia_encerrado = 1;
				break;
			}
			//sabendo qual foi a ultima casa visitada pelo agente
			ult_loc = dia->ultimo_no_rota(i);
			
			//criando um paciente p qualquer como o melhor
			double melhor_distancia = std::numeric_limits<double>::max(); 
			int melhor_indice = -1;
			
			for (int j = 0; j < dados->clientes_nao_prioritarios.size(); j++) {
    
				int id = dados->clientes_nao_prioritarios[j];

				if (!pacientes_inseridos[id]) {
					
					double distancia_atual = dados->matriz_distancias[ult_loc][id]; 

					if (distancia_atual < melhor_distancia) {
						melhor_distancia = distancia_atual;
						melhor_indice = j; 
					}
				}
			}
			if(melhor_indice != -1){
			
				double dist_volta_posto = dados->matriz_distancias[dia->ultimo_no_rota(i)][0];
				int retorno_adicionar = dia->adicionar_visita(dados->clientes_nao_prioritarios[melhor_indice], i, melhor_distancia, dist_volta_posto);

				//se o retorno 1, 2 ou 3 significa que nao deu certo a inserção pq violaria o horario e o agente teve que retornar ao posto
				if(retorno_adicionar > 0){
					dia->troca_turno(i, dist_volta_posto);
					continue;
				}

				//adicionando que o paciente foi servido
				cont_npriori_servidos++;
				pacientes_inseridos[dados->clientes_nao_prioritarios[melhor_indice]] = true;
			}
		}
		
		//o dia de todos os agentes encerrou
		if(flag_dia_encerrado){
			
			si.adiciona_dia(dias[cont_dias]);
			cont_dias++;
			dia = &(dias.at(cont_dias));
		}
		
		//se todos os clientes nao prioritarios foram servidos, adiciona o ultimo dia na solução 
		if(flag_npriori_servidos){
				 //adicionamos a volta ao posto caso nao tenha sido adicionada
				for(int j = 0; j < NUMERO_AGENTES; j++){
					
				 	int ult_loc = dia->ultimo_no_rota(j);
				 	if(ult_loc != 0){	
						dia->finaliza_turno(j, dados->matriz_distancias[ult_loc][0]);
				 	}
				 }
			si.adiciona_dia(dias[cont_dias]);
			break;
		}
	}
	//tem que ver a solucao final
	si.calcula_fo();
	//si.exibe_solucao();
	return si;
}

/*
Solucao Heuristica::solucao_inicial(){

	Solucao si(QTD_DIAS_PLANEJAMENTO);

	// Cria um vetor com N elementos, preenchendo todos eles com uma cópia de Rotas_diarias(NUMERO_AGENTES)
	std::vector<Rotas_diarias> dias(QTD_DIAS_PLANEJAMENTO, Rotas_diarias(NUMERO_AGENTES));
	
	//criando um vetor de booleano pra saber se o paciente já foi inserido
	int tam_vetor = (dados->q_clientes);
	std::vector<bool> pacientes_inseridos(tam_vetor, false);

	int cont_dias = 0;
	Rotas_diarias* dia = &(dias.at(cont_dias));
	int cont_priori_servidos = 0;
	std::random_device rd;  
	std::mt19937 gerador(rd()); 

	
	//1.escolher 3 nós aleatórios
	for (int agente = 0; agente < NUMERO_AGENTES; agente++){ 

		double dist_volta_posto;
		//estou colocando 3 aletaórios (dentre priori) para cada agente
		for(int i = 0; i < 3; i++){
			
			std::uniform_int_distribution<> distrib(0, (dados->clientes_prioritarios.size()-1));

			int ind_sorteado = distrib(gerador);
			int cliente_sorteado = dados->clientes_prioritarios[ind_sorteado];

			if(pacientes_inseridos[cliente_sorteado] == false){

				int ult_loc = dia->ultimo_no_rota(agente);
				dist_volta_posto = dados->matriz_distancias[cliente_sorteado][0];
				double dis_ult_no = dados->matriz_distancias[ult_loc][cliente_sorteado];
				int retorno = dia->adicionar_visita_fim(dados->clientes_prioritarios[ind_sorteado], agente, dis_ult_no, dist_volta_posto);

				//descontando o id se o cliente foi servido
				pacientes_inseridos[cliente_sorteado] = true;
				cont_priori_servidos++;
			}
		}
		dia->finaliza_turno(agente, dist_volta_posto);
	}
	
	
	//alocar o resto dos pacientes prioritários	
	while(1){
		//pra saber qtos prioritarios ja foram servidos
		bool flag_priori_servidos = 0;
		bool flag_dia_encerrado = 0;

		//tem que calcular a distancia para cada possivel inserção 
		for(int i = 0; i < NUMERO_AGENTES; i++){

			const auto& rota = dia->rotas[i].sequencia;
			int limite, inicio;
			int turno_atual = dia->get_turno(i);
			cout << "turno atual: " << turno_atual << endl;
			bool trocou_turno = 0;
			
			if(cont_priori_servidos >= dados->clientes_prioritarios.size()){
				flag_priori_servidos = 1;
				break;
			}
			
			if(dia->dia_encerrado()){
				flag_dia_encerrado = 1;
				break;
			}
			
			if(turno_atual == 3){
				continue;
			}

			switch (turno_atual){
				case 1:
					inicio = 1;
					limite = dia->rotas[i].qtd_pacientes_am;
					break;
				case 2:
					inicio = dia->rotas[i].qtd_pacientes_am + 1;
					limite = rota.size();
					break;
			}

			//int qtd_candidatos = dados->clientes_prioritarios.size() - cont_priori_servidos;
			std::vector<InsercaoInfo> custo_insercao;
			
			cout << "inicio: " << inicio << " ";
			cout << "fim: " << limite << endl; 

			for(int j=inicio; j < limite; j++) {
				
				int no_i = rota[j-1];
				int no_j = rota[j];
				
				if(no_i == 0 or no_j == 0){
					continue;
				}

				for (int k=0; k < dados->clientes_prioritarios.size(); k++) {
					int no_k = dados->clientes_prioritarios[k];
					

					if(pacientes_inseridos[no_k] == false){
						double custo = dados->matriz_distancias[no_i][no_k] + dados->matriz_distancias[no_j][no_k] - dados->matriz_distancias[no_i][no_j];
						
						if(dia->visita_possivel(i, custo) == 0){
							double custo = custo;
							int no =  no_k;
							int aresta = j-1;
							custo_insercao.emplace_back(no_k, aresta, custo);
						}else{
							//tem algum problema aqui
							int ult_no = dia->ultimo_no_rota(i);
							dia->troca_turno(i, dados->matriz_distancias[ult_no][0]);
							trocou_turno = 1;
							break;
						}
						
					}
				}

				if(trocou_turno){
					break;
				}
			}
			cout << "saiu1" << endl; 

			if(trocou_turno){
				continue;
			}
			if(custo_insercao.size() > 0){
				std::sort(custo_insercao.begin(), custo_insercao.end(), [](const InsercaoInfo& a, const InsercaoInfo& b) {
					return a.custo < b.custo; //ordem crescente pelo custo
				});

				std::uniform_real_distribution<double> distribuicao(0.0, 1.0);
				double alfa = distribuicao(gerador);
				int sorteado;  
				int id = -1;
				while(1){
					if(id >= 0){
						break;
					}
					int teto = ceil(alfa * custo_insercao.size());
					std::uniform_int_distribution<int> dist(0, teto - 1);
					int sorteado = dist(gerador);
					id = custo_insercao[sorteado].no_candidato;
				}
				int retorno = dia->adicionar_visita(custo_insercao[sorteado].no_candidato, i, custo_insercao[sorteado].custo, custo_insercao[sorteado].aresta_removida);
				if(retorno > 0){
					int ult_no = dia->ultimo_no_rota(i);
					dia->troca_turno(i, dados->matriz_distancias[ult_no][0]);
				}else{
					pacientes_inseridos[custo_insercao[sorteado].no_candidato] = true;   //colocando que o cliente foi servido
					cont_priori_servidos++;
				}
				//debug
			}
			
		}
		cout << "saiu2" << endl; 
		
		//o dia de todos os agentes encerrou
		if(flag_dia_encerrado){
			si.adiciona_dia(dias[cont_dias]);
			cont_dias++;
			dia = &(dias.at(cont_dias));
			
		}
		//se todos os clientes prioritarios foram servidos 
		if(flag_priori_servidos){
			break;
		}	
	}

	si.adiciona_dia(dias[cont_dias]);
	//verifica_fo(si);

	int cont_npriori_servidos = 0;
	//alocar pacientes n prioritários	
	while(1){
		//pra saber qtos prioritarios ja foram servidos
		bool flag_npriori_servidos = 0;
		bool flag_dia_encerrado = 0;

		//tem que calcular a distancia para cada possivel inserção 
		for(int i = 0; i < NUMERO_AGENTES; i++){

			const auto& rota = dia->rotas[i].sequencia;

			if(cont_npriori_servidos >= dados->clientes_nao_prioritarios.size()){
				cout << "servidos: " << cont_npriori_servidos << endl;
				cout << "total: " << dados->clientes_nao_prioritarios.size() << endl;
				flag_npriori_servidos = 1;
				break;
			}
			
			if(dia->dia_encerrado()){
				cout << "entrou aqui2" << endl;
				flag_dia_encerrado = 1;
				break;
			}
			
			std::vector<InsercaoInfo> custo_insercao(rota.size()* dados->clientes_nao_prioritarios.size());
	 		int l = 0;

			
			for(int j=1; j < rota.size(); j++) {
				int no_i = rota[j-1];
				int no_j = rota[j];
				
				if(no_i == 0 or no_j == 0){
					continue;
				}

				for (int k=0; k < dados->clientes_nao_prioritarios.size(); k++) {
					
					int no_k = dados->clientes_nao_prioritarios[k];

					if(!pacientes_inseridos[no_k]){
						double custo = dados->matriz_distancias[no_i][no_k] + dados->matriz_distancias[no_j][no_k] - dados->matriz_distancias[no_i][no_j];
						
						if(dia->visita_possivel(i, custo) == 0){
							custo_insercao[l].custo = custo;
							custo_insercao[l].no_candidato = k;
							custo_insercao[l].aresta_removida = j-1;
							l++;
						}else{
							
							int ult_no = dia->ultimo_no_rota(i);
							dia->troca_turno(i, dados->matriz_distancias[ult_no][0]);
						}
						
					}
				}
			}
			
			std::sort(custo_insercao.begin(), custo_insercao.end(), [](const InsercaoInfo& a, const InsercaoInfo& b) {
				 return a.custo < b.custo; //ordem crescente pelo custo
    		});
			std::uniform_real_distribution<double> distribuicao(0.0, 1.0);
    		double alfa = distribuicao(gerador);
			int selecionado;  
			int id;

			while(id == -1){
				selecionado = rand() % ((int) ceil(alfa * custo_insercao.size()));  
				id = custo_insercao[selecionado].no_candidato;
			}

			dia->adicionar_visita(dados->clientes_nao_prioritarios[id], i, custo_insercao[selecionado].custo, custo_insercao[selecionado].aresta_removida);
			cont_npriori_servidos++;
		}
		
		//o dia de todos os agentes encerrou
		if(flag_dia_encerrado){
			si.adiciona_dia(dias[cont_dias]);
			cont_dias++;
			dia = &(dias.at(cont_dias));
			
		}
		//se todos os clientes prioritarios foram servidos 
		if(flag_npriori_servidos){
			break;
		}
		
	}

}
*/
void Heuristica::printa_solucao(Solucao& s, int dia1, int dia2, int idxRota1, int idxRota2, int m1, int m2){

	const std::string VERMELHO = "\033[31m";
	const std::string RESET = "\033[0m";
	std::vector<Rotas_diarias> dias = s.get_dias_planejamento_estatico();
	cout << endl;
	int k = 0;
    for(Rotas_diarias &d : dias){
		cout << "esse é o dia " << k << endl;
		for(int i = 0; i < d.rotas.size(); i++){
			cout << "rota " << i << ": "; 
			for(int j = 0; j < d.rotas[i].sequencia.size(); j++){
				
				if(k == dia1 and i == idxRota1 and j == m1){
					cout << VERMELHO << d.rotas[i].sequencia[j] << " " << RESET;
				}else if(k == dia2 and i == idxRota2 and j == m2){
					cout << VERMELHO << d.rotas[i].sequencia[j] << " " << RESET;
				}else{
					cout << d.rotas[i].sequencia[j] << " ";
				}
			}
			cout << endl;
		}
		k++;
	}
}

bool Heuristica::validar_rotas(const std::vector<Rotas_diarias>& antigas, const std::vector<Rotas_diarias>& novas, int dia1, int dia2, int idxRota1, int idxRota2){
    // Se a quantidade total de rotas mudou, houve erro
    
	if (antigas.size() != novas.size()) {
        return false;
    }

    for (int i = 0; i < antigas.size(); i++) {
        // Pula as duas rotas que foram alteradas de propósito
		for (int j = 0; j < antigas.size(); j++) {

        	if ( (i == dia1 and j == idxRota1)  or  (i == dia2 and j == idxRota2)) {
            	continue;
        	}

        	// antigas[i] != novas[i] compara o tamanho e todos os elementos da rota i de uma vez
        	if (antigas[i].rotas[j].id_rota != novas[i].rotas[j].id_rota){
            	cout << "erro no id" << endl;
				return false;
        	}
			if (antigas[i].rotas[j].qtd_pacientes_am != novas[i].rotas[j].qtd_pacientes_am){
            	cout << "erro na qtd de paci am" << endl;
				return false;
        	}
			if (antigas[i].rotas[j].qtd_pacientes_pm != novas[i].rotas[j].qtd_pacientes_pm){
            	cout << "erro na qtd de paci pm" << endl;
				return false;
        	}
			if (antigas[i].rotas[j].tempo_total != novas[i].rotas[j].tempo_total){
            	cout << "erro no tempo" << endl;
				return false;
        	}
			if (antigas[i].rotas[j].sequencia != novas[i].rotas[j].sequencia){
            	cout << "erro na seq" << endl;
				return false;
        	}
    
		}
	}

    return true;
}

void Heuristica::ILS_RVND(){
	
	s = solucao_inicial();
	s_local = s;
	s_global = s;
	
	for (int i = 1; i <= MAX_ITER; i++) {
		
		s = solucao_inicial();
		cout << "passou pela veri da solucao inicial" << endl;

		s_local = s;

		int iterILS = 0;
        int MaxIterILS = dados->q_clientes;
        
        while (iterILS <= MaxIterILS) {
            
            RVND(s);
			cout << "saiu do rvnd" << endl;
			//verifica_fo(s);						
            //Critério de aceitação do ILS
            if (s.get_fo() < s_local.get_fo()) {
                s_local = s;
                iterILS = 0;
            }
            perturbacao(s_local);
			cout << "saiu da pert" << endl;
            iterILS++;
        }
        
        //Atualização da melhor solução global
        if (s_local.get_fo() < s_global.get_fo()) {
            s_global = s_local;
        }
	}
}

void Heuristica::busca_intra_rota(Solucao &s, Vizinhancas &mov){
	
	bool melhorou_intra;
   	vector<int> NLA;

	for(int i = 1; i <= QTD_VIZINHANCAS_INTRA; i++){
		NLA.push_back(i);
	}
    
    while (!NLA.empty()) {

        int n = rand() % NLA.size();

		switch (NLA[n]){
			 
			case 1:
				cout << "ia1" << endl;
				melhorou_intra = mov.intra_oropt2(s);
				break;
			case 2:
				cout << "ia2" << endl;
				melhorou_intra = mov.intra_reinsertion(s);
				break;
			case 3:
				cout << "ia3" << endl; 
				melhorou_intra = mov.intra_exchange(s);
				break;
		}
        
        //se melhorou a solucao
        if (melhorou_intra) {
            // não reinicia a lista NL' após encontrar uma melhora.
        } else {
			NLA.erase(NLA.begin() + n);
        }
    } 
}

void Heuristica::RVND(Solucao &s){

	bool melhorou = 1;  //esse = 1 deve ser apagado, só está aq para testes
	std::vector<int> NL;
	Vizinhancas movimentos(dados); 
    
	for(int i = 1; i <= QTD_VIZINHANCAS_INTER; i++){
		NL.push_back(i);
	}
    
    while (!NL.empty()) {
        
		//sorteia uma das vizinhanças
        int n = rand() % NL.size();
		
        //chama a vizinhança sorteada
        switch (NL[n]){
			case 1:  
				cout << "i1" << endl;
				melhorou =  movimentos.inter_shift10(s);  
				break;
			case 2:
				cout << "i2" << endl;
				melhorou =  movimentos.inter_swap11(s);
				break;
			case 3:
				cout << "i3" << endl;
				melhorou =  movimentos.inter_swap22(s);
				break;
		}
        
        //se melhorou a solucao 
        if (melhorou) {
			//busca intrarota
            busca_intra_rota(s, movimentos);

            //Reinicia a lista de vizinhanças para explorar todas novamente
            NL.clear();
            for(int i = 1; i <= QTD_VIZINHANCAS_INTER; i++){
				NL.push_back(i);
			}

		//não melhorou
        } else {
			NL.erase(NL.begin() + n);
        } 
    } 
}

/*
Multiple-Swap(1,1) – P (1) —Multiple random Swap(1,1) moves are performed in
sequence. After some preliminary experiments, the number of successive moves was
empirically set to be chosen from the interval {0.5v, 0.6v, . . . , 1.4v, 1.5v}.
*/
void Heuristica::perturbacao(Solucao &s){

	const std::string AZUL = "\033[34m";
    const std::string RESET = "\033[0m";
    static std::random_device rd; 
    static std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> distrib(5, 15);

    int n_sorteado = distrib(gen);

    double multiplicador = n_sorteado / 10.0;
    int qtd_movimentos = multiplicador * NUMERO_AGENTES;
	std::vector<Rotas_diarias> rotas_antigas = s.get_dias_planejamento();
	std::vector<Rotas_diarias>& rotas_novas = s.get_dias_planejamento();
	
	cout << AZUL << "SERAO REALIZADOS " << qtd_movimentos << " DE SWAPS" << RESET << endl;
	for(int i=0; i < qtd_movimentos; i++){
		//std::mt19937 gen(rd()); 
		std::uniform_int_distribution<> distrib(0, rotas_antigas.size()-1);
		int ind_dia1 = distrib(gen);
		int ind_dia2 = distrib(gen);

		distrib.param(decltype(distrib)::param_type(0, rotas_antigas[ind_dia1].rotas.size()-1));
		int ind_rota1 = distrib(gen);
		distrib.param(decltype(distrib)::param_type(0, rotas_antigas[ind_dia2].rotas.size()-1));
		int ind_rota2 = distrib(gen);
		
		while(1){

            if(rotas_antigas[ind_dia1].rotas[ind_rota1].id_rota != rotas_antigas[ind_dia2].rotas[ind_rota2].id_rota){ 
                break;
            }
			//se forem rotas iguais, vamos sortear novamente
			ind_rota2 = distrib(gen);
		}

		distrib.param(decltype(distrib)::param_type(0, rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia.size()-1));
		int paciente1 = distrib(gen);
		distrib.param(decltype(distrib)::param_type(0, rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia.size()-1));
		int paciente2 = distrib(gen);
		
		while(1){
			if(rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia[paciente1] != 0){
				break;
			}
			//tem que ser -2 pq o último nó é o depósito
			distrib.param(decltype(distrib)::param_type(0, rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia.size()-2));
			paciente1 = distrib(gen);
		}

		while(1){
			if(rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia[paciente2] != 0){
				break;
			}
			//tem que ser -2 pq o último nó é o depósito
			distrib.param(decltype(distrib)::param_type(0, rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia.size()-2));
			paciente2 = distrib(gen);
		}
		cout << "dia: " << ind_dia1  << " rota: " << ind_rota1 << "tamanho da rota: " << rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia.size() << " / escolhido: " << paciente1 << endl;
		cout << "dia: " << ind_dia2  << " rota: " << ind_rota2 << "tamanho da rota: " << rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia.size() << " / escolhido: " << paciente2 << endl;
		double necessario1, necessario2;


		while(1){	
			const auto& rota1 = rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia;
			const auto& rota2 = rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia;
			
			necessario1 += 	dados->matriz_distancias[rota1[paciente1-1]][rota2[paciente2]];
			necessario1 +=	dados->matriz_distancias[rota2[paciente2]][rota1[paciente1+1]];
			necessario1 -=	dados->matriz_distancias[rota1[paciente1-1]][rota1[paciente1]];
			necessario1 -= 	dados->matriz_distancias[rota1[paciente1]][rota1[paciente1+1]];
			
			//o cliente está na tarde
			if(paciente1 > rotas_antigas[ind_dia1].rotas[ind_rota1].qtd_pacientes_am+1){

				//disponível é insuficiente
				if(rotas_antigas[ind_dia1].get_tempo_disponivel_pm(ind_rota1) < necessario1){    

						cout << "[NEC1] não é suficiente. sorteando nova rota entre 0 e " << rotas_antigas[ind_dia1].rotas.size()-1 << "....."; getchar();
						//se o valor é insuficiente então temos que sortear outra rota

						distrib.param(decltype(distrib)::param_type(0, rotas_antigas[ind_dia1].rotas.size()-1));
						ind_rota1 = distrib(gen);
						
						while(rotas_antigas[ind_dia1].rotas[ind_rota1].id_rota == rotas_antigas[ind_dia2].rotas[ind_rota2].id_rota){
							ind_rota1 = distrib(gen);
						}
						cout << "rota: " << ind_dia1 << endl;
						//temos que sortear, consequentemente, outro paciente

						cout << "novo escolhido: ";
						distrib.param(decltype(distrib)::param_type(0, rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia.size()-2));
						paciente1 = distrib(gen);
						
						while(rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia[paciente1] == 0){
							
							distrib.param(decltype(distrib)::param_type(0, rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia.size()-2));
							paciente1 = distrib(gen);
						}
						cout << "novo escolhido1: " << paciente1 << endl;
				}else{
					//disponivel é suficiente
					break;
				}
			
			//o cliente tá na manhã
			}else{

				if(rotas_antigas[ind_dia1].get_tempo_disponivel_am(ind_rota1) < necessario1){    

					cout << "[NEC1] não é suficiente. sorteando nova paci para tarde....";
					//se o valor é insuficiente então temos que forçar o paciente a ser da tarde
					int primeira_posicao_tarde = rotas_antigas[ind_dia1].rotas[ind_rota1].qtd_pacientes_am;
					distrib.param(decltype(distrib)::param_type(primeira_posicao_tarde, rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia.size()-2));	
					paciente1 = distrib(gen);

					while(rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia[paciente1] == 0){
				
						distrib.param(decltype(distrib)::param_type(primeira_posicao_tarde, rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia.size()-2));
						paciente1 = distrib(gen);
					}
					cout << "novo escolhido1: " << paciente1 << endl;
				}else{
					//disponivel é suficiente
					break;
				}
			}
			
		}

		cout << "saiu do loop1" << endl;
		//tem q alterar
		while(1){			
			const auto& rota1 = rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia;
			const auto& rota2 = rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia;

			necessario2 = 	+dados->matriz_distancias[rota2[paciente2-1]][rota1[paciente1]]
							+dados->matriz_distancias[rota1[paciente1]][rota2[paciente2+2]] 
							-dados->matriz_distancias[rota2[paciente2-1]][rota2[paciente2]] 
							-dados->matriz_distancias[rota2[paciente2]][rota2[paciente2+1]] ;
			
			//o cliente está na tarde
			if(paciente2 > rotas_antigas[ind_dia2].rotas[ind_rota2].qtd_pacientes_am+1){

				//disponivel é insuficiente
				if(rotas_antigas[ind_dia2].get_tempo_disponivel_pm(ind_rota2) < necessario2){   

					//se o valor é insuficiente então temos que sortear outra rota
						distrib.param(decltype(distrib)::param_type(0, rotas_antigas[ind_dia2].rotas.size()-1));
						ind_rota2 = distrib(gen);

						while(rotas_antigas[ind_dia1].rotas[ind_rota1].id_rota == rotas_antigas[ind_dia2].rotas[ind_rota2].id_rota){
							ind_rota2 = distrib(gen);
						}
						
						//temos que sortear, consequentemente, outro paciente

						distrib.param(decltype(distrib)::param_type(0, rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia.size()-2));
						paciente2 = distrib(gen);
						
						while(rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia[paciente1] == 0){
							
							distrib.param(decltype(distrib)::param_type(0, rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia.size()-2));
							paciente2 = distrib(gen);
						}
				}else{
					//disponivel é suficiente
					break;
				}
			//o cliente tá na manhã
			}else{

				if(rotas_antigas[ind_dia2].get_tempo_disponivel_pm(ind_rota2) < necessario2){    

					//se o valor é insuficiente então temos que forçar o paciente a ser da tarde
					int primeira_posicao_tarde = rotas_antigas[ind_dia2].rotas[ind_rota2].qtd_pacientes_am;
					distrib.param(decltype(distrib)::param_type(primeira_posicao_tarde, rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia.size()-2));	
					paciente2 = distrib(gen);

					while(rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia[paciente2] == 0){
				
						int primeira_posicao_tarde = rotas_antigas[ind_dia2].rotas[ind_rota2].qtd_pacientes_am;
						distrib.param(decltype(distrib)::param_type(primeira_posicao_tarde, rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia.size()-2));
						paciente2 = distrib(gen);
					}
				}else{
					//disponivel é suficiente
					break;
				}
			}
		}
		cout << "saiu do loop2" << endl;

		necessario1 = 	+dados->matriz_distancias[rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia[paciente1-1]][rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia[paciente2]]
						+dados->matriz_distancias[rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia[paciente2]][rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia[paciente1+1]]
						-dados->matriz_distancias[rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia[paciente1-1]][rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia[paciente1]]
			 			-dados->matriz_distancias[rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia[paciente1]][rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia[paciente1+1]];

		necessario2 = 	+dados->matriz_distancias[rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia[paciente2-1]][rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia[paciente1]]
							+dados->matriz_distancias[rotas_antigas[ind_dia1].rotas[ind_rota1].sequencia[paciente1]][rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia[paciente2+2]] 
							-dados->matriz_distancias[rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia[paciente2-1]][rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia[paciente2]] 
							-dados->matriz_distancias[rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia[paciente2]][rotas_antigas[ind_dia2].rotas[ind_rota2].sequencia[paciente2+1]] ;
		
							//aqui ocorre a mudança --------------------------------------------------------------------------
		cout << "ANTES " << endl;
		printa_solucao(s, ind_dia1, ind_rota1, paciente1, ind_dia2, ind_rota2, paciente2);
		if(paciente2 > rotas_novas[ind_dia2].rotas[ind_rota2].qtd_pacientes_am){

            rotas_novas[ind_dia2].rotas[ind_rota2].tempo_pm += necessario2;
			rotas_novas[ind_dia1].rotas[ind_rota1].qtd_pacientes_pm++;
        }else{ 
            rotas_novas[ind_dia2].rotas[ind_rota2].tempo_am += necessario2;
            rotas_novas[ind_dia2].rotas[ind_rota2].qtd_pacientes_am++;
        }
        rotas_novas[ind_dia2].rotas[ind_rota2].tempo_total += necessario2;


        if(paciente1 > rotas_novas[ind_dia1].rotas[ind_rota1].qtd_pacientes_am){
            
            rotas_novas[ind_dia1].rotas[ind_rota1].tempo_pm += necessario1;
			rotas_novas[ind_dia1].rotas[ind_rota1].qtd_pacientes_pm++;
        }else{

            rotas_novas[ind_dia1].rotas[ind_rota1].tempo_am += necessario1;
            rotas_novas[ind_dia1].rotas[ind_rota1].qtd_pacientes_am--;
        }
        rotas_novas[ind_dia1].rotas[ind_rota1].tempo_total += necessario1;
        
        s.set_fo(s.get_fo() + necessario1 + necessario2 - float(DURACAO) - float(DURACAO));
		std::swap(rotas_novas[ind_dia1].rotas[ind_rota1].sequencia[paciente1], rotas_novas[ind_dia2].rotas[ind_rota2].sequencia[paciente2]);

		cout << "DEPOIS " << endl;
		printa_solucao(s, ind_dia1, ind_rota1, paciente1, ind_dia2, ind_rota2, paciente2);

		if (validar_rotas(rotas_antigas, rotas_novas, ind_dia1, ind_dia2, ind_rota1, ind_rota2)) {
        	std::cout << "OK: As outras rotas estao intactas." << std::endl;
    	} else {
        	std::cout << "ERRO: Houve alteracao indevida em outra rota." << std::endl;
    	}
		//fim do for
	}
}


//DEBUG		
void Heuristica::verifica_fo( Solucao &s){

	cout << "verifica fo" << endl;
	double t_total = 0; 
	int qtd_dias = s.get_quantidade_dias();
	auto dias = s.get_dias_planejamento();
	int qtd_zeros = 0;
	double t_am, t_pm;

	t_am = t_pm = 0;
	int total_pacientes = 0;
		
	//cout << "dias: " << dias.size() << endl;
	for (int i = 0; i < qtd_dias; i++) {
			
		//dias[i].exibir_dia();
        for (int j = 0; j < dias[i].rotas.size(); j++) {
			
			qtd_zeros = 0;
			t_am = 0;
			t_pm = 0;
			for (int k = 0; k < dias[i].rotas[j].sequencia.size()-1; k++) {

				if(dias[i].rotas[j].sequencia[k] == 0 or dias[i].rotas[j].sequencia[k+1] == 0){
					qtd_zeros++;
				}
				double t = dados->matriz_distancias[dias[i].rotas[j].sequencia[k]][dias[i].rotas[j].sequencia[k+1]];
				t_total += t;
				if(qtd_zeros > 2){
					t_pm += t;
				}else{
					//cout << "dist: " << d.rotas[j].sequencia[k] << " e " << d.rotas[j].sequencia[k+1] << endl;
					t_am += t;
				}
			}
			
			//cout << "qtd_paci_pm: " << dias[i].rotas[j].qtd_pacientes_pm << endl;
			t_am += 0.25* (dias[i].rotas[j].qtd_pacientes_am); 
			t_pm += 0.25* (dias[i].rotas[j].qtd_pacientes_pm);
			
			if(t_am - dias[i].rotas[j].tempo_am > double(EPSILON)){
				cout << "Tempos am não batem: ";
				cout << "calculado: " << t_am << " real: " << dias[i].rotas[j].tempo_am << endl;
				cout << "qtd_paci_am: " << dias[i].rotas[j].qtd_pacientes_am << endl;
			}

			if(t_pm - dias[i].rotas[j].tempo_pm > double(EPSILON)){
				cout << "Tempos pm não batem ";
				cout << "calculado: " << t_pm << " real: " << dias[i].rotas[j].tempo_pm << endl;
			}

			total_pacientes += dias[i].rotas[j].qtd_pacientes_am + dias[i].rotas[j].qtd_pacientes_pm;

		}
    } 
	 
	t_total += 0.25*(dados->clientes_nao_prioritarios.size()+dados->clientes_prioritarios.size());	
		
	
	if (abs(t_total - s.get_fo()) > double(EPSILON)){
		cout << "Tempos não batem" << endl;
        cout << "calculado: " << t_total << " real: " << s.get_fo() << endl;        
    }
}

Heuristica::~Heuristica(){
	
}
