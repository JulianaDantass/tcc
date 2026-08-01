#include "vizinhancas.h"
#include <iostream>
#include <fstream>
#include <algorithm>

#define EPSILON 1e-4 //margem de tolerancia
#define DURACAO 0.25


Vizinhancas::Vizinhancas(const Instancia *d){
    this->dados = d;
}

bool Vizinhancas::intra_reinsertion(Solucao &s){

    bool houve_melhoria_rotas = 0;
    auto dias = s.get_dias_planejamento();
    double delta, 
    melhor_delta = 0;                 

    int melhor_i, melhor_j;       
    int melhor_a, melhor_d;

	for(int d = 0; d < dias.size(); d++){   
        for(int a = 0; a < dias[d].rotas.size(); a++){

            int tam_rota = dias[d].rotas[a].sequencia.size();
            const auto& seq = dias[d].rotas[a].sequencia;
            double disponivel_am = dias[d].get_tempo_disponivel_am(a);
            double disponivel_pm = dias[d].get_tempo_disponivel_pm(a);

            //o indice i representa o paciente a ser deslocado
            //o indice j '' os espaços onde o paciente pode ser inserido, sendo j=1 o espaço entre o local de partida (0) e a 1ª casa

            for(int i = 1; i < tam_rota-1; i++){

                if(seq[i] == 0){
                    continue;
                }
                
                for(int j = 1; j < tam_rota; j++){

                    //tem q rever caso de adjacencia
                    if(i == j  or j == i+1){
                        continue;
                    }

                    //para posição da tarde j um paciente i da manhã  
                    if(j > dias[d].rotas[a].qtd_pacientes_am+1 and i <= dias[d].rotas[a].qtd_pacientes_am){
                        
                        double necessario = DURACAO 
                                            + dados->matriz_distancias[seq[j-1]][seq[i]]
                                            + dados->matriz_distancias[seq[i]][seq[j]]  
                                            - dados->matriz_distancias[seq[j-1]][seq[j]];

                        //mov invalido 
                        if(disponivel_pm < necessario){   
                            continue;
                        }
                    }  

                    //para a posicao j da manha um paciente i da tarde 
                    else if (j <= dias[d].rotas[a].qtd_pacientes_am+1 and i > dias[d].rotas[a].qtd_pacientes_am+1){
                       
                        double necessario = DURACAO
                                            + dados->matriz_distancias[seq[i-1]][seq[i+1]]
                                            + dados->matriz_distancias[seq[i]][seq[j]]  
                                            - dados->matriz_distancias[seq[j-1]][seq[j]];
                        
                        //mov invalido
                        if(disponivel_am < necessario){ 
                            continue;
                        }
                    }

                    delta = + dados->matriz_distancias[seq[i-1]][seq[i+1]] + dados->matriz_distancias[seq[j-1]][seq[i]] 
                            + dados->matriz_distancias[seq[i]][seq[j]]     - dados->matriz_distancias[seq[i-1]][seq[i]] 
                            - dados->matriz_distancias[seq[i]][seq[i+1]]   - dados->matriz_distancias[seq[j-1]][seq[j]];
                
                    // se delta for menor que o melhor delta atual significa que melhorou a solucao
                    if (delta < melhor_delta){ 

                        melhor_delta = delta;  
                        melhor_i = i;          
                        melhor_j = j;          
                        melhor_a = a;
                        melhor_d = d;
                    }
                }
		    }
        }
    }

   
    
    if (melhor_delta < -double(EPSILON)){

        for(auto c: dias[melhor_d].rotas[melhor_a].sequencia){
            cout << c << " ";
        }
        cout << endl;
    

        //alterando os tempos
        auto& seq = dias[melhor_d].rotas[melhor_a].sequencia;

        double dif_i = +dados->matriz_distancias[seq[melhor_i-1]][seq[melhor_i+1]] 
                        -dados->matriz_distancias[seq[melhor_i-1]][seq[melhor_i]] 
                        -dados->matriz_distancias[seq[melhor_i]][seq[melhor_i+1]];
        
        double dif_j = +dados->matriz_distancias[seq[melhor_j-1]][seq[melhor_i]] 
                        +dados->matriz_distancias[seq[melhor_i]][seq[melhor_j]]  
                        -dados->matriz_distancias[seq[melhor_j-1]][seq[melhor_j]];

        
        //posicao da tarde
        if(melhor_j > dias[melhor_d].rotas[melhor_a].qtd_pacientes_am+1){

            //paciente da manha na tarde
            if(melhor_i <= dias[melhor_d].rotas[melhor_a].qtd_pacientes_am){

                dias[melhor_d].rotas[melhor_a].qtd_pacientes_am--;
                dias[melhor_d].rotas[melhor_a].tempo_am = dias[melhor_d].rotas[melhor_a].tempo_am - DURACAO + dif_i;
                dias[melhor_d].rotas[melhor_a].tempo_pm += DURACAO + dif_j; 
            
            //paciente da tarde na tarde
            }else{
                
                dias[melhor_d].rotas[melhor_a].tempo_pm += melhor_delta;
            }
        
        //posicao da manha
        }else{
            
            //paciente da tarde na manhã
            if(melhor_i > (dias[melhor_d].rotas[melhor_a].qtd_pacientes_am+1) ){

                dias[melhor_d].rotas[melhor_a].qtd_pacientes_am++;
                dias[melhor_d].rotas[melhor_a].tempo_am += DURACAO + dif_j; 
                dias[melhor_d].rotas[melhor_a].tempo_pm = dias[melhor_d].rotas[melhor_a].tempo_pm - DURACAO + dif_i;
            
            //paciente da manha para manha
            }else{
                dias[melhor_d].rotas[melhor_a].tempo_am += melhor_delta;
            }
        }
        
        dias[melhor_d].rotas[melhor_a].tempo_total = dias[melhor_d].rotas[melhor_a].tempo_total + melhor_delta;
        s.set_fo(s.get_fo() + melhor_delta);

        
        //alterando a rota
        if(melhor_i < melhor_j) {
            seq.insert(seq.begin() + melhor_j, seq[melhor_i]);
            seq.erase(seq.begin() + melhor_i);

        }else{
            seq.insert(seq.begin() + melhor_j, seq[melhor_i]);
            seq.erase(seq.begin() + melhor_i + 2);
        }
        
        //corfirmando que houve melhoria em alguma rota
        houve_melhoria_rotas = 1;     
        
    }

    return houve_melhoria_rotas;
}

bool Vizinhancas::intra_oropt2(Solucao &s){

    bool houve_melhoria_rotas = 0;
    auto dias = s.get_dias_planejamento();
    double delta, melhor_delta = 0;              
    int melhor_i, melhor_j;       
    int melhor_a, melhor_d;

	for(int d = 0; d < dias.size(); d++){   
        for(int a = 0; a < dias[d].rotas.size(); a++){

            int tam_rota = dias[d].rotas[a].sequencia.size();
            const auto& seq = dias[d].rotas[a].sequencia;
            double disponivel_am = dias[d].get_tempo_disponivel_am(a);
            double disponivel_pm = dias[d].get_tempo_disponivel_pm(a);
            

            //o indice i representa o paciente a ser deslocado
            //o indice j '' os espaços onde o paciente pode ser inserido, sendo j=1 o espaço entre o local de partida (0) e a 1ª casa

            for(int i = 1; i < tam_rota-2; i++){

                //se um dos deslocados for o nó 0
                if(seq[i] == 0 or seq[i+1] == 0){
                    continue;
                }

                double duracao_i = DURACAO;
                delta = -dados->matriz_distancias[seq[i-1]][seq[i]] 
                        -dados->matriz_distancias[seq[i+1]][seq[i+2]] 
                        +dados->matriz_distancias[seq[i-1]][seq[i+2]];
                
                for(int j = 1; j < tam_rota-1; j++){

                    if(i == j or j == i+1 or j == i+2){
                        continue;
                    }

                    double necessario = delta + (2*duracao_i)
                                        -dados->matriz_distancias[seq[j-1]][seq[j]] 
                                        +dados->matriz_distancias[seq[j-1]][seq[i]]
                                        +dados->matriz_distancias[seq[i+1]][seq[j]];
                    
                    //para posição da tarde j, 2 pacientes i do turno da manhã  
                    if(j > dias[d].rotas[a].qtd_pacientes_am+1 and i < dias[d].rotas[a].qtd_pacientes_am){
                        
                        if(disponivel_pm < necessario){  //mov invalido  
                            continue;
                        }
                    }  

                    //para a posicao j da manha, 2 pacientes i do turno da tarde 
                    else if (j <= dias[d].rotas[a].qtd_pacientes_am+1 and i > dias[d].rotas[a].qtd_pacientes_am){
                       
                        if(disponivel_am < necessario){ //mov invalido
                            continue;
                        }
                    }
                    
                    delta = necessario - (2*duracao_i); 
                    
                    if (delta < melhor_delta){ 

                        melhor_delta = delta;  
                        melhor_i = i;          
                        melhor_j = j;          
                        melhor_a = a;
                        melhor_d = d;
                    }
                }
		    }
        }
    }

    
    if (melhor_delta < -double(EPSILON)){

        //alterando os tempos
        auto& seq = dias[melhor_d].rotas[melhor_a].sequencia;

        double dif_i = -dados->matriz_distancias[seq[melhor_i-1]][seq[melhor_i]] 
                       -dados->matriz_distancias[seq[melhor_i+1]][seq[melhor_i+2]] 
                       +dados->matriz_distancias[seq[melhor_i-1]][seq[melhor_i+2]];
        
        double dif_j = -dados->matriz_distancias[seq[melhor_j-1]][seq[melhor_j]] 
                       +dados->matriz_distancias[seq[melhor_j-1]][seq[melhor_i]]
                       +dados->matriz_distancias[seq[melhor_i+1]][seq[melhor_j]];


        //posicao da tarde            
        if(melhor_j > dias[melhor_d].rotas[melhor_a].qtd_pacientes_am+1){

            //pacientes da manhã 
            if(melhor_i < dias[melhor_d].rotas[melhor_a].qtd_pacientes_am){

                dias[melhor_d].rotas[melhor_a].qtd_pacientes_am -= 2;
                dias[melhor_d].rotas[melhor_a].tempo_am -=  2*DURACAO + dif_i;
                dias[melhor_d].rotas[melhor_a].tempo_pm +=  2*DURACAO + dif_j; 
            
            //pacientes da tarde
            }else{
                dias[melhor_d].rotas[melhor_a].tempo_pm += melhor_delta;
            }
        
        //posicao da manhã
        }else{ 
            
            //paciente da tarde
            if(melhor_i > dias[melhor_d].rotas[melhor_a].qtd_pacientes_am){

                dias[melhor_d].rotas[melhor_a].qtd_pacientes_am += 2;
                dias[melhor_d].rotas[melhor_a].tempo_am += 2*DURACAO + dif_j; 
                dias[melhor_d].rotas[melhor_a].tempo_pm -= 2*DURACAO + dif_i;
            
            //paciente da manhã    
            }else{
                dias[melhor_d].rotas[melhor_a].tempo_am += melhor_delta;
            }
        }
        
        dias[melhor_d].rotas[melhor_a].tempo_total +=  melhor_delta;
        s.set_fo(s.get_fo() + melhor_delta);

        //alterando a rota  
        if(melhor_i < melhor_j) {
            seq.insert(seq.begin() + melhor_j + 1, seq[melhor_i]);
            seq.insert(seq.begin() + melhor_j + 2, seq[melhor_i+1]);
            seq.erase(seq.begin() + (melhor_i + 1));
            seq.erase(seq.begin() + melhor_i);

        }else{
            seq.insert(seq.begin() + melhor_j, seq[melhor_i]);
            seq.insert(seq.begin() + melhor_j + 1, seq[melhor_i+2]);
            seq.erase(seq.begin() + melhor_i + 2);
            seq.erase(seq.begin() + melhor_i + 2);
        }
        houve_melhoria_rotas = 1;     //corfirmando que houve melhoria em alguma rota
    }

    //confere(dias[melhor_d].rotas[melhor_a]);getchar();

    return houve_melhoria_rotas;
}

bool Vizinhancas::intra_exchange(Solucao &s){
    
    bool houve_melhoria_rotas = 0;
    auto dias = s.get_dias_planejamento();

    //vai guardar a diferenca de valor em relacao a solucao de um dos dias
    double delta, 
    melhor_delta = 0;                 

    //vao guardar os indices de melhor troca
    int melhor_i, melhor_j, melhor_d;       
    int melhor_a;

	for(int d = 0; d < dias.size(); d++){   
        for(int a = 0; a < dias[d].rotas.size(); a++){

            int tam_rota = dias[d].rotas[a].sequencia.size();
            const auto& seq = dias[d].rotas[a].sequencia;
            double disponivel_am = dias[d].get_tempo_disponivel_am(a);
            double disponivel_pm = dias[d].get_tempo_disponivel_pm(a);

            for(int i = 1; i < tam_rota-1; i++){

                if(seq[i] == 0){ //tqv se isso faz sentido  
                    continue;
                }

                
                delta = -dados->matriz_distancias[seq[i-1]][seq[i]] - dados->matriz_distancias[seq[i]][seq[i+1]];
                
                for(int j = i+1; j < tam_rota-1; j++){
                    
                    if(seq[j] == 0){
                        continue;
                    
                    }else if(i == j-1){
                        delta = -dados->matriz_distancias[seq[i-1]][seq[i]] - dados->matriz_distancias[seq[j]][seq[j+1]]
                                -dados->matriz_distancias[seq[i]][seq[j]]      
                                +dados->matriz_distancias[seq[i-1]][seq[j]] + dados->matriz_distancias[seq[i]][seq[j+1]]
                                +dados->matriz_distancias[seq[j]][seq[i]];
                    }else{
                        delta += -dados->matriz_distancias[seq[j]][seq[j+1]] - dados->matriz_distancias[seq[j-1]][seq[j]] 
                                 +dados->matriz_distancias[seq[i-1]][seq[j]] + dados->matriz_distancias[seq[j]][seq[i+1]] 
                                 +dados->matriz_distancias[seq[i]][seq[j+1]] + dados->matriz_distancias[seq[j-1]][seq[i]];
                    }
                
                    // se delta for menor que o melhor delta atual significa que melhorou a solucao
                    if (delta < melhor_delta){ 

                        melhor_d = d;
                        melhor_delta = delta;  
                        melhor_i = i;          
                        melhor_j = j;          
                        melhor_a = a;
                    }
                }
		    }
        }   
    }

    /*
    cout << "melhor i: " << melhor_i << " melhor j: " << melhor_j << endl;
        cout << "rota: ";
        for(auto c: dias[d].rotas[melhor_a].sequencia){
            cout << c << " ";
        }
        cout << endl;
    */

    if (melhor_delta < -double(EPSILON)){
        
        auto& rota = dias[melhor_d].rotas[melhor_a];
        const auto& seq = rota.sequencia;
        
        //alteracoes do i

        if (melhor_i != melhor_j-1) {
            delta = -dados->matriz_distancias[seq[melhor_i-1]][seq[melhor_i]] - dados->matriz_distancias[seq[melhor_i]][seq[melhor_i+1]] 
                    +dados->matriz_distancias[seq[melhor_i-1]][seq[melhor_j]] + dados->matriz_distancias[seq[melhor_j]][seq[melhor_i+1]]; 
                                
                

            if(melhor_i > rota.qtd_pacientes_am) {
                rota.tempo_pm += delta; 
            }else{
                rota.tempo_am += delta; 
            }

            delta = -dados->matriz_distancias[seq[melhor_j-1]][seq[melhor_j]] - dados->matriz_distancias[seq[melhor_j]][seq[melhor_j+1]] 
                    +dados->matriz_distancias[seq[melhor_i]][seq[melhor_j+1]] + dados->matriz_distancias[seq[melhor_j-1]][seq[melhor_i]]; 

            if(melhor_j > rota.qtd_pacientes_am) {
                rota.tempo_pm += delta; 
            }else{
                rota.tempo_am += delta; 
            }
        
        }else{   //se i=j-1, eles estão no mesmo turno

            if(melhor_j > rota.qtd_pacientes_am) {
               rota.tempo_pm = rota.tempo_pm + melhor_delta; 
            }else{
                rota.tempo_am = rota.tempo_am + melhor_delta; 
            }
        }
            
        
        dias[melhor_d].rotas[melhor_a].tempo_total = dias[melhor_d].rotas[melhor_a].tempo_total + melhor_delta;
        s.set_fo(s.get_fo() + melhor_delta);

        //executando o movimento
        swap(dias[melhor_d].rotas[melhor_a].sequencia[melhor_i], dias[melhor_d].rotas[melhor_a].sequencia[melhor_j]);
                    
        //corfirmando que houve melhoria em alguma rota
        houve_melhoria_rotas = 1;     
    }

    return houve_melhoria_rotas;
}

bool Vizinhancas::inter_shift10(Solucao &s){  

    bool houve_melhoria_rotas = 0;
    auto dias = s.get_dias_planejamento();
    //vai guardar a diferenca de valor em relacao a solucao de um dos dias
    double delta, 
    melhor_delta = 0, melhor_delta1, melhor_delta2;                 

    //vao guardar os indices de melhor troca
    int melhor_i, melhor_j;       
    int melhor_a, melhor_b, melhor_d, melhor_e;

    

	for(int d = 0; d < dias.size(); d++){   
        for(int a = 0; a < dias[d].rotas.size(); a++){
            const auto& rota1 = dias[d].rotas[a].sequencia;

            //i representa um paciente da seq1 do agente a do dia d; começa do 1 pois a posicao 0 é deposito
            for(int i = 1; i < rota1.size(); i++){

                if(rota1[i] == 0){
                    continue;
                }

                double necessario1 = -DURACAO   
                        +dados->matriz_distancias[rota1[i-1]][rota1[i+1]] 
                        -dados->matriz_distancias[rota1[i-1]][rota1[i]] 
                        -dados->matriz_distancias[rota1[i]][rota1[i+1]];

                

                //teoricamente tirar um paciente não seria problema mas checo:
                if(i > dias[d].rotas[a].qtd_pacientes_am){

                    //mov invalido 
                    if(dias[d].get_tempo_disponivel_pm(a) < necessario1){    
                        continue;
                    }
                }else{ 
                    //mov invalido
                    if(dias[d].get_tempo_disponivel_am(a) < necessario1){
                        continue;
                    }
                }

                for(int e = 0; e < dias.size(); e++){
                    for(int b = 0; b < dias[e].rotas.size(); b++){
                        const auto& rota2 = dias[e].rotas[b].sequencia;

                        //se for a mesma rota, n vamos fzr nd 
                        if(dias[d].rotas[a].id_rota == dias[e].rotas[b].id_rota){ 
                            continue;
                        }

                        //j representa um espaco da seq2 do agente b do dia e
                        for(int j = 1; j < rota2.size(); j++){

                            double necessario2 = DURACAO  
                                                    -dados->matriz_distancias[rota2[j-1]][rota2[j]] 
                                                    +dados->matriz_distancias[rota2[j-1]][rota1[i]]
                                                    +dados->matriz_distancias[rota1[i]][rota2[j]];

                             

                            //tentativa de inserir no turno pm na posicao j
                            if(j > dias[e].rotas[b].qtd_pacientes_am){

                                //mov invalido
                                if(dias[e].get_tempo_disponivel_pm(b) < necessario2){    
                                    continue;
                                }
                            
                            //tentativa de inserir no turno am na posicao j
                            }else{ 
                                //mov invalido
                                if(dias[e].get_tempo_disponivel_am(b) < necessario2){
                                    continue;
                                }
                            }
                            
                            delta = necessario1+necessario2;
                        
                            // se delta for menor que o melhor delta atual significa que melhorou a solucao
                            if (delta < melhor_delta){ 

                                melhor_delta = delta;  
                                melhor_delta1 = necessario1;
                                melhor_delta2 = necessario2;
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

    if (melhor_delta < -double(EPSILON)){

        //debug
        cout << "a rota a seguir deve ter o cara da posicao " <<  melhor_i << " retirado" << endl;
        printa(dias[melhor_d].rotas[melhor_a]);
        cout << "a rota a seguir deve ter o cara colado na posicao " <<  melhor_j << endl;
        printa(dias[melhor_e].rotas[melhor_b]);
        
        
        if(melhor_j > dias[melhor_e].rotas[melhor_b].qtd_pacientes_am){

            dias[melhor_e].rotas[melhor_b].tempo_pm += melhor_delta2;
        
        }else{ 
            dias[melhor_e].rotas[melhor_b].tempo_am += melhor_delta2;
            dias[melhor_e].rotas[melhor_b].qtd_pacientes_am++;
        }
        dias[melhor_e].rotas[melhor_b].tempo_total += melhor_delta2;

        //retirando o custo do paciente da posi i

        if(melhor_i > dias[melhor_d].rotas[melhor_a].qtd_pacientes_am){
            
            dias[melhor_d].rotas[melhor_a].tempo_pm +=  melhor_delta1;
        }else{

            dias[melhor_d].rotas[melhor_a].tempo_am +=  melhor_delta1;
            dias[melhor_d].rotas[melhor_a].qtd_pacientes_am--;
        }
        dias[melhor_d].rotas[melhor_a].tempo_total += melhor_delta1;

        //funcao objetivo
        s.set_fo(s.get_fo() + melhor_delta);


        //efetivar troca
        dias[melhor_e].rotas[melhor_b].sequencia.insert(dias[melhor_e].rotas[melhor_b].sequencia.begin() + melhor_j, dias[melhor_d].rotas[melhor_a].sequencia[melhor_i]);
        dias[melhor_d].rotas[melhor_a].sequencia.erase(dias[melhor_d].rotas[melhor_a].sequencia.begin() + melhor_i);
        
        houve_melhoria_rotas = 1;

        //debug
        cout << "alt feitas:" << endl;
        printa(dias[melhor_d].rotas[melhor_a]);
        printa(dias[melhor_e].rotas[melhor_b]);
    }

    return houve_melhoria_rotas;
}

bool Vizinhancas::inter_swap11(Solucao &s){

    auto dias = s.get_dias_planejamento();
    bool houve_melhoria_rotas = 0;
    double delta, 
    melhor_delta = 0, melhor_delta1, melhor_delta2;                 

    //vao guardar os indices de melhor troca
    int melhor_i, melhor_j;       
    int melhor_a, melhor_b, melhor_d, melhor_e;

	for(int d = 0; d < dias.size(); d++){   
        for(int a = 0; a < dias[d].rotas.size(); a++){
            const auto& rota1 = dias[d].rotas[a].sequencia;
            
            //i representa um paciente da seq1 do agente a do dia d
            for(int i = 1; i < rota1.size(); i++){

                if(rota1[i] == 0){
                    continue;
                }

                delta = -dados->matriz_distancias[rota1[i-1]][rota1[i]] 
                        -dados->matriz_distancias[rota1[i]][rota1[i+1]];

                for(int e = 0; e < dias.size(); e++){
                    for(int b = 0; b < dias[e].rotas.size(); b++){
                        const auto& rota2 = dias[e].rotas[b].sequencia;

                        //se for a mesma rota, n vamos fzr nd 
                        if(dias[d].rotas[a].id_rota == dias[e].rotas[b].id_rota){ 
                            continue;
                        }

                        //j representa um paciente da seq2 do agente b do dia e
                        for(int j = 1; j < rota2.size()-1; j++){

                            if(rota2[j] == 0){
                                continue;
                            }
                            double necessario_r2 =  -dados->matriz_distancias[rota2[j]][rota2[j+1]] 
                                                    -dados->matriz_distancias[rota2[j-1]][rota2[j]]   
                                                    +dados->matriz_distancias[rota2[j-1]][rota1[i]]
                                                    +dados->matriz_distancias[rota1[i]][rota2[j+1]];
                                
                            
                            if(j > dias[e].rotas[b].qtd_pacientes_am){

                                //mov invalido
                                if(dias[e].get_tempo_disponivel_pm(b) < necessario_r2){    
                                    continue;
                                }
                            }else{ 
                                //mov invalido
                                if(dias[e].get_tempo_disponivel_am(b) < necessario_r2){
                                    continue;
                                }
                            }


                            double necessario_r1 = delta + dados->matriz_distancias[rota1[i-1]][rota2[j]]
                                                        + dados->matriz_distancias[rota2[j]][rota1[i+1]];

                            if(i > dias[d].rotas[a].qtd_pacientes_am){
                                //mov invalido
                                if(dias[d].get_tempo_disponivel_pm(a) < necessario_r1){    
                                    continue;
                                }
                            }else{ 
                                //mov invalido
                                if(dias[d].get_tempo_disponivel_am(a) < necessario_r1){
                                    continue;
                                }
                            }
                        
                            delta = necessario_r1 + necessario_r2;
                            // se delta for menor que o melhor delta atual significa que melhorou a solucao
                            if (delta < melhor_delta){ 

                                melhor_delta = delta;
                                melhor_delta1 = necessario_r1;
                                melhor_delta2 = necessario_r2;  
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

    if (melhor_delta < -double(EPSILON)){
        
        
        if(melhor_j > dias[melhor_e].rotas[melhor_b].qtd_pacientes_am){

            dias[melhor_e].rotas[melhor_b].tempo_pm += melhor_delta2;
        
        }else{ 
            dias[melhor_e].rotas[melhor_b].tempo_am += melhor_delta2;
            dias[melhor_e].rotas[melhor_b].qtd_pacientes_am++;
        }
        dias[melhor_e].rotas[melhor_b].tempo_total += melhor_delta2;


        if(melhor_i > dias[melhor_d].rotas[melhor_a].qtd_pacientes_am){
            
            dias[melhor_d].rotas[melhor_a].tempo_pm += melhor_delta1;
        }else{

            dias[melhor_d].rotas[melhor_a].tempo_am += melhor_delta1;
            dias[melhor_d].rotas[melhor_a].qtd_pacientes_am--;
        }
        dias[melhor_d].rotas[melhor_a].tempo_total += melhor_delta1;
        
        s.set_fo(s.get_fo() + melhor_delta);

        //efetivar troca--------------------------------------------------
        std::swap(dias[melhor_d].rotas[melhor_a].sequencia[melhor_i], dias[melhor_e].rotas[melhor_b].sequencia[melhor_j]);

        houve_melhoria_rotas = 1;
    }
    //confere(dias[melhor_d].rotas[melhor_a]);
    //confere(dias[melhor_e].rotas[melhor_b]);

    return houve_melhoria_rotas;
}

//fazer swap considerando dois vizinhos
bool Vizinhancas::inter_swap22(Solucao &s){

    bool houve_melhoria_rotas = 0;
    auto dias = s.get_dias_planejamento();
    double delta, melhor_delta = 0;
    double melhor_delta1, melhor_delta2;                 
    int melhor_i, melhor_j;       
    int melhor_a, melhor_b, melhor_d, melhor_e;


	for(int d = 0; d < dias.size(); d++){   
        for(int a = 0; a < dias[d].rotas.size(); a++){
            const auto& rota1 = dias[d].rotas[a].sequencia;
            
            //i representa o inicio do par do agente a e do dia d
            for(int i = 1; i < rota1.size()-2; i++){

                //se o indice atual ou o próximo for a sede, nao pode 
                if(rota1[i] == 0 or rota1[i+1] == 0){
                    continue;
                }

                delta = -dados->matriz_distancias[rota1[i-1]][rota1[i]]
                        -dados->matriz_distancias[rota1[i]][rota1[i+1]]
                        -dados->matriz_distancias[rota1[i+1]][rota1[i+2]];

                for(int e = 0; e < dias.size(); e++){
                    for(int b = 0; b < dias[e].rotas.size(); b++){
                        const auto& rota2 = dias[e].rotas[b].sequencia;

                        //se for a mesma rota, n vamos fzr nd 
                        if(dias[d].rotas[a].id_rota == dias[e].rotas[b].id_rota){ 
                            continue;
                        }

                        //j representa o inicio do par do agente b e do dia e
                        for(int j = 1; j < rota2.size()-2; j++){

                            //se o indice atual ou o próximo for a sede, nao pode 
                            if(rota2[j] == 0 or rota2[j+1] == 0){
                                continue;
                            }

                            double necessario_r2 = -dados->matriz_distancias[rota2[j-1]][rota2[j]]
                                                   -dados->matriz_distancias[rota2[j+1]][rota2[j+2]] 
                                                   -dados->matriz_distancias[rota2[j]][rota2[j+1]] 
                                                   +dados->matriz_distancias[rota2[j-1]][rota1[i]] 
                                                   +dados->matriz_distancias[rota1[i]][rota1[i+1]] 
                                                   +dados->matriz_distancias[rota1[i+1]][rota2[j+2]] ;
                            
                            if(j > dias[e].rotas[b].qtd_pacientes_am){

                                //mov invalido
                                if(dias[e].get_tempo_disponivel_pm(b) < necessario_r2){    
                                    continue;
                                }
                            }else{ 
                                //mov invalido
                                if(dias[e].get_tempo_disponivel_am(b) < necessario_r2){
                                    continue;
                                }
                            }


                            double necessario_r1 = delta + dados->matriz_distancias[rota1[i-1]][rota2[j]]
                                                         + dados->matriz_distancias[rota2[j]][rota2[j+1]]
                                                         + dados->matriz_distancias[rota2[j+1]][rota1[i+2]];

                            if(i > dias[d].rotas[a].qtd_pacientes_am){
                                //mov invalido
                                if(dias[d].get_tempo_disponivel_pm(a) < necessario_r1){    
                                    continue;
                                }
                            }else{ 
                                //mov invalido
                                if(dias[d].get_tempo_disponivel_am(a) < necessario_r1){
                                    continue;
                                }
                            }
                            
                            delta = necessario_r1+necessario_r2;
                            
                            // se delta for menor que o melhor delta atual significa que melhorou a solucao
                            if (delta < melhor_delta){ 

                                melhor_delta = delta; 
                                melhor_delta1 = necessario_r1;
                                melhor_delta2 = necessario_r2;                                 
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

    if (melhor_delta < -double(EPSILON)){

        //custos-----------------------------------------
        
        //rota 2
        if(melhor_j > dias[melhor_e].rotas[melhor_b].qtd_pacientes_am){

            dias[melhor_e].rotas[melhor_b].tempo_pm += melhor_delta2;
        
        }else{ 
            dias[melhor_e].rotas[melhor_b].tempo_am += melhor_delta2;
            
        }
        dias[melhor_e].rotas[melhor_b].tempo_total += melhor_delta2;


        //rota 1
        if(melhor_i > dias[melhor_d].rotas[melhor_a].qtd_pacientes_am){
            
            dias[melhor_d].rotas[melhor_a].tempo_pm += melhor_delta1;
        }else{

            dias[melhor_d].rotas[melhor_a].tempo_am += melhor_delta1;
        }
        dias[melhor_d].rotas[melhor_a].tempo_total += melhor_delta1;

        //alterar funcao objetivo
        s.set_fo(s.get_fo() + melhor_delta);

        //efetivar troca--------------------------------------------------
        std::swap(dias[melhor_d].rotas[melhor_a].sequencia[melhor_i], dias[melhor_e].rotas[melhor_b].sequencia[melhor_j]);
        std::swap(dias[melhor_d].rotas[melhor_a].sequencia[melhor_i+1], dias[melhor_e].rotas[melhor_b].sequencia[melhor_j+1]);

        houve_melhoria_rotas = 1;
    }

    //confere(dias[melhor_d].rotas[melhor_a]);
    //confere(dias[melhor_e].rotas[melhor_b]);

    return houve_melhoria_rotas;
}

void Vizinhancas::printa(Rota& r){

    double t_total = 0;
    double t_am = 0;
    double t_pm = 0;
    int qtd_paci_am = 0;
    int marca_tarde = 0;
    bool para_contagem = 0;


    for(int i = 0; i < r.sequencia.size(); i++){

        cout << r.sequencia[i] << " ";
    }
    cout << endl;
}