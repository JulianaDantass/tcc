#pragma once

#include <vector>
#include "instancia.h"
#include "solucao.h"
#include "vizinhancas.h"

#define QTD_DIAS_PLANEJAMENTO 30

struct InsercaoInfo
{
	Paciente no_candidato; // no k a ser inserido
	int aresta_removida; // aresta {i,j} na qual o no k sera inserido
	double custo; // delta ao inserir k na aresta {i,j}
};

class Heuristica{
	public:
		Heuristica(Instancia* dados);
		~Heuristica();
		vector<InsercaoInfo> calcular_custo_insercao(Rotas_diarias* d, vector<bool> &pacientes_inseridos);
		Solucao solucao_inicial();
		void busca_intra_rota(Solucao &s);
		Solucao perturbacao(Solucao &s);
		Solucao RVND(Solucao &s);
		Solucao ILS_RVND();
		
		//DEBUG
		void verifica_fo(Solucao& s);

	private:
		vector<vector<InsercaoInfo>> custos_insercao;
		Instancia *dados;
		Solucao s{QTD_DIAS_PLANEJAMENTO};
		Solucao s_local{QTD_DIAS_PLANEJAMENTO};
		Solucao s_global{QTD_DIAS_PLANEJAMENTO};
		Vizinhancas movimentos; 
		
};

