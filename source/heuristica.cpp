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

#define EPSILON  0.0001
#define QTD_VIZINHANCAS_INTER 3
#define QTD_VIZINHANCAS_INTRA 3
#define MAX_ITER 300


Heuristica::Heuristica(Instancia* dados_atuais){
	
	this->dados = dados_atuais;

}

// o vizinho mais próximo é utilizado como guloso
Solucao Heuristica::solucao_inicial(){

	Solucao si(dados->qtd_dias_planejamento);

	// Cria um vetor com N elementos, preenchendo todos eles com uma cópia de Rotas_diarias(dados->numero_agentes)
	std::vector<Rotas_diarias> dias(dados->qtd_dias_planejamento, Rotas_diarias(dados->numero_agentes, dados->duracao_visita_h));
	
	//criando um vetor de booleano pra saber se o paciente já foi inserido
	int tam_vetor = (dados->quantidade_clientes);
	//int tam_vetor = 307;
	std::vector<bool> pacientes_inseridos(tam_vetor, false);

	//criando um vetor com o numero do paciente e a distancia entre o posto e o paciente
	int ult_loc = 0;   		
	//inicializando no primeiro dia 
	int cont_dias = 0;
	Rotas_diarias* dia = &(dias.at(cont_dias));
	int cont_priori_servidos = 0;
	std::random_device rd;
    std::mt19937 gen(rd());
	//alocar o resto dos pacientes prioritários	
	while(1){
		//pra saber qtos prioritarios ja foram servidos
		bool flag_priori_servidos = 0;
		bool flag_dia_encerrado = 0;

		//adicionando o paciente prioritario que oferece menor distancia ao agente considerando sua rota 
		for(int i = 0; i < dados->numero_agentes; i++){
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
			vector<pair<int,double>> indice_e_distancia;
			
			for (int j = 0; j < dados->clientes_prioritarios.size(); j++) {
				
				int id = dados->clientes_prioritarios[j];

				if (!pacientes_inseridos[id]) {
					double distancia = dados->matriz_tempo[ult_loc][id]; 
					indice_e_distancia.push_back(pair<int,double>(j, distancia));
				}
			}
			
			//ordenando crescente
			std::sort(indice_e_distancia.begin(), indice_e_distancia.end(), [](const pair<int,double>& a, const pair<int,double>& b) {
        		return a.second < b.second;
    		});

			std::uniform_real_distribution<double> dist_alfa(0.0, 1.0);
			double alfa = dist_alfa(gen);

			// 3. Selecionar aleatoriamente um dos (alfa * |Ômega|) primeiros pares
			size_t limite = std::max<size_t>(1, std::ceil(alfa * indice_e_distancia.size()));
			
			// Distribuição para escolher um índice inteiro aleatório entre 0 e (limite - 1)
			std::uniform_int_distribution<size_t> dist_indice(0, limite - 1);
			size_t indice_sorteado = dist_indice(gen);

			pair<int,double> paciente_sorteado = indice_e_distancia[indice_sorteado];
			
			int id = dados->clientes_prioritarios[paciente_sorteado.first];
			double dist_volta_posto = dados->matriz_tempo[id][0];
			int retorno_adicionar = dia->adicionar_visita(dados->clientes_prioritarios[paciente_sorteado.first], i, paciente_sorteado.second, dist_volta_posto);

			//se o retorno 1, 2 ou 3 significa que nao deu certo a inserção pq violaria o horario e o agente teve que retornar ao posto
			if(retorno_adicionar > 0){
				dia->troca_turno(i, dist_volta_posto);
				continue;
			}

			//adicionando que o paciente foi servido
			cont_priori_servidos++;
			pacientes_inseridos[dados->clientes_prioritarios[paciente_sorteado.first]] = true;
			
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
		for(int i = 0; i < dados->numero_agentes; i++){

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
			vector<pair<int,double>> indice_e_distancia;
			
			for (int j = 0; j < dados->clientes_nao_prioritarios.size(); j++) {
				
				int id = dados->clientes_nao_prioritarios[j];

				if (!pacientes_inseridos[id]) {
					double distancia = dados->matriz_tempo[ult_loc][id]; 
					indice_e_distancia.push_back(pair<int,double>(j, distancia));
				}
			}
			
			//ordenando crescente
			std::sort(indice_e_distancia.begin(), indice_e_distancia.end(), [](const pair<int,double>& a, const pair<int,double>& b) {
        		return a.second < b.second;
    		});

			std::uniform_real_distribution<double> dist_alfa(0.0, 1.0);
			double alfa = dist_alfa(gen);

			// 3. Selecionar aleatoriamente um dos (alfa * |Ômega|) primeiros pares
			size_t limite = std::max<size_t>(1, std::ceil(alfa * indice_e_distancia.size()));
			
			// Distribuição para escolher um índice inteiro aleatório entre 0 e (limite - 1)
			std::uniform_int_distribution<size_t> dist_indice(0, limite - 1);
			size_t indice_sorteado = dist_indice(gen);

			pair<int,double> paciente_sorteado = indice_e_distancia[indice_sorteado];
			
			int id = dados->clientes_nao_prioritarios[paciente_sorteado.first];
			double dist_volta_posto = dados->matriz_tempo[id][0];
			int retorno_adicionar = dia->adicionar_visita(dados->clientes_nao_prioritarios[paciente_sorteado.first], i, paciente_sorteado.second, dist_volta_posto);

			if(retorno_adicionar > 0){
				dia->troca_turno(i, dist_volta_posto);
				continue;
			}

				//adicionando que o paciente foi servido
			cont_npriori_servidos++;
			pacientes_inseridos[dados->clientes_nao_prioritarios[paciente_sorteado.first]] = true;
			
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
				for(int j = 0; j < dados->numero_agentes; j++){
					
				 	int ult_loc = dia->ultimo_no_rota(j);
				 	if(ult_loc != 0){	
						dia->finaliza_turno(j, dados->matriz_tempo[ult_loc][0]);
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

//tem que marcar o tempo de execucao total e o tempo gasto na busca local
void Heuristica::ILS_RVND(std::string nome_saida){
	
	s = solucao_inicial();
	s_local = s;
	s_global = s;
	
	for (int i=1; i<=MAX_ITER; i++) {
		
		//cout << "entrou na cons" << endl;
		s = solucao_inicial();
		//cout << "saiu da cons" << endl;
		s_local = s;

		int iterILS = 0;
        int MaxIterILS = dados->quantidade_clientes;
        while (iterILS <= MaxIterILS) {
			//cout << "iterILS: " << iterILS << "/" << MaxIterILS << endl; 
            
			RVND(s);
									
            //Critério de aceitação do ILS
            if (s.get_fo() < s_local.get_fo()) {
                s_local = s;
                iterILS = 0;
            }
			
			//cout << "entrou na pert" << endl;
            perturbacao(s_local);
            //cout << "saiu da pert" << endl;
			iterILS++;
        }
        
        //Atualização da melhor solução global
        if (s_local.get_fo() < s_global.get_fo()) {
            s_global = s_local;
        }
	}
	cout << "encerrou tudo" << endl;
	s_global.salva_solucao_arquivo(nome_saida, dados->clientes_prioritarios, dados->clientes_nao_prioritarios);
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
				melhorou_intra = mov.intra_oropt2(s);
				break;
			case 2:
				melhorou_intra = mov.intra_reinsertion(s);
				break;
			case 3: 
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
				melhorou =  movimentos.inter_shift10(s);  
				break;
			case 2:
				melhorou =  movimentos.inter_swap11(s);
				break;
			case 3:
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
 After some preliminary experiments, the number of successive moves was
empirically set to be chosen from the interval {0.5v, 0.6v, . . . , 1.4v, 1.5v}.

vai ser um shift 1,0
*/
void Heuristica::perturbacao(Solucao &s){

	//const std::string AZUL = "\033[34m";
    //const std::string RESET = "\033[0m";
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> distrib(1, dados->numero_agentes);

    int qtd_movimentos = distrib(gen);

	std::vector<Rotas_diarias>& rotas = s.get_dias_planejamento();
	int ind_ultdia = rotas.size()-1;

	//cout << AZUL << "SERAO REALIZADOS " << qtd_movimentos << " DE SWAPS" << RESET << endl;
	
	for(int i = 0; i < qtd_movimentos; i++){

		std::uniform_int_distribution<> distrib(0, ind_ultdia);
		int dia1_sorteado = distrib(gen);
		//cout << "opa0" << endl;

		distrib.param(decltype(distrib)::param_type(0, rotas[dia1_sorteado].rotas.size()-1));
		int rota1_sorteada = distrib(gen);
		//cout << "opa01" << endl;

		while(rotas[dia1_sorteado].rotas[rota1_sorteada].sequencia.size() <= 3){
			distrib.param(decltype(distrib)::param_type(0, ind_ultdia));
			dia1_sorteado = distrib(gen);
			distrib.param(decltype(distrib)::param_type(0, rotas[dia1_sorteado].rotas.size()-1));
			rota1_sorteada = distrib(gen);
		}

		distrib.param(decltype(distrib)::param_type(1, rotas[dia1_sorteado].rotas[rota1_sorteada].sequencia.size()-2));
		int i_sorteado = distrib(gen);
		//cout << "opa02" << endl;

		while(rotas[dia1_sorteado].rotas[rota1_sorteada].sequencia[i_sorteado] == 0){
			i_sorteado = distrib(gen);
		}
		//cout << "opa03" << endl;

		//isso é um atalho para o paciente que será removido pq o paciente é apenas um número
		int id_sorteado = rotas[dia1_sorteado].rotas[rota1_sorteada].sequencia[i_sorteado];
		
		distrib.param(decltype(distrib)::param_type(0, ind_ultdia));
		int dia2_sorteado = distrib(gen);
		

		distrib.param(decltype(distrib)::param_type(0, rotas[dia2_sorteado].rotas.size()-1));
		int rota2_sorteada = distrib(gen);
		

		while(rotas[dia2_sorteado].rotas[rota2_sorteada].id_rota == rotas[dia1_sorteado].rotas[rota1_sorteada].id_rota or rotas[dia2_sorteado].rotas[rota2_sorteada].sequencia.size() <= 3){
			distrib.param(decltype(distrib)::param_type(0, ind_ultdia));
			dia2_sorteado = distrib(gen);
			distrib.param(decltype(distrib)::param_type(0, rotas[dia1_sorteado].rotas.size()-1));
			rota2_sorteada = distrib(gen);
		}
		
		distrib.param(decltype(distrib)::param_type(1, rotas[dia2_sorteado].rotas[rota2_sorteada].sequencia.size()-1));
		int j_sorteado = distrib(gen);
		
		
		/*
		cout << AZUL << "ANTES: " << RESET << endl; 
		printa_solucao(rotas[dia1_sorteado].rotas[rota1_sorteada], i_sorteado, -1);
		printa_solucao(rotas[dia2_sorteado].rotas[rota2_sorteada], -1, j_sorteado);
		*/
		const auto& rota1 = rotas[dia1_sorteado].rotas[rota1_sorteada].sequencia;
		const auto& rota2 = rotas[dia2_sorteado].rotas[rota2_sorteada].sequencia;

		//necessario 1 é o que [e necessario]
		double necessario1 = -dados->duracao_visita_h   
                        	+dados->matriz_tempo[rota1[i_sorteado-1]][rota1[i_sorteado+1]] 
                        	-dados->matriz_tempo[rota1[i_sorteado-1]][rota1[i_sorteado]] 
                        	-dados->matriz_tempo[rota1[i_sorteado]][rota1[i_sorteado+1]];

		 //teoricamente tirar um paciente não seria problema mas checo:
		 while(1){
        	if(i_sorteado > rotas[dia1_sorteado].rotas[rota1_sorteada].qtd_pacientes_am){

				if(rotas[dia1_sorteado].get_tempo_disponivel_pm(rota1_sorteada) > necessario1){    
					break;
            	}else{
					distrib.param(decltype(distrib)::param_type(1, rotas[dia1_sorteado].rotas[rota1_sorteada].sequencia.size()-2));
					while(rotas[dia1_sorteado].rotas[rota1_sorteada].sequencia[i_sorteado] == 0){
						i_sorteado = distrib(gen);
					}	
				}
			}else{ 
                //mov invalido
                if(rotas[dia1_sorteado].get_tempo_disponivel_am(rota1_sorteada) > necessario1){
                        continue;
                    }
            }
		}
		double necessario2 = dados->duracao_visita_h   
                            -dados->matriz_tempo[rota2[j_sorteado-1]][rota2[j_sorteado]] 
                            +dados->matriz_tempo[rota2[j_sorteado-1]][rota1[i_sorteado]]
                            +dados->matriz_tempo[rota1[i_sorteado]][rota2[j_sorteado]];
		bool i_adicionado = 0;					 
			
		//posicao da tarde
		if(j_sorteado > rotas[dia2_sorteado].rotas[rota2_sorteada].qtd_pacientes_am){


			if(rotas[dia2_sorteado].get_tempo_disponivel_pm(rota2_sorteada) < necessario1){    
				
				//disponivel na tarde é insuficiente, entao tem que adicionar mais um dia
				Rotas_diarias r = Rotas_diarias(dados->numero_agentes, dados->duracao_visita_h);
				int primeira_agente = 0;
				double posto_casa1 = dados->matriz_tempo[0][id_sorteado];
				double casas1_posto = dados->matriz_tempo[id_sorteado][0];

				r.adicionar_visita(id_sorteado, primeira_agente, posto_casa1, casas1_posto);
				r.finaliza_turno(primeira_agente, dados->matriz_tempo[id_sorteado][0]);
				s.adiciona_dia(r);
				rotas[dia2_sorteado].rotas[rota2_sorteada].tempo_am += posto_casa1 + casas1_posto;
				rotas[dia2_sorteado].rotas[rota2_sorteada].tempo_total += posto_casa1 + casas1_posto;
				i_adicionado = 1; 
			}else{
				//deu certo
				continue;
			}
		
		//a posição é para manhã
		}else{
			
			if(rotas[dia2_sorteado].get_tempo_disponivel_am(rota2_sorteada) < necessario1){    
				 
				if(rotas[dia2_sorteado].rotas[rota2_sorteada].qtd_pacientes_pm == 0){
					
					//a tarde está com 0 pacientes	
					double posto_casa1 = dados->matriz_tempo[0][id_sorteado];
					double casas1_posto = dados->matriz_tempo[id_sorteado][0];
					rotas[dia2_sorteado].avancar_turno(rota2_sorteada);
					rotas[dia2_sorteado].adicionar_visita(id_sorteado, rota2_sorteada, posto_casa1, casas1_posto);
					rotas[dia2_sorteado].rotas[rota2_sorteada].tempo_pm += posto_casa1 + casas1_posto;
					i_adicionado = 1;
				}else{
					
					if(rotas[dia2_sorteado].get_tempo_disponivel_pm(rota2_sorteada) < necessario1){
						
						//a tarde não tem tempo suficiente
						Rotas_diarias r = Rotas_diarias(dados->numero_agentes, dados->duracao_visita_h);
						int primeira_agente = 0;
						r.adicionar_visita(id_sorteado, primeira_agente, dados->matriz_tempo[0][id_sorteado], dados->matriz_tempo[id_sorteado][0]);
						r.finaliza_turno(primeira_agente, dados->matriz_tempo[id_sorteado][0]);
						s.adiciona_dia(r);
						i_adicionado = 1; 
					}else{
						//tem q usar o da vizinhanca
					}
				}
				
			}	
		}
		
		if(i_adicionado == 0){

			if(rotas[dia1_sorteado].rotas[rota1_sorteada].qtd_pacientes_pm == 1){
            	rotas[dia1_sorteado].retroceder_turno(rota1_sorteada);
            	//retirando o 0 que fica a mais na solução já q o unico paciente do turno foi retirado
            	rotas[dia1_sorteado].rotas[rota1_sorteada].sequencia.erase(rotas[dia1_sorteado].rotas[rota1_sorteada].sequencia.begin() + i_sorteado+1);
			}

			if(j_sorteado > rotas[dia2_sorteado].rotas[rota2_sorteada].qtd_pacientes_am){

				rotas[dia2_sorteado].rotas[rota2_sorteada].tempo_pm += necessario2;
				rotas[dia2_sorteado].rotas[rota2_sorteada].qtd_pacientes_pm++;
			
			}else{ 
				rotas[dia2_sorteado].rotas[rota2_sorteada].tempo_am += necessario2;
				rotas[dia2_sorteado].rotas[rota2_sorteada].qtd_pacientes_am++;
			}
			rotas[dia2_sorteado].rotas[rota2_sorteada].tempo_total += necessario2;

			//retirando o custo do paciente da posi i
			if(i_sorteado > rotas[dia1_sorteado].rotas[rota1_sorteada].qtd_pacientes_am){
				rotas[dia1_sorteado].rotas[rota1_sorteada].qtd_pacientes_pm--;
				rotas[dia1_sorteado].rotas[rota1_sorteada].tempo_pm += necessario1;
			}else{

				rotas[dia1_sorteado].rotas[rota1_sorteada].tempo_am += necessario1;
				rotas[dia1_sorteado].rotas[rota1_sorteada].qtd_pacientes_am--;
			}
			rotas[dia1_sorteado].rotas[rota1_sorteada].tempo_total += necessario1;
		}
		


		/*
		cout << AZUL << "DEPOIS: " << RESET << endl; 
		printa_solucao(rotas[dia1_sorteado].rotas[rota1_sorteada], i_sorteado, -1);
		printa_solucao(rotas[dia2_sorteado].rotas[rota2_sorteada], -1, j_sorteado);
		getchar();
		*/
	//fim do for de qtd de movimentos		
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
				double t = dados->matriz_tempo[dias[i].rotas[j].sequencia[k]][dias[i].rotas[j].sequencia[k+1]];
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
