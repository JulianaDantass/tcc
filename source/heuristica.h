#pragma once

#include <vector>
#include "instancia.h"
#include "solucao.h"
#include "vizinhancas.h"

#define QTD_DIAS_PLANEJAMENTO 30

struct InsercaoInfo
{
	int no_candidato = -1; // no k a ser inserido
	int aresta_removida = -1; // aresta {i,j} na qual o no k sera inserido
	double custo =  1e9; // delta ao inserir k na aresta {i,j}

	InsercaoInfo(int id, int a, double c) : no_candidato(id), aresta_removida(a), custo(c) {}
};

class Heuristica{
	public:
		Heuristica(Instancia* d);
		~Heuristica();
		vector<InsercaoInfo> calcular_custo_insercao(Rotas_diarias* d, vector<bool> &pacientes_inseridos);
		Solucao solucao_inicial();
		void busca_intra_rota(Solucao &s, Vizinhancas &mov);
		void perturbacao(Solucao &s);
		void RVND(Solucao &s);
		void ILS_RVND();
		
		//DEBUG
		bool validar_rotas(const std::vector<Rotas_diarias>& antigas, const std::vector<Rotas_diarias>& novas, int dia1, int dia2, int idxRota1, int idxRota2);
		void verifica_fo(Solucao& s);
		void printa_solucao(Rota& r, int melhor_paciente, int melhor_posicao);

	private:
		vector<vector<InsercaoInfo>> custos_insercao;
		Instancia *dados;
		Solucao s{QTD_DIAS_PLANEJAMENTO};
		Solucao s_local{QTD_DIAS_PLANEJAMENTO};
		Solucao s_global{QTD_DIAS_PLANEJAMENTO};
		
		
};

