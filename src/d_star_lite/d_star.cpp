#include <iostream>
#include <string>

#include "d_star_lite/d_star_lite.h"


int main() {
    
    Grid grid = Grid(30, 30);
    
    for (int i = 0; i < 25; i++) {
        grid.obstruct(15, i);
    }
    
    Node* start = grid.get_node(0, 0);
    Node* goal = grid.get_node(29, 15);
    
    DStarLite d_star = DStarLite(start, goal, &grid);
    
    Path p = d_star.main_loop(start);
    
    int count = 0;
    while (p.nodes.size() > 1) {
        Node* next_pos = p.nodes[1];
        p = d_star.main_loop(next_pos);
        
        p.export_to_file("./output/d_star_lite/path/path_" + std::to_string(count) + ".csv");
        d_star.map->export_obs_to_file("./output/d_star_lite/path/obs_" + std::to_string(count) + ".csv");
        count++;
    }
    
    return 0;
}