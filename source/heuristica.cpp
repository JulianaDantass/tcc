#include "heuristica.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <vector>
#include <time.h>
#include <cstdlib> 
#include <math.h>
#include <iomanip>
#include <fstream>
#include <random>
#include <vector>

using namespace std;

#define EPSILON  0.001
#define QTD_VIZINHANCAS_INTER 3
#define QTD_VIZINHANCAS_INTRA 3
#define MAX_ITER 250


Heuristica::Heuristica(Instancia* dados_atuais){
	
	this->dados = dados_atuais;

}


//DEBUG		
void Heuristica::verifica_fo( Solucao &s){

	std::cout << "verifica fo" << endl;
	double t_total = 0; 
	int qtd_dias = s.get_quantidade_dias();
	auto dias = s.get_dias_planejamento();
	double t_am, t_pm, qtd_am, qtd_pm;
	
	
	for (int i = 0; i < qtd_dias; i++) {
			
		//std::cout << "dia " << i  << " - ";
        for (int j = 0; j < dias[i].rotas.size(); j++) {
			//std::cout << "a" << j << ": ";
			t_am = 0;
			t_pm = 0;
			qtd_am = 0;
			qtd_pm = 0;
			int tam1 = dias[i].rotas[j].sequencia_am.size(); 
			int tam2 = dias[i].rotas[j].sequencia_pm.size();

			if(tam1 > 0){
				for (int k = 0; k < dias[i].rotas[j].sequencia_am.size()-1; k++) {
					//cout << dias[i].rotas[j].sequencia_am[k] << " e " << dias[i].rotas[j].sequencia_am[k+1] << endl;
					double t = dados->matriz_tempo[dias[i].rotas[j].sequencia_am[k]][dias[i].rotas[j].sequencia_am[k+1]];
					t_total += t;
					t_am += t;
				}
				qtd_am = dias[i].rotas[j].sequencia_am.size()-2;
				t_am += 0.25* qtd_am;
			}

			if(tam2 > 0){
				for (int k = 0; k < dias[i].rotas[j].sequencia_pm.size()-1; k++) {
					cout << dias[i].rotas[j].sequencia_pm[k] << " e " << dias[i].rotas[j].sequencia_pm[k+1] << endl;
					double t = dados->matriz_tempo[dias[i].rotas[j].sequencia_pm[k]][dias[i].rotas[j].sequencia_pm[k+1]];
					t_total += t;
					t_pm += t;
				}
				qtd_pm = dias[i].rotas[j].sequencia_pm.size()-2;
				t_pm += 0.25* qtd_pm;
			}

			if(t_am - dias[i].rotas[j].tempo_am > double(EPSILON)){
				std::cout << "Tempos am não batem -> dia " << i << " rota " << j << endl;
				std::cout << "certo: " << t_am << " calculado: " << dias[i].rotas[j].tempo_am << endl;
				getchar();
			}

			if(t_pm - dias[i].rotas[j].tempo_pm > double(EPSILON)){
				std::cout << "Tempos pm não batem ";
				std::cout << "certo: " << t_pm << " calculado: " << dias[i].rotas[j].tempo_pm << endl;
				getchar();
			}
		}
    } 
	 
	t_total += 0.25*(dados->quantidade_clientes-1);	
		
	if (abs(t_total - s.get_fo()) > double(EPSILON)){
		std::cout << "Tempos não batem" << endl;
        std::cout << "certo: " << setprecision(3) << t_total << " fo: " << s.get_fo() << endl;
		getchar();        
    }
}

// o vizinho mais próximo é utilizado como guloso
Solucao Heuristica::solucao_inicial(){

	Solucao si(dados->qtd_dias_planejamento);

	// Cria um vetor com N elementos, preenchendo todos eles com uma cópia de Rotas_diarias(dados->numero_agentes)
	std::vector<Rotas_diarias> dias(dados->qtd_dias_planejamento, Rotas_diarias(dados->numero_agentes, dados->duracao_visita_h));
	
	//criando um vetor de booleano pra saber se o paciente já foi inserido
	std::cout << "qtd cliente: " << dados->quantidade_clientes << endl;
	int tam_vetor = (dados->quantidade_clientes+1);
	std::vector<bool> pacientes_inseridos(tam_vetor, false);

	//criando um vetor com o numero do paciente e a distancia entre o posto e o paciente
	int ult_loc = 0;   		
	//inicializando no primeiro dia 
	int cont_dias = 0;
	Rotas_diarias* dia = &(dias.at(cont_dias));
	
	std::random_device rd;
    std::mt19937 gen(rd());
	//unsigned int sem = 40;
	//std::mt19937 gen(sem);
	//alocar o resto dos pacientes prioritários	
	int cont_priori_servidos = 0;
	bool flag_priori_servidos = 0;
	while(1){
		//pra saber qtos prioritarios ja foram servidos
		bool flag_dia_encerrado = 0;

		//adicionando o paciente prioritario que oferece menor distancia ao agente considerando sua rota 
		for(int i = 0; i < dados->numero_agentes; i++){

			if(cont_priori_servidos == dados->clientes_prioritarios.size()){
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
			//std::cout << ult_loc << " e " << id << " = " << paciente_sorteado.second;
			int retorno_adicionar = dia->adicionar_visita(id, i, paciente_sorteado.second, dist_volta_posto);

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
	bool flag_npriori_servidos = 0;
	//ALOCAR OS PACIENTES NAO PRIORITÁRIOS
	while(1){
		//é um contador pra saber qtos agentes tem o dia encerrado
		int flag_dia_encerrado = 0;
		
		//adicionando o paciente n prioritario que oferece menor distancia ao agente considerando sua rota 
		for(int i = 0; i < dados->numero_agentes; i++){

			if(cont_npriori_servidos == dados->clientes_nao_prioritarios.size()){
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
			//std::cout << ult_loc << " e " << id << " = " << paciente_sorteado.second << endl;
			int retorno_adicionar = dia->adicionar_visita(id, i, paciente_sorteado.second, dist_volta_posto);

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
						//cout << ult_loc << " e 0 = " << dados->matriz_tempo[ult_loc][0] << endl;
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

				//std::cout << "inter shift: ";
				melhorou =  movimentos.inter_shift10(s);  
				//verifica_fo(s);
				//std::cout << melhorou << endl;
				break;
			case 2:
				//std::cout << "inter swap22 ";
				melhorou =  movimentos.inter_swap22(s);
				//verifica_fo(s);
				//std::cout << melhorou << endl;
				break;
		}
        
        //se melhorou a solucao 
        if (melhorou) {
			
			//std::cout<< "entrou na busca intra" << endl;
            busca_intra_rota(s, movimentos);
			//std::cout<< "saiu da busca intra" << endl;

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
	
	bool melhorou_intra;
   	vector<int> NLA;

	for(int i = 1; i <= QTD_VIZINHANCAS_INTRA; i++){
		NLA.push_back(i);
	}
    
    while (!NLA.empty()) {

        int n = rand() % NLA.size();

		switch (NLA[n]){
			 
			case 1:
				melhorou_intra = mov.intra_swap22(s);
				break;
			case 2:
				melhorou_intra = mov.intra_reinsertion(s);
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


//the number of successive moves was empirically set to be chosen from the interval {0.5v, 0.6v, . . . , 1.4v, 1.5v}. vai ser um shift 1,0
void Heuristica::perturbacao(Solucao &s){

	const std::string AZUL = "\033[34m";
    const std::string RESET = "\033[0m";
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> distrib(1, dados->numero_agentes);

    int qtd_movimentos = distrib(gen);

	std::vector<Rotas_diarias>& dias = s.get_dias_planejamento();
	int ind_ultdia = dias.size()-1;

	cout << "ind ult dia: " << ind_ultdia << endl;
	//std::std::cout << AZUL << "SERAO REALIZADOS " << qtd_movimentos << " DE shift" << RESET << endl;
	
	for(int i = 0; i < qtd_movimentos; i++){
		
		std::uniform_int_distribution<> distrib(0, ind_ultdia);
		int dia1_sorteado = distrib(gen);
		distrib.param(decltype(distrib)::param_type(0, dias[dia1_sorteado].rotas.size()-1));
		int rota1_sorteada = distrib(gen);
		distrib.param(decltype(distrib)::param_type(1, 2));
		int turno1 = distrib(gen);

		vector<int>* rota1 = (turno1 == 1) ? &dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_am : &dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_pm;

		while(rota1->size() < 3){
			//dias[dia1_sorteado].printa_rota(rota1_sorteada);
			distrib.param(decltype(distrib)::param_type(0, ind_ultdia));
			dia1_sorteado = distrib(gen);
			distrib.param(decltype(distrib)::param_type(0, dias[dia1_sorteado].rotas.size()-1));
			rota1_sorteada = distrib(gen);
			distrib.param(decltype(distrib)::param_type(1, 2));
			turno1 = distrib(gen);
		}
		
		rota1 = (turno1 == 1) ? &dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_am : &dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_pm;
		//sorteia a rota dnv pq a rota tem que conter pelo menos 3 nós
		distrib.param(decltype(distrib)::param_type(1, rota1->size()-1));
		int i_sorteado = distrib(gen);

		double necessario1;
		bool deu_errado = 0;
		int j=0;
		//dias[dia1_sorteado].printa_rota(rota1_sorteada);
		while(1){
			cout << "ok" << endl;
		

			necessario1 = 	-dados->duracao_visita_h   
							+ dados->matriz_tempo[(*rota1)[i_sorteado - 1]][(*rota1)[i_sorteado + 1]] 
							- dados->matriz_tempo[(*rota1)[i_sorteado - 1]][(*rota1)[i_sorteado]] 
							- dados->matriz_tempo[(*rota1)[i_sorteado]][(*rota1)[i_sorteado + 1]];

        	if(turno1 == 2){

				if(dias[dia1_sorteado].get_tempo_disponivel_pm(rota1_sorteada) > necessario1){    
					deu_errado = 0;
					break;
            	}else{
					deu_errado = 1;
				}
			}else{

                if(dias[dia1_sorteado].get_tempo_disponivel_am(rota1_sorteada) > necessario1){
					deu_errado = 0;
					break;
                }else{
					deu_errado = 1; 
				}
            }

			if(deu_errado){
				std::cout << "(while) distribuicao rota 1 entre 0 e " << dias[dia1_sorteado].rotas.size()-1 << endl;

				distrib.param(decltype(distrib)::param_type(0, ind_ultdia));
				dia1_sorteado = distrib(gen);
				distrib.param(decltype(distrib)::param_type(0, dias[dia1_sorteado].rotas.size()-1));
				rota1_sorteada = distrib(gen);
				distrib.param(decltype(distrib)::param_type(1, 2));
				turno1 = distrib(gen);
				rota1 = (turno1 == 1) ? &dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_am : &dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_pm;

				while(rota1->size() < 3){
					distrib.param(decltype(distrib)::param_type(0, ind_ultdia));
					dia1_sorteado = distrib(gen);
					distrib.param(decltype(distrib)::param_type(0, dias[dia1_sorteado].rotas.size()-1));
					rota1_sorteada = distrib(gen);
					distrib.param(decltype(distrib)::param_type(1, 2));
					turno1 = distrib(gen);
				}
				
				rota1 = (turno1 == 1) ? &dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_am : &dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_pm;
				distrib.param(decltype(distrib)::param_type(1,rota1->size()-1));
				i_sorteado = distrib(gen);
			}
			j++;
		}		
		
		//o dia 2 será necessariamente o ultimo dia
		int dia2 = ind_ultdia;
		distrib.param(decltype(distrib)::param_type(0, dias[dia2].rotas.size()-1));
		int rota2_sorteada = distrib(gen);
		distrib.param(decltype(distrib)::param_type(1, 2));
		int turno2 = distrib(gen);

		vector<int>* rota2 = (turno2 == 1) ? &dias[dia2].rotas[rota2_sorteada].sequencia_am : &dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_pm;
		
		distrib.param(decltype(distrib)::param_type(1, rota2->size()-1));
		int j_sorteado = distrib(gen);
		//std::cout << "j sorteado: " << j_sorteado << endl;
	
		
		double necessario2;
		while(1){

			necessario2 = 	-dados->duracao_visita_h   
							-dados->matriz_tempo[(*rota2)[j - 1]][(*rota2)[j]] 
							+dados->matriz_tempo[(*rota2)[j - 1]][(*rota1)[i]]
							+dados->matriz_tempo[(*rota1)[i]][(*rota2)[j]];
        	if(turno2 == 2){

				if(dias[dia2].get_tempo_disponivel_pm(rota2_sorteada) > necessario2){    
					deu_errado = 0;
					break;
            	}else{
					deu_errado = 1;
				}
			}else{

                if(dias[dia2].get_tempo_disponivel_am(rota2_sorteada) > necessario2){
					deu_errado = 0;
					break;
                }else{
					deu_errado = 1; 
				}
            }

			if(deu_errado){
			
				distrib.param(decltype(distrib)::param_type(0, dias[dia2].rotas.size()-1));
				rota2_sorteada = distrib(gen);
				distrib.param(decltype(distrib)::param_type(1, 2));
				turno2 = distrib(gen);
				rota2 = (turno2 == 1) ? &dias[dia2].rotas[rota2_sorteada].sequencia_am : &dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_pm;
				distrib.param(decltype(distrib)::param_type(1, rota2->size()-1));
				j_sorteado = distrib(gen);
				
			}
		}

		int movido = dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_pm[i_sorteado];
		 //so 1 paciente no turno da tarde
        if(turno1 == 2 and rota1->size() == 3){
            dias[dia1_sorteado].retroceder_turno(rota1_sorteada);
        }

        // 3. Atualiza contadores e tempos na ROTA DE DESTINO
        if(turno2 == 2 ){
            cout <<"j no tempo pm\n";
            dias[dia2].rotas[rota2_sorteada].tempo_pm += necessario2;
        } else { 
            cout <<"j no tempo am\n";
            dias[dia2].rotas[rota2_sorteada].tempo_am += necessario2;
        }

        // 4. Atualiza contadores e tempos na ROTA DE ORIGEM
        if(turno1 == 2){
            cout <<"i no tempo pm\n";
            dias[dia1_sorteado].rotas[rota1_sorteada].tempo_pm += necessario1;
        } else {
            cout <<"i no tempo am\n";
            dias[dia1_sorteado].rotas[rota1_sorteada].tempo_am += necessario1;
        }
        
        // 5. Atualiza o custo da solução global
        s.set_fo(s.get_fo() + necessario1 + necessario2); // melhor_delta == melhor_delta1 + melhor_delta2

        // 6. Efetiva as alterações nos vetores (Inter-rotas)
        // Remove da origem
		if(turno1 == 2){
			dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_pm.erase(dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_pm.begin() + i_sorteado);
		}else{
			dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_am.erase(dias[dia1_sorteado].rotas[rota1_sorteada].sequencia_am.begin() + i_sorteado);
		}

		if(turno2 == 2){
			dias[dia2].rotas[rota2_sorteada].sequencia_pm.insert(dias[dia2].rotas[rota2_sorteada].sequencia_pm.begin() + j_sorteado, movido);
		}else{
			dias[dia2].rotas[rota2_sorteada].sequencia_am.insert(dias[dia2].rotas[rota2_sorteada].sequencia_am.begin() + j_sorteado, movido);
		}
       	
		//verifica_fo(s);
	//fim do for
	} 
}

//tem que marcar o tempo de execucao total e o tempo gasto na busca local
void Heuristica::ILS_RVND(std::string nome_saida){
	
	for (int i=1; i<=MAX_ITER; i++) {
		
		s = solucao_inicial();
		verifica_fo(s); std::cout << "---------------------------------------------------------";

		if(i == 1){
			s_local = s;
			s_global = s;
		}
		//std::cout << "entrou na cons" << endl;
	
		
		//std::cout << "saiu da cons" << endl;
		s_local = s;

		int iterILS = 0;
        int MaxIterILS = dados->quantidade_clientes;
        while (iterILS <= MaxIterILS) {
			
			
			RVND(s);
			verifica_fo(s);
            
			//Critério de aceitação do ILS
            if (s.get_fo() < s_local.get_fo()) {
                s_local = s;
                iterILS = 0;
            }
			//algum problema na pertubacao
			//std::std::cout << "entrou na pert" << endl;
            //perturbacao(s_local);
            //std::std::cout << "saiu da pert" << endl;

			iterILS++;
        }
        
        //Atualização da melhor solução global
        if (s_local.get_fo() < s_global.get_fo()) {
            s_global = s_local;
        }
		std::cout << i << endl;
	}
	std::cout << "encerrou tudo" << endl;
	s_global.salva_solucao_arquivo(nome_saida, dados->clientes_prioritarios, dados->clientes_nao_prioritarios);
}




Heuristica::~Heuristica(){
	
}
