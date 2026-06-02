#include "heuristica.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <vector>
#include <time.h>
#include <cstdlib> 
#include <math.h>
#include <climits>
#include <fstream>
#include <iomanip>
#include <fstream>

using namespace std;

#define MAX_ITER 300
#define QTD_DIAS_PLANEJAMENTO 30
#define QTD_VIZINHANCAS_INTER 3 
#define QTD_VIZINHANCAS_INTRA 3
#define ILS_EXECUCOES 1//50
#define NUMERO_AGENTES 5 

Heuristica::Heuristica(Instancia* dados_atuais){
	
	this->dados = dados_atuais;
	movimentos.set_dados(dados_atuais);

}

// o vizinho mais próximo é utilizado como guloso
Solucao Heuristica::solucao_inicial(){

	Solucao si(QTD_DIAS_PLANEJAMENTO);

	// Cria um vetor com N elementos, preenchendo todos eles com uma cópia de Rotas_diarias(NUMERO_AGENTES)
	std::vector<Rotas_diarias> dias(QTD_DIAS_PLANEJAMENTO, Rotas_diarias(NUMERO_AGENTES));
	
	//criando um vetor de booleano pra saber se o paciente já foi inserido
	//int tam_vetor = (dados->q_clientes);
	int tam_vetor = 307;
	std::vector<bool> pacientes_inseridos(tam_vetor, false);

	//criando um vetor com o numero do paciente e a distancia entre o posto e o paciente
	int ult_loc = 0;   		
	std::vector<std::pair<int, double>> distancias_posto_pacientes;
	
	for(int i = 0; i < dados->clientes_prioritarios.size(); i++){
		int posi = i;
		double distancia = dados->matriz_distancias[ult_loc][dados->clientes_prioritarios[posi].id];
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

		int p = dados->clientes_prioritarios[distancias_posto_pacientes.back().first].id;  

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
    
				
				int id = dados->clientes_prioritarios[j].id;

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
					continue;
				}

				//adicionando que o paciente foi servido
				cont_priori_servidos++;
				pacientes_inseridos[dados->clientes_prioritarios[melhor_indice].id] = true;
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
    
				int id = dados->clientes_nao_prioritarios[j].id;

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
				int retorno_adicionar = dia->adicionar_visita(dados->clientes_nao_prioritarios[melhor_indice], i, melhor_distancia, dist_volta_posto);

				//se o retorno 1, 2 ou 3 significa que nao deu certo a inserção pq violaria o horario e o agente teve que retornar ao posto
				if(retorno_adicionar > 0){
					continue;
				}

				//adicionando que o paciente foi servido
				cont_npriori_servidos++;
				pacientes_inseridos[dados->clientes_nao_prioritarios[melhor_indice].id] = true;
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
				 for(int i = 0; i < NUMERO_AGENTES; i++){
					
				 	int ult_loc = dia->ultimo_no_rota(i);

				 	if(ult_loc != 0){	
						dia->finaliza_rota(i, dados->matriz_distancias[ult_loc][0]);
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

Solucao Heuristica::ILS_RVND(){
	
	solucao_inicial();
	
	for (int i = 1; i <= MAX_ITER; ++i) {
		
		s = solucao_inicial();
		s_local = s;

		int iterILS = 0;
        int MaxIterILS = dados->q_clientes;
        
        while (iterILS <= MaxIterILS) {
            
            RVND(s);
							
            //Critério de aceitação do ILS
            if (s.get_fo() < s_local.get_fo()) {
                s_local = s;
                iterILS = 0;
            }
            
            //s = perturbacao(s_local);
            
            iterILS++;
        }
        
        //Atualização da melhor solução global
        if (s_local.get_fo() < s_global.get_fo()) {
            s_global = s_local;
        }
	}
}

void Heuristica::busca_intra_rota(Solucao s){
	
	bool melhorou_intra;
   	vector<int> NLA;

	for(int i = 1; i <= QTD_VIZINHANCAS_INTRA; i++){
		NLA.push_back(i);
	}
    
    while (!NLA.empty()) {

        int n = rand() % NLA.size();

		switch (NLA[n]){
			 
			case 1:
				//melhorou_intra = movimentos.intra_oropt2(s);
				melhorou_intra = movimentos.intra_exchange(s);
				//melhorou_intra = movimentos.intra_reinsertion(s);
				break;
			case 2:
				//melhorou_intra = movimentos.intra_reinsertion(s);
				break;
			case 3: 
				//melhorou_intra = movimentos.intra_exchange(s);
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

Solucao Heuristica::RVND(Solucao &s){

	bool melhorou = 1;  //esse = 1 deve ser apagado, só está aq para testes
	std::vector<int> NL;
    //UpdateADSs(s);
    
	for(int i = 1; i <= QTD_VIZINHANCAS_INTER; i++){
		NL.push_back(i);
	}
    
    while (!NL.empty()) {
        
		//sorteia uma das vizinhanças
        int n = rand() % NL.size();
		
        //chama a vizinhança sorteada
		
        switch (NL[n]){
			case 1:
				//melhorou =  movimentos.inter_shift10(s);
				//verifica_fo(s);
				break;
			case 2:
				//melhorou =  movimentos.inter_swap11(s);
				break;
			case 3:
				//melhorou =  movimentos.inter_swap22(s);
				break;
		}
        
        //se melhorou a solucao 
        if (melhorou) {
			
			//busca intrarota
            busca_intra_rota(s);

            //Reinicia a lista de vizinhanças para explorar todas novamente
            NL.clear();
            for(int i = 1; i <= QTD_VIZINHANCAS_INTER; i++){
				NL.push_back(i);
			}
		//não melhorou
        } else {
			NL.erase(NL.begin() + n);
        } 
        
        //UpdateADSs(s);
        
    } 
    
    return s;
}

Solucao Heuristica::perturbacao(Solucao s){
	/*
	Multiple-Shift(1,1) – P (2) moves are performed in sequence randomly. The Shift(1,1) consists in transferring a customer k from a route r1 to a
								route r2 , whereas a customer l from r2 is transferred to r1 . In this case, the number of
								moves is randomly selected from the interval {0.5v, 0.6v, . . . , 1.4v, 1.5v}. . This perturbation is more
								stronger than the previous one since it admits a larger number of moves.
								(v é o nº de veiculos)
	*/
}


//DEBUG		
void Heuristica::verifica_fo(Solucao& s){

	cout << "verifica fo" << endl;
	double t_total = 0; 
	int qtd_dias = s.get_quantidade_dias();
	auto dias = s.get_dias_planejamento();
		
	for (int i = 0; i < qtd_dias; i++) {
			
		Rotas_diarias& d = dias.at(i);

        for (int j = 0; j < d.rotas.size(); j++) {	
			for (int k = 0; k < d.rotas[j].sequencia.size()-1; k++) {

				t_total += dados->matriz_distancias[d.rotas[j].sequencia[k].id][d.rotas[j].sequencia[k+1].id];
			}
        }
    } 
	
	t_total += 0.25*(dados->clientes_nao_prioritarios.size()+dados->clientes_prioritarios.size());	
		
	double epsilon = 0.0001; 
	if (std::abs(t_total - s.get_fo()) > epsilon){

		std::cout << "Tempos não batem: ";
        cout << t_total << " " << s.get_fo() << endl;
        
    }else{
		cout << "ok " << endl;
	} 
	getchar();
}

Heuristica::~Heuristica(){
	
}
