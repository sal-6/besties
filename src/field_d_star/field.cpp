
#include "field_d_star/field_d_star.h"
#include <iostream>


void test_consec(){
    Grid g = Grid(10, 10);
    
    std::vector<NeighborPair> pairs = g.connbrs(g.get_node(0, 0));
    
    for (int i = 0; i < pairs.size(); i++){
        std::cout << pairs[i].n1->x << ", " << pairs[i].n1->y << " and " << pairs[i].n2->x << ", " << pairs[i].n2->y << std::endl;
    }
}

int main() {
    
    test_consec();
}