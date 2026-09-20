#pragma once
#include <vector>
#include <string>

struct Rua {
    std::string nome;
    std::vector<int> lista_casas;
};

struct Microarea {
    int id;
    std::vector<Rua> lista_ruas;
};

class Resultados {
private:
    std::vector<Microarea> microareas;

public:
    // Construtores
    Resultados();

    // Métodos
    bool salvar_solucao_manual_csv(const std::string& nome_arquivo);
    bool carregar_csv(const std::string& nome_arquivo);
};