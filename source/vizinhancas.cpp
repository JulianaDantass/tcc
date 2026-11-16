#include "vizinhancas.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>



Vizinhancas::Vizinhancas(const Instancia *d){
    this->dados = d;
}


bool Vizinhancas::intra_reinsertion(Solucao &s){

    bool houve_melhoria_rotas = 0;
    std::vector<Rotas_diarias>& dias = s.get_dias_planejamento();
    double delta = 0;
    double melhor_delta = 0;                 

    int melhor_i, melhor_j, turno;       
    int melhor_a, melhor_d;

	for(int d = 0; d < dias.size(); d++){   
        for(int a = 0; a < dias[d].rotas.size(); a++){
            for(int t = 1; t <= 2 ; t++){

                if(t == 2 and dias[d].rotas[a].sequencia_pm.size() == 0){
                    continue;
                }
                const vector<int>& rota1 = (t == 1) ? dias[d].rotas[a].sequencia_am : dias[d].rotas[a].sequencia_pm;
                double disponivel_am = dias[d].get_tempo_disponivel_am(a);
                double disponivel_pm = dias[d].get_tempo_disponivel_pm(a);

                //o indice i representa o paciente a ser deslocado
                //o indice j '' os espaços onde o paciente pode ser inserido, sendo j=1 o espaço entre o local de partida (0) e a 1ª casa

                for(int i = 1; i < rota1.size()-1; i++){

                    if(rota1[i] == 0){
                        continue;
                    }
                    
                    for(int j = 1; j < rota1.size(); j++){

                        //tem q rever caso de adjacencia
                        if(i == j  or j == i+1){
                            continue;
                        }
                        
                        delta = + dados->matriz_tempo[rota1[i-1]][rota1[i+1]] + dados->matriz_tempo[rota1[j-1]][rota1[i]] 
                                + dados->matriz_tempo[rota1[i]][rota1[j]]     - dados->matriz_tempo[rota1[i-1]][rota1[i]] 
                                - dados->matriz_tempo[rota1[i]][rota1[i+1]]   - dados->matriz_tempo[rota1[j-1]][rota1[j]];
                        
                        if(t == 2){
                            if(delta > disponivel_pm) continue;
                        }else{
                            if(delta > disponivel_am) continue;
                        }
                        // se delta for menor que o melhor delta atual significa que melhorou a solucao
                        if (delta < melhor_delta){ 

                            melhor_delta = delta;  
                            turno = t;
                            melhor_i = i;          
                            melhor_j = j;          
                            melhor_a = a;
                            melhor_d = d;
                        }
                    }
                }
		    }
        }
    }

    if (melhor_delta < -dados->eps){
        //posicao da tarde
        if(turno == 2 ){
            dias[melhor_d].rotas[melhor_a].tempo_pm += melhor_delta;
        //posicao da manha
        }else{
            
            dias[melhor_d].rotas[melhor_a].tempo_am += melhor_delta;
        }
        s.set_fo(s.get_fo() + melhor_delta);

        //alterando a rota
        vector<int>& seq = (turno == 1) ? dias[melhor_d].rotas[melhor_a].sequencia_am : dias[melhor_d].rotas[melhor_a].sequencia_pm;

        // Guarda o elemento que será movido
        int paciente = seq[melhor_i];

        // 1. Remove da posição antiga primeiro
        seq.erase(seq.begin() + melhor_i);

        // 2. Ajusta o destino se necessário e insere
        int destino = melhor_j;
        if (melhor_i < melhor_j) {
            // Como removemos um elemento antes de j, o índice j recua 1 posição
            destino--;
        }

        seq.insert(seq.begin() + destino, paciente);
  
        houve_melhoria_rotas = 1;
    }

    return houve_melhoria_rotas;
}

bool Vizinhancas::intra_swap22(Solucao &s){
    
    bool houve_melhoria_rotas = 0;
    std::vector<Rotas_diarias>& dias = s.get_dias_planejamento();
    double delta, melhor_delta = 0;                 
    int melhor_i, melhor_j, melhor_d, melhor_a, turno;

	for(int d = 0; d < dias.size(); d++){   
        for(int a = 0; a < dias[d].rotas.size(); a++){
            for(int t = 1; t <= 2; t++){
                
                if(t == 2 and dias[d].rotas[a].sequencia_pm.size() == 0){
                    continue;
                }
                const vector<int>& rota1 = (t == 1) ? dias[d].rotas[a].sequencia_am : dias[d].rotas[a].sequencia_pm;
                double disponivel_am = dias[d].get_tempo_disponivel_am(a);
                double disponivel_pm = dias[d].get_tempo_disponivel_pm(a);

                for(int i = 1; i + 3 < rota1.size()-1; i++){
                    
                    int no_a = rota1[i - 1];
                    int b = rota1[i + 2];
                    int no_i = rota1[i];
                    int no_ip1 = rota1[i + 1];

                    for(int j = i+3; j + 1 <  rota1.size()-1; j++){
                        
                        // Renomeado para evitar conflito com 'a' e 'd' dos loops externos
                       
                        int c    = rota1[j - 1];
                        int no_d = rota1[j + 2];
                        int no_j   = rota1[j];
                        int no_jp1 = rota1[j + 1];

                        // Custos novos (o que entra) - atualizado com os novos nomes
                        double custo_novo = dados->matriz_tempo[no_a][no_j] 
                                        + dados->matriz_tempo[no_jp1][b] 
                                        + dados->matriz_tempo[c][no_i] 
                                        + dados->matriz_tempo[no_ip1][no_d];

                        // Custos antigos (o que sai)
                        double custo_antigo = dados->matriz_tempo[no_a][no_i] 
                                            + dados->matriz_tempo[no_ip1][b] 
                                            + dados->matriz_tempo[c][no_j] 
                                            + dados->matriz_tempo[no_jp1][no_d];

                        delta = custo_novo - custo_antigo;
                    
                        // Se delta for menor que o melhor delta atual, atualiza a solução
                        if (delta < melhor_delta){ 
                            melhor_d   = d;     // Agora usa corretamente o índice do dia do loop externo
                            melhor_delta = delta;  
                            turno      = t;
                            melhor_i   = i;          
                            melhor_j   = j;          
                            melhor_a   = a;     // Agora usa corretamente o índice da rota do loop externo
                        }
                    }
                }
            }
        }   
    }

    if (melhor_delta < -dados->eps){
        //cout << "intra swap 22" << endl;
        //cout << "melhor i: " << melhor_i << " / melhor j: " << melhor_j << endl;
        //cout << "rota antes: " << endl;
        //dias[melhor_d].printa_rota(melhor_a);
        
        vector<int>& seq = (turno == 1) ? dias[melhor_d].rotas[melhor_a].sequencia_am : dias[melhor_d].rotas[melhor_a].sequencia_pm;
        if (turno == 2) {
            dias[melhor_d].rotas[melhor_a].tempo_pm += melhor_delta;
        } else {
            dias[melhor_d].rotas[melhor_a].tempo_am += melhor_delta;
        }

        s.set_fo(s.get_fo() + melhor_delta);
        // Executa a troca dos dois blocos usando std::swap diretamente
        std::swap(seq[melhor_i],     seq[melhor_j]);
        std::swap(seq[melhor_i + 1], seq[melhor_j + 1]);

        //debug
        //cout << "rota dps: " << endl;
        //dias[melhor_d].printa_rota(melhor_a);
        //getchar();
        houve_melhoria_rotas = 1;
        
    }

    return houve_melhoria_rotas;
}


//Shift(1,0)– A customer k is transferred from a route r1 to a route r2.
bool Vizinhancas::inter_shift10(Solucao &s){  

    //cout << "entrou inter shift 10" << endl; 
    bool houve_melhoria_rotas = 0;
    std::vector<Rotas_diarias>& dias = s.get_dias_planejamento();
    //vai guardar a diferenca de valor em relacao a solucao de um dos dias
    double delta, 
    melhor_delta = 0, melhor_delta1, melhor_delta2;                 

    //vao guardar os indices de melhor troca
    int melhor_i = -1, melhor_j=-1;       
    int melhor_a=-1, melhor_b=-1, melhor_d=-1, melhor_e=-1;
    int turno_i=-1, turno_j=-1;
    

	for(int d = 0; d < dias.size(); d++){   
        for(int a = 0; a < dias[d].rotas.size(); a++){
            for(int t1 = 1; t1 <= 2; t1++){

                if(t1 == 2 and dias[d].rotas[a].sequencia_pm.size() == 0){
                    continue;
                }
                
                const vector<int>& rota1 = (t1 == 1) ? dias[d].rotas[a].sequencia_am : dias[d].rotas[a].sequencia_pm;

                if(rota1.size() <= 1){
                    continue;
                }
                //i representa um paciente da seq1 do agente a do dia d; começa do 1 pois a posicao 0 é deposito
                for(int i = 1; i < rota1.size()-1; i++){

                    double necessario1 = -dados->duracao_visita_h   
                            +dados->matriz_tempo[rota1[i-1]][rota1[i+1]] 
                            -dados->matriz_tempo[rota1[i-1]][rota1[i]] 
                            -dados->matriz_tempo[rota1[i]][rota1[i+1]];



                    //teoricamente tirar um paciente não seria problema mas checo:
                    if(t1 == 2){
                        //mov invalido 
                        if(dias[d].get_tempo_disponivel_pm(a) < necessario1) continue;
                    }else{ 
                        //mov invalido
                        if(dias[d].get_tempo_disponivel_am(a) < necessario1) continue;
                    
                    }

                    for(int e = 0; e < dias.size(); e++){
                        
                        //se for um paciente prioritario e tá tentando colocar num dia que não é o primeiro, cancela
                        if(dados->prioritarios[rota1[i]] == true and e > 0){
                            continue;
                        }
                            
                        
                        for(int b = 0; b < dias[e].rotas.size(); b++){
                            for(int t2 = 1; t2 <= 2; t2++){

                                

                                if(t2 == 2 and dias[e].rotas[b].sequencia_pm.size() == 0){
                                    continue;
                                }

                                //se for a mesma rota, n vamos fzr nd 
                                if(dias[d].rotas[a].id_rota == dias[e].rotas[b].id_rota and t1 == t2){ 
                                    continue;
                                }

                                

                                const vector<int>& rota2 = (t2 == 1) ? dias[e].rotas[b].sequencia_am : dias[e].rotas[b].sequencia_pm;

                                
                                for(int j = 1; j < rota2.size(); j++){

                                    
                                    double necessario2 = dados->duracao_visita_h  
                                                            -dados->matriz_tempo[rota2[j-1]][rota2[j]] 
                                                            +dados->matriz_tempo[rota2[j-1]][rota1[i]]
                                                            +dados->matriz_tempo[rota1[i]][rota2[j]];

                                    

                                    //tentativa de inserir no turno pm na posicao j
                                    if(t2 == 2){
                                        //mov invalido 
                                        if(dias[e].get_tempo_disponivel_pm(b) < necessario2) continue;
                                    }else{ 
                                        //mov invalido
                                        if(dias[e].get_tempo_disponivel_am(b) < necessario2) continue;
                                    
                                    }
                                    
                                    delta = necessario1+necessario2;
                                
                                    // se delta for menor que o melhor delta atual significa que melhorou a solucao
                                    if (delta < melhor_delta){ 

                                        melhor_delta = delta;  
                                        melhor_delta1 = necessario1;
                                        melhor_delta2 = necessario2;
                                        turno_i = t1;
                                        turno_j = t2;
                                        melhor_i = i;          
                                        melhor_j = j;          
                                        melhor_a = a;
                                        melhor_b = b;
                                        melhor_d = d;
                                        melhor_e = e;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (melhor_delta < -dados->eps and melhor_a != -1){

        vector<int>& rota1 = (turno_i == 1) ? dias[melhor_d].rotas[melhor_a].sequencia_am : dias[melhor_d].rotas[melhor_a].sequencia_pm;
        vector<int>& rota2 = (turno_j == 1) ? dias[melhor_e].rotas[melhor_b].sequencia_am : dias[melhor_e].rotas[melhor_b].sequencia_pm;
        
  
        int paciente_movido = rota1[melhor_i];

        //so 1 paciente no turno da tarde
        if(turno_i == 2 and rota1.size() == 3){
            dias[melhor_d].retroceder_turno(melhor_a);
        }

        // 3. Atualiza contadores e tempos na ROTA DE DESTINO
        if(turno_j == 2 ){
            //cout <<"j no tempo pm\n";
            dias[melhor_e].rotas[melhor_b].tempo_pm += melhor_delta2;
        } else { 
            //cout <<"j no tempo am\n";
            dias[melhor_e].rotas[melhor_b].tempo_am += melhor_delta2;
        }


        // 4. Atualiza contadores e tempos na ROTA DE ORIGEM
        if(turno_i == 2){
            //cout <<"i no tempo pm\n";
            dias[melhor_d].rotas[melhor_a].tempo_pm += melhor_delta1;
        } else {
            //cout <<"i no tempo am\n";
            dias[melhor_d].rotas[melhor_a].tempo_am += melhor_delta1;
        }
        
        // 5. Atualiza o custo da solução global
        s.set_fo(s.get_fo() + melhor_delta); // melhor_delta == melhor_delta1 + melhor_delta2

        // 6. Efetiva as alterações nos vetores (Inter-rotas)
        // Remove da origem
        rota1.erase(rota1.begin() + melhor_i);
        rota2.insert(rota2.begin() + melhor_j,  paciente_movido);
        houve_melhoria_rotas = 1;
        
    }

    return houve_melhoria_rotas;
}

//fazer swap considerando dois vizinhos
bool Vizinhancas::inter_swap22(Solucao &s){

    bool houve_melhoria_rotas = 0;
    std::vector<Rotas_diarias>& dias = s.get_dias_planejamento();
    double delta, melhor_delta = 0;
    double melhor_delta1, melhor_delta2;                 
    int melhor_i=-1, melhor_j=-1, turno_i=-1, turno_j=-1;       
    int melhor_a=-1, melhor_b=-1, melhor_d=-1, melhor_e=-1;

    

	for(int d = 0; d < dias.size(); d++){   
        for(int a = 0; a < dias[d].rotas.size(); a++){
            for(int t1 = 1; t1 <= 2; t1++){
                
                const vector<int>& rota1 = (t1 == 1) ? dias[d].rotas[a].sequencia_am : dias[d].rotas[a].sequencia_pm;
                double disponive11 = (t1 == 1) ? dias[d].get_tempo_disponivel_am(a) : dias[d].get_tempo_disponivel_pm(a);

                if(rota1.size() <= 3){
                    continue;
                }
                
                //i representa o inicio do par do agente a e do dia d
                for(int i = 1; i < rota1.size()-3; i++){

                    //se o indice atual ou o próximo for a sede, nao pode 
                    if(rota1[i] == 0 or rota1[i+1] == 0){
                        continue;
                    }
                    
                    for(int e = 0; e < dias.size(); e++){

                        if((dados->prioritarios[rota1[i]] == true or dados->prioritarios[rota1[i+1]] == true) and e > 0){
                            continue;
                        }

                        for(int b = 0; b < dias[e].rotas.size(); b++){
                            for(int t2 = 1; t2 <= 2; t2++){

                                const vector<int>& rota2 = (t2 == 1) ? dias[e].rotas[b].sequencia_am : dias[e].rotas[b].sequencia_pm;
                                 double disponivel2 = (t2 == 1) ? dias[e].get_tempo_disponivel_am(b) : dias[e].get_tempo_disponivel_pm(b);
                                
                                if(rota2.size() <= 3) continue;

                                if(dias[d].rotas[a].id_rota == dias[e].rotas[b].id_rota and t1 == t2) continue;
                            
                                
                                
                                //j representa o inicio do par do agente b e do dia e
                                for(int j = 1; j < rota2.size()-3; j++){

                                    if((dados->prioritarios[rota2[j]] or dados->prioritarios[rota2[j+1]])){
                                        continue;
                                    }

                                    //se o indice atual ou o próximo for a sede, nao pode 
                                    if(rota2[j] == 0 or rota2[j+1] == 0){
                                        continue;
                                    }
                                    
                                    //se a rota não tem pacientes suficientes, então encerramos esse laço
                                    if( (j+2) >= rota2.size()){
                                        break;
                                    }

                                    double necessario_r1  = -dados->matriz_tempo[rota1[i-1]][rota1[i]]
                                                            -dados->matriz_tempo[rota1[i]][rota1[i+1]]
                                                            -dados->matriz_tempo[rota1[i+1]][rota1[i+2]]
                                                            +dados->matriz_tempo[rota1[i-1]][rota2[j]]
                                                            +dados->matriz_tempo[rota2[j]][rota2[j+1]]
                                                            +dados->matriz_tempo[rota2[j+1]][rota1[i+2]];
                                   
                                    if(necessario_r1 > disponive11){
                                        continue;
                                    }


                                    double necessario_r2 = -dados->matriz_tempo[rota2[j-1]][rota2[j]]
                                                        -dados->matriz_tempo[rota2[j+1]][rota2[j+2]] 
                                                        -dados->matriz_tempo[rota2[j]][rota2[j+1]] 
                                                        +dados->matriz_tempo[rota2[j-1]][rota1[i]] 
                                                        +dados->matriz_tempo[rota1[i]][rota1[i+1]] 
                                                        +dados->matriz_tempo[rota1[i+1]][rota2[j+2]] ;
                                    
                                    if(necessario_r2 > disponivel2){
                                        continue;
                                    }
                                    delta = necessario_r1 + necessario_r2;
                                    // se delta for menor que o melhor delta atual significa que melhorou a solucao
                                    if (delta < melhor_delta){ 

                                        melhor_delta = delta; 
                                        melhor_delta1 = necessario_r1;
                                        melhor_delta2 = necessario_r2;                                 
                                        melhor_i = i;          
                                        melhor_j = j;          
                                        turno_i = t1;
                                        turno_j = t2;
                                        melhor_a = a;
                                        melhor_b = b;
                                        melhor_d = d;
                                        melhor_e = e;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (melhor_delta < -dados->eps and melhor_a != -1){
        //cout << endl << "melhor rota1: " << melhor_d << " " << melhor_a << endl;
        //cout << "melhor rota2: " << melhor_e << " " << melhor_b << endl;
        //cout << "melhor i: " << melhor_i << " no turno" << turno_i << endl;
        //cout << "melhor j: " << melhor_j << " no turno" << turno_j << endl;  
        //dias[melhor_d].printa_rota(melhor_a);
        //dias[melhor_e].printa_rota(melhor_b);
        
        //debug
        vector<int>& rota1 = (turno_i == 1) ? dias[melhor_d].rotas[melhor_a].sequencia_am : dias[melhor_d].rotas[melhor_a].sequencia_pm;
        vector<int>& rota2 = (turno_j == 1) ? dias[melhor_e].rotas[melhor_b].sequencia_am : dias[melhor_e].rotas[melhor_b].sequencia_pm;
        
        //rota 2
        if(turno_j == 2){

            dias[melhor_e].rotas[melhor_b].tempo_pm += melhor_delta2;
        
        }else{
            dias[melhor_e].rotas[melhor_b].tempo_am += melhor_delta2;
        }
        //rota 1
        if(turno_i == 2){
            
            dias[melhor_d].rotas[melhor_a].tempo_pm += melhor_delta1;
        }else{

            dias[melhor_d].rotas[melhor_a].tempo_am += melhor_delta1;
        }

        //alterar funcao objetivo
        s.set_fo(s.get_fo() + melhor_delta);

        //efetivar troca--------------------------------------------------
        std::swap(rota1[melhor_i], rota2[melhor_j]);
        std::swap(rota1[melhor_i+1], rota2[melhor_j+1]);

        
        houve_melhoria_rotas = 1;

        //debug
        //cout << "melhor i: " << melhor_i << " no turno" << turno_i << endl;
        //cout << "melhor j: " << melhor_j << " no turno" << turno_j << endl;  
        //dias[melhor_d].printa_rota(melhor_a);
        //dias[melhor_e].printa_rota(melhor_b);
        
    }

    return houve_melhoria_rotas;
}

