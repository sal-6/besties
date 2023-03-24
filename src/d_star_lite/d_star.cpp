#include <iostream>

#include "d_star_lite/d_star_lite.h"


int main() {
    
    Grid grid = Grid(30, 30);
    
    Node* start = grid.get_node(0, 0);
    Node* goal = grid.get_node(29, 29);
    
    DStarLite d_star = DStarLite(start, goal, &grid);
    
    
    
    
    return 0;
}