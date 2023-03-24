#include "d_star_lite/d_star_lite.h"
#include <iostream>

int main() {
    Grid grid = Grid(10, 10);
    Node* n = grid.get_node(1, 1);
    
    std::cout << n->x << ", " << n->y << std::endl;
    
    n->x = 2;
    
    std::cout << n->x << ", " << n->y << std::endl;
    
    Node* n2 = grid.get_node(1, 1);
    
    std::cout << n2->x << ", " << n2->y << std::endl;
    
    
    return 0;
}