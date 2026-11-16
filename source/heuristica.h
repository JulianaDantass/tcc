#pragma once

#include <vector>
#include <random>
#include "instancia.h"
#include "solucao.h"
#include "vizinhancas.h"

#define QTD_DIAS_PLANEJAMENTO 30


class Heuristica{
	public:
		Heuristica(Instancia* d);
		~Heuristica();
		
		void printa(Rota& r);
		
		void ILS_RVND(std::string nome_saida);
		
		//DEBUG
		bool validar_rotas(const std::vector<Rotas_diarias>& antigas, const std::vector<Rotas_diarias>& novas, int dia1, int dia2, int idxRota1, int idxRota2);
		void verifica_fo(Solucao& s);
		void printa_solucao(Rota& r, int melhor_paciente, int melhor_posicao);

	private:
		Solucao solucao_inicial();
		bool atender_um (Rotas_diarias& dia, int ag, int m, bool prioritario); // essa é diferente da do manual
		void busca_intra_rota(Solucao &s, Vizinhancas &mov);
		void perturbacao(Solucao &s);
		void RVND(Solucao &s);
		void reorganiza_ult_dia(Solucao &s);
		std::random_device rd;
   	 	std::mt19937 gen{rd()};
		Instancia *dados;
		
		
};

