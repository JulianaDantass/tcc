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
#include <random>

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
/*
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
*/
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


	//1.escolher 3 nós aleatórios
	for (int agente = 0; agente < NUMERO_AGENTES; agente++){ 

		double dist_volta_posto;
		//estou colocando 3 aletaórios (dentre priori) para cada agente
		for(int i = 0; i < 3; i++){

			std::random_device rd;  
			std::mt19937 gen(rd()); 
			std::uniform_int_distribution<> distrib(0, (dados->clientes_prioritarios.size()-1));

			int ind_sorteado = distrib(gen);
			int cliente_sorteado = dados->clientes_prioritarios[ind_sorteado].id;

			if(pacientes_inseridos[cliente_sorteado] == false){

				int ult_loc = dia->ultimo_no_rota(agente);
				dist_volta_posto = dados->matriz_distancias[cliente_sorteado][0];
				double dis_ult_no = dados->matriz_distancias[ult_loc][cliente_sorteado];
				int retorno = dia->adicionar_visita_fim(dados->clientes_prioritarios[ind_sorteado], agente, dis_ult_no, dist_volta_posto) ;

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

			if(cont_priori_servidos >= dados->clientes_prioritarios.size()){
				flag_priori_servidos = 1;
				break;
			}
			
			if(dia->dia_encerrado()){
				flag_dia_encerrado = 1;
				break;
			}
			
			int qtd_candidatos = dados->clientes_prioritarios.size() - cont_priori_servidos;
			std::vector<InsercaoInfo> custo_insercao((rota.size() - 1)* qtd_candidatos);
	 		int l = 0;

			for(int j=1; j < rota.size(); j++) {
				int no_i = rota[j-1].id;
				int no_j = rota[j].id;

				for (int k=0; k < dados->clientes_prioritarios.size(); k++) {
					int no_k = dados->clientes_prioritarios[k].id;

					if(!pacientes_inseridos[no_k]){
						double custo = dados->matriz_distancias[no_i][no_k] + dados->matriz_distancias[no_j][no_k] - dados->matriz_distancias[no_i][no_j];
						
						if(dia->visita_possivel(i, custo) == 0){
							custo_insercao[l].custo = custo;
							custo_insercao[l].no_candidato = dados->clientes_prioritarios[k];
							custo_insercao[l].aresta_removida = j-1;
							l++;
						}else{
							int ult_no = dia->ultimo_no_rota(i);
							dia->troca_turno(i, dados->matriz_distancias[ult_no][0]);
						}
						
					}
				}
			}

			std::sort(custo_insercao.begin(), custo_insercao.begin()+l, [](const InsercaoInfo& a, const InsercaoInfo& b) {
				 return a.custo < b.custo; //ordem crescente pelo custo
    		});

			std::random_device rd;  
    		std::mt19937 gerador(rd());
			std::uniform_real_distribution<double> distribuicao(0.0, 1.0);
    		double alfa = distribuicao(gerador);
			int selecionado = rand() % ((int) ceil(alfa * custo_insercao.size()));  
			dia->adicionar_visita(custo_insercao[selecionado].no_candidato, i, custo_insercao[selecionado].custo, custo_insercao[selecionado].aresta_removida);

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

void Heuristica::busca_intra_rota(Solucao &s){
	
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
}

Solucao Heuristica::perturbacao(Solucao &s){
	/*
	Multiple-Swap(1,1)– 
	*/

    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> distrib(5, 15);

    int n_sorteado = distrib(gen);

    double multiplicador = n_sorteado / 10.0;
    int qtd_movimentos = multiplicador * NUMERO_AGENTES;
	std::vector<Rotas_diarias>& dias = s.get_dias_planejamento();

	for(int i=0; i < qtd_movimentos; i++){
		//std::mt19937 gen(rd()); 
		std::uniform_int_distribution<> distrib(0, dias.size()-1);
		int ind_dia1 = distrib(gen);
		int ind_dia2 = distrib(gen);

		std::uniform_int_distribution<> distrib(0, dias[ind_dia1].rotas.size()-1);
		int ind_rota1 = distrib(gen);
		std::uniform_int_distribution<> distrib(0, dias[ind_dia2].rotas.size()-1);
		int ind_rota2 = distrib(gen);

		while(1){

			//se forem rotas diferentes, entao vamos promover  troca 
            if(dias[ind_dia1].rotas[ind_rota1].id_rota != dias[ind_dia2].rotas[ind_rota2].id_rota){ 
                break;
            }
			//std::mt19937 gen(rd());
			ind_rota2 = distrib(gen);
		}

		std::uniform_int_distribution<> distrib(0, dias[ind_dia1].rotas[ind_rota1].sequencia.size()-1);
		int j = distrib(gen);
		std::uniform_int_distribution<> distrib(0, dias[ind_dia2].rotas[ind_rota2].sequencia.size()-1);
		int k = distrib(gen);
		
		while(1){
			if(dias[ind_dia1].rotas[ind_rota1].sequencia[j].id != 0){
				break;
			}
			std::uniform_int_distribution<> distrib(0, dias[ind_dia1].rotas[ind_rota1].sequencia.size()-1);
			j = distrib(gen);
		}

		while(1){
			if(dias[ind_dia2].rotas[ind_rota2].sequencia[k].id != 0){
				break;
			}
			std::uniform_int_distribution<> distrib(0, dias[ind_dia2].rotas[ind_rota2].sequencia.size()-1);
			k = distrib(gen);
		}
		
		const auto& rota1 = dias[ind_dia1].rotas[ind_rota1].sequencia;
		const auto& rota2 = dias[ind_dia2].rotas[ind_rota2].sequencia;
		double necessario1;

		while(1){	
			necessario1 = 	+dados->matriz_distancias[rota1[j-1].id][rota2[k].id]
							+dados->matriz_distancias[rota2[k].id][rota1[j+1].id] 
							-dados->matriz_distancias[rota1[j-1].id][rota1[j].id] 
							-dados->matriz_distancias[rota1[j].id][rota1[j+1].id];
			
			//o cliente está na tarde
			if(j > dias[ind_dia1].rotas[ind_rota1].qtd_pacientes_am+1){

				//disponivel é insuficiente
				if(dias[ind_dia1].tempo_disponivel_pm(ind_rota1) < necessario1){    
					std::uniform_int_distribution<> distrib(0, dias[ind_dia1].rotas[ind_rota1].sequencia.size()-1);
					j = distrib(gen);
				}else{
					//disponivel é suficiente
					break;
				}

			//o cliente tá na manhã
			}else{

				if(dias[ind_dia1].tempo_disponivel_am(ind_rota1) < necessario1){    
					std::uniform_int_distribution<> distrib(0, dias[ind_dia1].rotas[ind_rota1].sequencia.size()-1);
					j = distrib(gen);
				}else{
					//disponivel é suficiente
					break;
				}
			}
		}
		double necessario2;

		while(1){			
			necessario2 = 	+dados->matriz_distancias[rota2[k-1].id][rota1[j].id]
							+dados->matriz_distancias[rota1[j].id][rota2[k+2].id] 
							-dados->matriz_distancias[rota2[k-1].id][rota2[k].id] 
							-dados->matriz_distancias[rota2[k].id][rota2[k+1].id] ;
			
			//o cliente está na tarde
			if(k > dias[ind_dia2].rotas[ind_rota2].qtd_pacientes_am+1){

				//disponivel é insuficiente
				if(dias[ind_dia2].tempo_disponivel_pm(ind_rota2) < necessario2){    
					std::uniform_int_distribution<> distrib(0, dias[ind_dia2].rotas[ind_rota2].sequencia.size()-1);
					k = distrib(gen);
				}else{
					//disponivel é suficiente
					break;
				}

			//o cliente tá na manhã
			}else{

				if(dias[ind_dia2].tempo_disponivel_pm(ind_rota2) < necessario2){    
					std::uniform_int_distribution<> distrib(0, dias[ind_dia2].rotas[ind_rota2].sequencia.size()-1);
					k = distrib(gen);
				}else{
					//disponivel é suficiente
					break;
				}
			}
		}
		
		//efetivar o swap e alterar a fo
		if(k > dias[ind_dia2].rotas[ind_rota2].qtd_pacientes_am){

            dias[ind_dia2].rotas[ind_rota2].tempo_pm += necessario2;
        
        }else{ 
            dias[ind_dia2].rotas[ind_rota2].tempo_am += necessario2;
            dias[ind_dia2].rotas[ind_rota2].qtd_pacientes_am++;
        }
        dias[ind_dia2].rotas[ind_rota2].tempo_total += necessario2;


        if(j > dias[ind_dia1].rotas[ind_rota1].qtd_pacientes_am){
            
            dias[ind_dia1].rotas[ind_rota1].tempo_pm += necessario1;
        }else{

            dias[ind_dia1].rotas[ind_rota1].tempo_am += necessario1;
            dias[ind_dia1].rotas[ind_rota1].qtd_pacientes_am--;
        }
        dias[ind_dia1].rotas[ind_rota1].tempo_total += necessario1;
        
        s.set_fo(s.get_fo() + necessario1 + necessario2 - dias[ind_dia1].rotas[ind_rota1].sequencia[j].get_duracao() 
														- dias[ind_dia2].rotas[ind_rota2].sequencia[k].get_duracao());
		
		std::swap(dias[ind_dia1].rotas[ind_rota1].sequencia[j], dias[ind_dia2].rotas[ind_rota2].sequencia[k]);

		//fim do for
	}
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
