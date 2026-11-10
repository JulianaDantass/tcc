#include "heuristica.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <vector>
#include <time.h>
#include <cstdlib> 
#include <iomanip>
#include <fstream>
#include <random>
#include <vector>

using namespace std;

#define QTD_VIZINHANCAS_INTER 2
#define QTD_VIZINHANCAS_INTRA 2
#define MAX_ITER 30


Heuristica::Heuristica(Instancia* dados_atuais){
	
	this->dados = dados_atuais;

}


//DEBUG		
void Heuristica::verifica_fo( Solucao &s){

	double t_total = 0; 
	int qtd_dias = s.get_quantidade_dias();
	auto dias = s.get_dias_planejamento();
	double t_am, t_pm, qtd_am, qtd_pm;
	
	
	for (int i = 0; i < qtd_dias; i++) {
			
		//std::std::cout << "dia " << i  << " - ";
        for (int j = 0; j < dias[i].rotas.size(); j++) {
			//std::std::cout << "a" << j << ": ";
			t_am = 0;
			t_pm = 0;
			qtd_am = 0;
			qtd_pm = 0;
			int tam1 = dias[i].rotas[j].sequencia_am.size(); 
			int tam2 = dias[i].rotas[j].sequencia_pm.size();
			
			if(tam1 > 1){
				//std::cout << dias[i].rotas[j].sequencia_am[0] << " ";
				for (int k = 0; k < dias[i].rotas[j].sequencia_am.size()-1; k++) {
					double t = dados->matriz_tempo[dias[i].rotas[j].sequencia_am[k]][dias[i].rotas[j].sequencia_am[k+1]];
					//std::cout << dias[i].rotas[j].sequencia_am[k+1] << " ";
					t_total += t;
					t_am += t;
				}
				int qtd_zeros = std::count(dias[i].rotas[j].sequencia_am.begin(), dias[i].rotas[j].sequencia_am.end(), 0);
				qtd_am = dias[i].rotas[j].sequencia_am.size()-2;
				t_am += dados->duracao_visita_h* qtd_am;
			}
			
			if(tam2 > 0){
				//std::cout << endl << dias[i].rotas[j].sequencia_pm[0] << " ";
				for (int k = 0; k < dias[i].rotas[j].sequencia_pm.size()-1; k++) {
					
					double t = dados->matriz_tempo[dias[i].rotas[j].sequencia_pm[k]][dias[i].rotas[j].sequencia_pm[k+1]];
					//std::cout << dias[i].rotas[j].sequencia_pm[k+1] << " ";
					//std::cout << dias[i].rotas[j].sequencia_pm[k] << " e " << dias[i].rotas[j].sequencia_pm[k+1] << " " << t << endl;
					
					t_total += t;
					t_pm += t;
					//std::cout << " = " << t << endl;
				}
				
				qtd_pm = dias[i].rotas[j].sequencia_pm.size()-2;
				t_pm += dados->duracao_visita_h* qtd_pm;
				//std::cout << endl;
			}
			
			double dif = std::fabs(t_am - dias[i].rotas[j].tempo_am);
			if(dif > dados->eps){
				std::cout << "dif: " << std::fixed << std::setprecision(10)<< dif << endl;
				std::cout << "dia " << i << " rota " << j << endl;
				//dias[i].printa_rota(j);
				std::cout << "Tempos am não batem -> dia " << i << " rota " << j << endl;
				std::cout << "certo: " << t_am << " calculado: " << dias[i].rotas[j].tempo_am << endl;
				getchar();
			}

			dif = std::fabs(t_pm - dias[i].rotas[j].tempo_pm);
			if(dif > dados->eps){
				std::cout << "dia " << i << " rota " << j << endl;
				//dias[i].printa_rota(j);
				std::cout << "Tempos pm não batem ";
				std::cout << "certo: " << t_pm << " calculado: " << dias[i].rotas[j].tempo_pm << endl;
				getchar();
			}
		}
    } 
	 
	t_total += dados->duracao_visita_h*(dados->quantidade_clientes);	
	double dif = std::fabs(t_total - s.get_fo());
	if (dif > dados->eps){
		std::cout << "Tempos não batem" << endl;
        std::cout << "certo: " << setprecision(4) << t_total << " fo: " << s.get_fo() << endl;
		getchar();        
    }
}

void Heuristica::reorganiza_ult_dia(Solucao &s){
	
	Rotas_diarias &d = s.get_ultimo_dia();
	
	vector<double> horas_por_agente(dados->numero_agentes, 0);
	double horas = 0;

	
	for(int i = 0; i < d.rotas.size(); i++){
		int por_agente = 0;	
		//std::cout << "qtd: " << por_agente << endl;
		horas_por_agente[i] = d.rotas[i].tempo_am + d.rotas[i].tempo_pm;
		horas += d.rotas[i].tempo_am + d.rotas[i].tempo_pm;
	}
	
	double divisao = ceil(horas / double(dados->numero_agentes)); 
	//std::cout << divisao; getchar();
	

	for(int agente_origem = 0; agente_origem < d.rotas.size()-1;agente_origem++){
		
		int agente_destino = 0;
		int tudo_igual = 0;
		while(horas_por_agente[agente_origem] > divisao){
			
			
			//std::cout << "turno: " << d.get_turno(agente_destino) << endl;
			if(horas_por_agente[agente_origem] <= divisao or d.get_turno(agente_destino) == 3){
				agente_destino ++;

				if(agente_destino == dados->numero_agentes){
					tudo_igual = 1; 
					break;
				}
			}
			
			if(tudo_igual){
				break;
			}
			//d.printa_rota(agente_origem);
			//d.printa_rota(agente_destino);
			//std::cout << "horas: " << horas_por_agente[agente_destino] << endl;
			int ult_loc = d.ultimo_no_rota(agente_destino); 
			vector<vector<double>> indice_e_distancia;

			//cout << "tam: " << d.rotas[agente_origem].sequencia_am.size() << endl;
			for (int j = 1; j < d.rotas[agente_origem].sequencia_am.size()-1; j++) {
				
				int id = d.rotas[agente_origem].sequencia_am[j];
				double distancia = dados->matriz_tempo[ult_loc][id];
				indice_e_distancia.push_back({1, j, distancia});
				
			}

			for (int j = 1; j < d.rotas[agente_origem].sequencia_pm.size()-1; j++) {
				
				int id = d.rotas[agente_origem].sequencia_pm[j];
				double distancia = dados->matriz_tempo[ult_loc][id]; 
				indice_e_distancia.push_back({2, j, distancia});
			}


			std::sort(indice_e_distancia.begin(), indice_e_distancia.end(), [](const vector<double>& a, const vector<double>& b) {
        		return a[2] < b[2];
    		});

			std::uniform_real_distribution<double> dist_alfa(0.0, 1.0);
			double alfa = dist_alfa(gen);

			if(indice_e_distancia.size()==0){
				continue;
			}
			size_t limite = std::max<size_t>(1, std::ceil(alfa * indice_e_distancia.size()));

			std::uniform_int_distribution<size_t> dist_indice(0, limite - 1);

			size_t indice_sorteado = dist_indice(gen);

			vector<double> paciente_sorteado = indice_e_distancia[indice_sorteado];
			int turno = (int)paciente_sorteado[0];
			//falta tirar do turno do paciente de origem e ajetar os tempos
			int indice = (int)paciente_sorteado[1];
			
			vector<int>& rota1 = (turno == 1) ? d.rotas[agente_origem].sequencia_am : d.rotas[agente_origem].sequencia_pm;
			double dist_volta_posto = dados->matriz_tempo[rota1[indice]][0];
			int retorno_adicionar = d.adicionar_visita(rota1[indice], agente_destino, paciente_sorteado[2], dist_volta_posto);

			if(retorno_adicionar > 0){
				
				ult_loc = d.ultimo_no_rota(agente_destino);
				dist_volta_posto = dados->matriz_tempo[ult_loc][0];
				horas_por_agente[agente_destino] += dist_volta_posto;
				d.troca_turno(agente_destino, dist_volta_posto);
				continue;
			}
			
			double necessario1 = -dados->duracao_visita_h   
                            +dados->matriz_tempo[rota1[indice-1]][rota1[indice+1]] 
                            -dados->matriz_tempo[rota1[indice-1]][rota1[indice]] 
                            -dados->matriz_tempo[rota1[indice]][rota1[indice+1]];
			
			if(turno == 2 and rota1.size() == 3){
            	d.retroceder_turno(agente_origem);
        	}

			if(turno == 2){
            	//std::cout <<"i no tempo pm\n";
            	d.rotas[agente_origem].tempo_pm += necessario1;
        	} else {
            	//std::cout <<"i no tempo am\n";
            	d.rotas[agente_origem].tempo_am += necessario1;
        	}
			rota1.erase(rota1.begin() + indice);
			horas_por_agente[agente_origem] += necessario1;
			horas_por_agente[agente_destino] += paciente_sorteado[2] + dados->duracao_visita_h;
			
			//d.printa_rota(agente_origem);
			//d.printa_rota(agente_destino);	 
		}
		if(tudo_igual){
			break;
		}
	}
	
}

// o vizinho mais próximo é utilizado como guloso
Solucao Heuristica::solucao_inicial(){

	dados->zera();
	Solucao si(dados->qtd_dias_planejamento);
	int opa = si.get_quantidade_dias();
	//cout << "qtd dias: " << si.get_quantidade_dias(); 
	// Cria um vetor com N elementos, preenchendo todos eles com uma cópia de Rotas_diarias(dados->numero_agentes)
	std::vector<Rotas_diarias> dias(dados->qtd_dias_planejamento, Rotas_diarias(dados->numero_agentes, dados->duracao_visita_h));
	
	//criando um vetor de booleano pra saber se o paciente já foi inserido
	//std::cout << "qtd cliente: " << dados->quantidade_clientes << endl;
	int tam_vetor = (dados->quantidade_clientes+1); 
	std::vector<bool> pacientes_inseridos(tam_vetor, false);

	//criando um vetor com o numero do paciente e a distancia entre o posto e o paciente
	int ult_loc = 0;   		
	//inicializando no primeiro dia 
	int cont_dias = 0;
	Rotas_diarias* dia = &(dias.at(cont_dias));

	
	
	//string saida = "proposta_pos_construcao.txt";
	//si.salva_solucao_arquivo(saida, dados->clientes_prioritarios, dados->clientes_nao_prioritarios);
	//si.exibe_solucao();
	return si;
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

				//std::std::cout << "inter shift: ";
				melhorou =  movimentos.inter_shift10(s);  
				//verifica_fo(s);
				//std::std::cout << melhorou << endl;
				break;
			case 2:
				//std::std::cout << "inter swap22 ";
				melhorou =  movimentos.inter_swap22(s);
				//verifica_fo(s);
				//std::std::cout << melhorou << endl;
				break;
		}
        
        //se melhorou a solucao 
        if (melhorou) {
			
            busca_intra_rota(s, movimentos);
			//std::std::cout<< "saiu da busca intra" << endl;

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

void Heuristica::busca_intra_rota(Solucao &s, Vizinhancas &mov){
	
	bool melhorou_intra = 0;
   	vector<int> NLA;

	for(int i = 1; i <= QTD_VIZINHANCAS_INTRA; i++){
		NLA.push_back(i);
	}
    
    while (!NLA.empty()) {

        int n = rand() % NLA.size();

		switch (NLA[n]){
			 
			case 1:
				melhorou_intra = mov.intra_swap22(s);
				//verifica_fo(s);
				break;
			case 2:
				melhorou_intra = mov.intra_reinsertion(s);
				//verifica_fo(s);
				break;
		}
        //cout << "melhorou: " << melhorou_intra << endl;
        //se melhorou a solucao
        if (melhorou_intra) {
            // não reinicia a lista NL' após encontrar uma melhora.
        } else {
			NLA.erase(NLA.begin() + n);
        }
    } 
}

//the number of successive moves was empirically set to be chosen from the interval {0.5v, 0.6v, . . . , 1.4v, 1.5v}. vai ser um shift 1,0
void Heuristica::perturbacao(Solucao &s){

	
    std::vector<Rotas_diarias>& dias = s.get_dias_planejamento();
    if (dias.empty()) return;

    std::uniform_int_distribution<> distrib;

    // 1. Sorteia dia e rota de origem
    int dia1_sorteado = distrib(gen, decltype(distrib)::param_type(0, dias.size()-1));
    int rota1_sorteada = distrib(gen, decltype(distrib)::param_type(0, dados->numero_agentes - 1));
    int turno1 = distrib(gen, decltype(distrib)::param_type(1, 2));
    auto rota1 = dias[dia1_sorteado].rotas[rota1_sorteada];

    // Proteção: A rota precisa ter pelo menos 3 elementos para poder remover um elemento interno
    while(turno1 == 1 and rota1.sequencia_am.size() < 3){
		dia1_sorteado = distrib(gen, decltype(distrib)::param_type(0, dias.size()-1));
    	rota1_sorteada = distrib(gen, decltype(distrib)::param_type(0, dados->numero_agentes - 1));
		rota1 = dias[dia1_sorteado].rotas[rota1_sorteada];
	} 

	while(turno1 == 2 and rota1.sequencia_pm.size() < 3){
		dia1_sorteado = distrib(gen, decltype(distrib)::param_type(0, dias.size()-1));
    	rota1_sorteada = distrib(gen, decltype(distrib)::param_type(0, dados->numero_agentes - 1));
		rota1 = dias[dia1_sorteado].rotas[rota1_sorteada];
	}

	std::vector<int>* rota1_turno = (turno1 == 1) ? &dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_am 
                                          : &dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_pm; 
    
	int i_sorteado = distrib(gen, decltype(distrib)::param_type(1, rota1_turno->size() - 2));
	
	//debug
	/*
	std::cout << "dia1: " << dia1_sorteado << " rota1: " << rota1_sorteada << " turno1: " << turno1 << " i:" << i_sorteado << endl;
	std::cout << "antes: ";
	dias[dia1_sorteado].printa_rota(rota1_sorteada);
	*/

    int dia2_sorteado = distrib(gen, decltype(distrib)::param_type(0, dias.size()-1));
    int rota2_sorteada = distrib(gen, decltype(distrib)::param_type(0, dados->numero_agentes-1));
    int turno2 = distrib(gen, decltype(distrib)::param_type(1, 2));

	auto rota2 = dias[dia2_sorteado].rotas[rota2_sorteada];

	while(dia2_sorteado == dia1_sorteado and rota1_sorteada == rota2_sorteada and turno1 == turno2){
		dia2_sorteado = distrib(gen, decltype(distrib)::param_type(0, dias.size()-1));
    	rota2_sorteada = distrib(gen, decltype(distrib)::param_type(0, dados->numero_agentes-1));
		rota2 = dias[dia2_sorteado].rotas[rota2_sorteada];
	} 

	while(turno2 == 1 and rota2.sequencia_am.size() < 3){
		dia2_sorteado = distrib(gen, decltype(distrib)::param_type(0, dias.size()-1));
    	rota2_sorteada = distrib(gen, decltype(distrib)::param_type(0, dados->numero_agentes-1));
		rota2 = dias[dia2_sorteado].rotas[rota2_sorteada];
	} 

	while(turno2 == 2 and rota2.sequencia_pm.size() < 3){
		dia2_sorteado = distrib(gen, decltype(distrib)::param_type(0, dias.size()-1));
    	rota2_sorteada = distrib(gen, decltype(distrib)::param_type(0, dados->numero_agentes-1));
		rota2 = dias[dia2_sorteado].rotas[rota2_sorteada];
	} 

	std::vector<int>* rota2_turno = (turno2 == 1) ? &dias[dia2_sorteado].rotas[rota2_sorteada].sequencia_am : &dias[dia2_sorteado].rotas[rota2_sorteada].sequencia_pm; 
    int j_sorteado = distrib(gen, decltype(distrib)::param_type(1, rota2_turno->size() - 1));

	/*
	std::cout << "dia2: " << dia2_sorteado << " rota2: " << rota2_sorteada << " turno2: " << turno2 << " j:" << j_sorteado << endl;
	dias[dia2_sorteado].printa_rota(rota2_sorteada);
	*/

    // 3. Cálculo das variações de tempo
    double necessario1 = -dados->duracao_visita_h   
                        + dados->matriz_tempo[(*rota1_turno)[i_sorteado - 1]][(*rota1_turno)[i_sorteado + 1]] 
                        - dados->matriz_tempo[(*rota1_turno)[i_sorteado - 1]][(*rota1_turno)[i_sorteado]] 
                        - dados->matriz_tempo[(*rota1_turno)[i_sorteado]][(*rota1_turno)[i_sorteado + 1]];

    double necessario2 = dados->duracao_visita_h  
                        - dados->matriz_tempo[(*rota2_turno)[j_sorteado - 1]][(*rota2_turno)[j_sorteado]] 
                        + dados->matriz_tempo[(*rota2_turno)[j_sorteado - 1]][(*rota1_turno)[i_sorteado]]
                        + dados->matriz_tempo[(*rota1_turno)[i_sorteado]][(*rota2_turno)[j_sorteado]];

    

    // 4. Identifica o elemento movido
    int movido = (*rota1_turno)[i_sorteado];

    // 5. Atualizações estruturais e de custos
    if (turno1 == 2 && rota1_turno->size() == 3) {
        dias[dia1_sorteado].retroceder_turno(rota1_sorteada); // ajuste se necessário
    }

    if (turno2 == 2) {
        dias[dia2_sorteado].rotas[rota2_sorteada].tempo_pm += necessario2;
        dias[dia2_sorteado].rotas[rota2_sorteada].sequencia_pm.insert(dias[dia2_sorteado].rotas[rota2_sorteada].sequencia_pm.begin() + j_sorteado, movido);
    } else { 
        dias[dia2_sorteado].rotas[rota2_sorteada].tempo_am += necessario2;
        dias[dia2_sorteado].rotas[rota2_sorteada].sequencia_am.insert(dias[dia2_sorteado].rotas[rota2_sorteada].sequencia_am.begin() + j_sorteado, movido);
    }

    if (turno1 == 2) {
        dias[dia1_sorteado].rotas[rota1_sorteada].tempo_pm += necessario1;
        dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_pm.erase(dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_pm.begin() + i_sorteado);
    } else {
        dias[dia1_sorteado].rotas[rota1_sorteada].tempo_am += necessario1;
        dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_am.erase(dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_am.begin() + i_sorteado);
    }

	/*
	std::cout << "depois rota1: ";
	dias[dia1_sorteado].printa_rota(rota1_sorteada);
	std::cout << "turno1: " << turno1 << " i:" << i_sorteado << endl;
	std::cout << "depois rota2: ";
	dias[dia2_sorteado].printa_rota(rota2_sorteada);
	std::cout << "turno2: " << turno2 << " j: " << j_sorteado << endl;
	//std::cout << "fo atual: " << s.get_fo() << " delta: " << delta;
    */
   	double delta = necessario1 + necessario2;
	s.set_fo(s.get_fo() + delta);
       	
}

//tem que marcar o tempo de execucao total e o tempo gasto na busca local
void Heuristica::ILS_RVND(std::string nome_saida){
	
	Solucao melhor_todas;
	for(int i = 0; i < MAX_ITER; i++){
		Solucao s = solucao_inicial();
    
    	// 2. Aplica a busca local inicial

    	Solucao melhor_s = s;

    	int iter = 0;

		
		int iterILS= dados->quantidade_clientes/4;   	// --- LOOP ITERATIVO (NÃO RECURSIVO) ---
   		while (iter < iterILS) {
        
        // Faz uma cópia para perturbar
        	
			RVND(s);
       		// Aplica a perturbação
			//std::cout << "pertubacao" << endl;

			//std::cout << "saiu pertubacao" << endl;
			//verifica_fo(s);
			//std::cout << "saiu rvnd" << endl;
        
        	// Critério de aceitação (ex: se for melhor que a atual)

        	if (s.get_fo() < (melhor_s.get_fo()-dados->eps)) {
            	melhor_s = s;
				iter = 0;
        	}
			//std::cout << iter << endl;
       		
			Solucao s_linha = s;
			perturbacao(s_linha);
			iter++;
    	}


		if (melhor_s.get_fo() < (melhor_todas.get_fo()-dados->eps)) {
			//cout << "melhorou todas" << endl;
			cout << melhor_s.get_fo() << endl;
            melhor_todas = melhor_s;
        }
	}
		std::cout << "encerrou tudo" << endl;

	melhor_todas.salva_solucao_arquivo(nome_saida);
}




Heuristica::~Heuristica(){
	
}
