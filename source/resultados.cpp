#include "resultados.h"
#include <iostream>
#include <fstream>
#include <sstream>

Resultados::Resultados() {
}

bool Resultados::salvar_solucao_manual_csv(const std::string& nome_arquivo) {

    std::ofstream arquivo(nome_arquivo);
    
    if (!arquivo.is_open()) {
        std::cerr << "Erro: Não foi possível criar o arquivo " << nome_arquivo << ".\n";
        return false;
    }
    
    arquivo << "Id_Microarea,Nome_Rua,Numero_Casa\n";

    for (const Microarea& microarea : microareas) {    
        for (const Rua& rua : microarea.lista_ruas) {            
            for (int numero_casa : rua.lista_casas) {
                
                arquivo << microarea.id << "," 
                        << rua.nome << "," 
                        << numero_casa << "\n";
            }
        }
    }
    
    arquivo.close();
    return true;
}

bool Resultados::carregar_csv(const std::string& nome_arquivo) {

    std::ifstream arquivo(nome_arquivo);
    
    if (!arquivo.is_open()) {
        std::cerr << "Erro: Não abriu o arquivo " << nome_arquivo << ".\n";
        return false;
    }

    // Limpa a memória atual para evitar misturar com os dados antigos
    microareas.clear(); 

    std::string linha;    
    // Pula a primeira linha, pois é apenas o cabeçalho
    std::getline(arquivo, linha); 
    
    // Lê o arquivo linha por linha até o final
    while (std::getline(arquivo, linha)) {
        
        // Pula linhas que estiverem vazias acidentalmente
        if (linha.empty()) continue;

        std::stringstream stream_linha(linha);
        std::string texto_id, nome_rua, texto_numero_casa;
        
        try {
            // Passo 1: Extrair os blocos de texto separados pela vírgula
            std::getline(stream_linha, texto_id, ',');
            std::getline(stream_linha, nome_rua, ',');
            std::getline(stream_linha, texto_numero_casa, '\n'); 
            
            // Passo 2: Converter os textos para números inteiros
            int id_microarea = std::stoi(texto_id);
            int numero_casa = std::stoi(texto_numero_casa);
            
            // Passo 3: Procurar se a Microárea já existe na nossa lista
            Microarea* microarea_alvo = nullptr;
            for (Microarea& ma : microareas) {
                if (ma.id == id_microarea) {
                    microarea_alvo = &ma; // Encontrou!
                    break;
                }
            }
            
            // Se não encontrou, cria uma nova Microárea
            if (microarea_alvo == nullptr) {
                Microarea nova_microarea;
                nova_microarea.id = id_microarea;
                microareas.push_back(nova_microarea);
                
                // Aponta para essa nova microárea recém-adicionada
                microarea_alvo = &microareas.back();
            }
            
            // Passo 4: Procurar se a Rua já existe dentro dessa Microárea
            Rua* rua_alvo = nullptr;
            for (Rua& rua : microarea_alvo->lista_ruas) {
                if (rua.nome == nome_rua){
                    rua_alvo = &rua; // Encontrou!
                    break;
                }
            }
            
            // Se não encontrou, cria uma nova Rua
            if (rua_alvo == nullptr) {
                Rua nova_rua;
                nova_rua.nome = nome_rua;
                microarea_alvo->lista_ruas.push_back(nova_rua);
                
                // Aponta para essa nova rua recém-adicionada
                rua_alvo = &microarea_alvo->lista_ruas.back();
            }
            
            // Passo 5: Finalmente, adiciona o número da casa na rua correta
            rua_alvo->lista_casas.push_back(numero_casa);

        } catch (...) {
            // Se der erro de conversão (ex: letras onde devia ter número), avisa e continua
            std::cerr << "Aviso: Linha ignorada por erro de formato -> " << linha << "\n";
        }
    }
    
    arquivo.close();
    return true;
}

