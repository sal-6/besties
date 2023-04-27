
#include "field_d_star/field_d_star.h"
#include <iostream>


void test_consec(){
    Grid g = Grid(10, 10);
    
    std::vector<NeighborPair> pairs = g.connbrs(g.get_node(0, 0));
    
    for (int i = 0; i < pairs.size(); i++){
        std::cout << pairs[i].n1->x << ", " << pairs[i].n1->y << " and " << pairs[i].n2->x << ", " << pairs[i].n2->y << std::endl;
    }
}

void test_1() {
    
    Grid g = Grid(10, 10);
    
    Node* start_node = g.get_node(0, 0);
    Node* goal_node = g.get_node(3, 7);
    
    FieldDStar fds = FieldDStar(start_node, goal_node, &g);
    
    Path p = fds.main(start_node);
    
    g.log_cost_map();
    
    
}

int main() {
    
    test_1();
}