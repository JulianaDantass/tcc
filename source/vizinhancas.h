/*
Intra
Reinsertion— One customer is removed and inserted in another position of the route.
Or-opt2— Two adjacent customers are removed and inserted in another position of the route.
Exchange— Permutation between two customers.

Inter
Shift(1,0)– A customer k is transferred from a route r1 to a route r2.
Swap(1,1)— Permutation between a customer k from a route r1 and acustomer l, from a route r2.
Swap(2,2)— Permutation of two adjacent customers, k and l, from a router1 by a customer k' and l' from a route r2.
*/

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "solucao.h"
#include "instancia.h"


//tem que ver se o calculo de cada delta tá certo

class Vizinhancas{    
    
    private:
        const Instancia *dados;
    public:
        Vizinhancas(const Instancia *d);
        void set_dados(Instancia *d);
        bool intra_reinsertion(Solucao &s); 
        bool intra_oropt2(Solucao &s);      
        bool intra_exchange(Solucao &s);    
        bool inter_shift10(Solucao &s);     
        bool inter_swap11(Solucao &s);      
        bool inter_swap22(Solucao &s);      
        
        //debug
        void printa(Rota &r, int mi, int mj);
};

