#pragma once

#include <vector>
#include "instancia.h"
#include "solucao.h"
#include "vizinhancas.h"

#define QTD_DIAS_PLANEJAMENTO 30


class Heuristica{
	public:
		Heuristica(Instancia* dados);
		~Heuristica();
		Solucao solucao_inicial();
		void busca_intra_rota(Solucao s);
		Solucao perturbacao(Solucao s);
		Solucao RVND(Solucao &s);
		Solucao ILS_RVND();
		
		//DEBUG
		void verifica_fo(Solucao& s);

	private:
		Instancia *dados;
		Solucao s{QTD_DIAS_PLANEJAMENTO};
		Solucao s_local{QTD_DIAS_PLANEJAMENTO};
		Solucao s_global{QTD_DIAS_PLANEJAMENTO};
		Vizinhancas movimentos; 
		
};

