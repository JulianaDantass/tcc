#include "heuristica.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <vector>
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

	//alocar o resto dos pacientes prioritários	
	bool flag_priori_servidos = 0;
	int microarea_atual = 0;
	int agente_atual;
	while(1){

		//pra saber qtos prioritarios ja foram servidos
		bool flag_dia_encerrado = 0;
		agente_atual = 0;
		//adicionando o paciente prioritario que oferece menor distancia ao agente considerando sua rota 
		while(1){
											
			//std::cout << "cont priori: " << dados->microareas[microarea_atual].contador_prioritarios;
			if(dados->todos_servidos_microarea(microarea_atual,1)){
				microarea_atual++;
			}

			if(microarea_atual == dados->microareas.size()){
				flag_priori_servidos = 1;
				break;
			}
			if(dia->dia_encerrado()){
				flag_dia_encerrado = 1;
				break;
			}

			if(dia->get_turno(agente_atual) == 3){
				agente_atual++;
			}

			if(agente_atual == dados->microareas.size()){
				agente_atual = 0;
			}
			//sabendo qual foi a ultima casa visitada pelo agente
			
			ult_loc = dia->ultimo_no_rota(agente_atual);
			
			//criando um paciente p qualquer como o melhor
			vector<pair<int,double>> indice_e_distancia;
			//cout << "m: "<< microarea_atual << " ";
			for (int j = 0; j < dados->microareas[microarea_atual].clientes_prioritarios.size(); j++) {
				
				int id = dados->microareas[microarea_atual].clientes_prioritarios[j];
				//std::cout << agente_atual << " id: " << id; 

				if (pacientes_inseridos[id] == false) {
					
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
			if(indice_e_distancia.size()==0){
				continue;
			}
			size_t limite = std::max<size_t>(1, std::ceil(alfa * indice_e_distancia.size()));
			
			// Distribuição para escolher um índice inteiro aleatório entre 0 e (limite - 1)
			
			std::uniform_int_distribution<size_t> dist_indice(0, limite - 1);
			size_t indice_sorteado = dist_indice(gen);

			pair<int,double> paciente_sorteado = indice_e_distancia[indice_sorteado];
			
			int id = dados->microareas[microarea_atual].clientes_prioritarios[paciente_sorteado.first];
			double dist_volta_posto = dados->matriz_tempo[id][0];
			//std::std::cout << ult_loc << " e " << id << " = " << paciente_sorteado.second;
			int retorno_adicionar = dia->adicionar_visita(id, agente_atual, paciente_sorteado.second, dist_volta_posto);
			//se o retorno 1, 2 ou 3 significa que nao deu certo a inserção pq violaria o horario e o agente teve que retornar ao posto
			if(retorno_adicionar > 0){
				
				ult_loc = dia->ultimo_no_rota(agente_atual);
				dist_volta_posto = dados->matriz_tempo[ult_loc][0];
				dia->troca_turno(agente_atual, dist_volta_posto);
				continue;
			}
			
			//adicionando que o paciente foi servido
			dados->microareas[microarea_atual].contador_prioritarios++;
			pacientes_inseridos[id] = true;
			
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

	bool flag_npriori_servidos = 0;
	microarea_atual = 0;
	//ALOCAR OS PACIENTES NAO PRIORITÁRIOS
	while(1){
		//é um contador pra saber qtos agentes tem o dia encerrado
		int flag_dia_encerrado = 0;
		agente_atual = 0;	
		//adicionando o paciente n prioritario que oferece menor distancia ao agente considerando sua rota 
		while(1){			
			if(dados->todos_servidos_microarea(microarea_atual,0)){
				microarea_atual++;
			}

			if(microarea_atual == dados->microareas.size()){
				flag_npriori_servidos = 1;
				break;
			}
			if(dia->dia_encerrado()){
				flag_dia_encerrado = 1;
				break;
			}

			if(dia->get_turno(agente_atual) == 3){
				agente_atual++;
			}

			if(agente_atual == dados->microareas.size()){
				agente_atual = 0;
			}

			ult_loc = dia->ultimo_no_rota(agente_atual);
			
			//criando um paciente p qualquer como o melhor
			vector<pair<int,double>> indice_e_distancia;
			
			for (int j = 0; j < dados->microareas[microarea_atual].clientes_nao_prioritarios.size(); j++) {
				
				
				int id = dados->microareas[microarea_atual].clientes_nao_prioritarios[j];

				if (!pacientes_inseridos[id]) {
					//std::cout << id << endl;
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
			if(indice_e_distancia.size()==0){
				continue;
			}
			size_t limite = std::max<size_t>(1, std::ceil(alfa * indice_e_distancia.size()));
			
			// Distribuição para escolher um índice inteiro aleatório entre 0 e (limite - 1)
			std::uniform_int_distribution<size_t> dist_indice(0, limite - 1);
			size_t indice_sorteado = dist_indice(gen);

			//std::cout << indice_sorteado <<  "/" << indice_e_distancia.size() << endl;
			pair<int,double> paciente_sorteado = indice_e_distancia[indice_sorteado];
			
			int id = dados->microareas[microarea_atual].clientes_nao_prioritarios[paciente_sorteado.first];
			double dist_volta_posto = dados->matriz_tempo[id][0];
			//std::std::cout << ult_loc << " e " << id << " = " << paciente_sorteado.second << endl;
			int retorno_adicionar = dia->adicionar_visita(id, agente_atual, paciente_sorteado.second, dist_volta_posto);

			if(retorno_adicionar > 0){
				ult_loc = dia->ultimo_no_rota(agente_atual);
				dist_volta_posto = dados->matriz_tempo[ult_loc][0];
				dia->troca_turno(agente_atual, dist_volta_posto);
				continue;
			}

				//adicionando que o paciente foi servido
			dados->microareas[microarea_atual].contador_nao_prioritarios++;
			pacientes_inseridos[dados->microareas[microarea_atual].clientes_nao_prioritarios[paciente_sorteado.first]] = true;
			
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
			si.adiciona_dia(dias[cont_dias]);
			break;
		}

	}
	//tem que ver a solucao final
	//reorganiza_ult_dia(si);

	std::vector<Rotas_diarias>& s = si.get_dias_planejamento();
	for(Rotas_diarias& i : s){
		for(int j = 0; j < i.rotas.size(); j++){
			int id = i.ultimo_no_rota(j);
			i.finaliza_turno(j, dados->matriz_tempo[id][0]);	
		}
	}
	si.calcula_fo();
	//string saida = "proposta_pos_construcao.txt";
	//si.salva_solucao_arquivo(saida, -1, -1, -1, dados->prioritarios);
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
	auto sorteia = [&](int lo, int hi){ return distrib(gen, decltype(distrib)::param_type(lo, hi)); };

	const int MAX_TENTATIVAS = 100;

	for(int tent = 0; tent < MAX_TENTATIVAS; tent++){

		// 1. Sorteia o primeiro cliente
		int d1 = sorteia(0, dias.size()-1);
		int a1 = sorteia(0, dados->numero_agentes-1);
		int t1 = sorteia(1, 2);

		std::vector<int>& seq1 = (t1 == 1) ? dias[d1].rotas[a1].sequencia_am
										: dias[d1].rotas[a1].sequencia_pm;
		if(seq1.size() < 3){
			continue;   // precisa ter ao menos 1 cliente interno
		}

		int i = sorteia(1, seq1.size()-2);
		int x = seq1[i];
		if(dados->prioritarios[x]){
			continue;   // prioritário não pode ser movido
		} 

		// 2. Sorteia o segundo cliente
		int d2 = sorteia(0, dias.size()-1);
		int a2 = sorteia(0, dados->numero_agentes-1);
		int t2 = sorteia(1, 2);

		// mesma rota e mesmo turno: não faz nada
		if(d1 == d2 and a1 == a2 and t1 == t2) {
			continue;
		}

		std::vector<int>& seq2 = (t2 == 1) ? dias[d2].rotas[a2].sequencia_am
										: dias[d2].rotas[a2].sequencia_pm;
		if(seq2.size() < 3) continue;

		int j = sorteia(1, seq2.size()-2);
		int y = seq2[j];
		if(dados->prioritarios[y]) continue;

		// 3. Variação de tempo em cada rota
		// rota1: x sai, y entra na posição i
		double delta1 = -dados->matriz_tempo[seq1[i-1]][x]
						-dados->matriz_tempo[x][seq1[i+1]]
						+dados->matriz_tempo[seq1[i-1]][y]
						+dados->matriz_tempo[y][seq1[i+1]];

		// rota2: y sai, x entra na posição j
		double delta2 = -dados->matriz_tempo[seq2[j-1]][y]
						-dados->matriz_tempo[y][seq2[j+1]]
						+dados->matriz_tempo[seq2[j-1]][x]
						+dados->matriz_tempo[x][seq2[j+1]];

		// 4. Verificação de viabilidade
		double disponivel1 = (t1 == 1) ? dias[d1].get_tempo_disponivel_am(a1)
									: dias[d1].get_tempo_disponivel_pm(a1);
		double disponivel2 = (t2 == 1) ? dias[d2].get_tempo_disponivel_am(a2)
									: dias[d2].get_tempo_disponivel_pm(a2);

		if(disponivel1 < delta1) continue;
		if(disponivel2 < delta2) continue;

		// 5. Aplica o swap
		seq1[i] = y;
		seq2[j] = x;

		if(t1 == 1){
			dias[d1].rotas[a1].tempo_am += delta1;
		}else{
			  dias[d1].rotas[a1].tempo_pm += delta1;
		}      

		if(t2 == 1){
			dias[d2].rotas[a2].tempo_am += delta2;
		} 
		else{
			dias[d2].rotas[a2].tempo_pm += delta2;
		}        

		s.set_fo(s.get_fo() + delta1 + delta2);
		return;
	}
// se nenhuma tentativa foi viável, a solução fica inalterada
       	
}

//tem que marcar o tempo de execucao total e o tempo gasto na busca local
void Heuristica::ILS_RVND(std::string nome_saida){
	
	Solucao melhor_todas;
	int iteracao_melhor_todas = -1;

	std::vector<double> tempos_rvnd; 
	auto inicio_ils = std::chrono::high_resolution_clock::now();
	double soma_rvnd = 0;
	int iterILS = 100;
	for(int i = 0; i < MAX_ITER; i++){
		Solucao s = solucao_inicial();
		 
    	RVND(s);                                // busca local inicial

   	 	Solucao melhor_s = s;
    	int iter = 0;

   		while(iter < iterILS){
        	Solucao s_linha = melhor_s;         // perturba a melhor solução
       		perturbacao(s_linha);
        	verifica_fo(s_linha);
        	RVND(s_linha);                      // busca local depois de perturbar

        	if(s_linha.get_fo() < melhor_s.get_fo() - dados->eps){
            	melhor_s = s_linha;
            	iter = 0;
        	}else{
            	iter++;
        	}
    	}
		

		if (melhor_s.get_fo() < (melhor_todas.get_fo()-dados->eps)) {
			//cout << "melhorou todas" << endl;
			iteracao_melhor_todas = i; 
			//cout << melhor_todas.get_fo() << endl;
            melhor_todas = melhor_s;
        }
	}
	auto fim_ils = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duracao_ils = fim_ils - inicio_ils;
	double rvnd_pertubacao = soma_rvnd / 10.0;
	std::cout << "encerrou tudo" << endl;
	melhor_todas.salva_solucao_arquivo(nome_saida, duracao_ils.count(), iteracao_melhor_todas,dados->prioritarios);
}




Heuristica::~Heuristica(){
	
}