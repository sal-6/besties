#include <iostream>
#include <string>

#include "d_star_lite/d_star_lite.h"


int main() {
    
    Grid true_grid = Grid(30, 30);
    Grid known_grid = Grid(30, 30);
    
    for (int i = 0; i < 25; i++) {
        true_grid.obstruct(15, i);
        known_grid.obstruct(15, i);
    }
    
    /* for (int i = 16; i < 26; i++) {
        true_grid.obstruct(i, 24);
    } */
    for (int i = 5; i < 15; i++) {
        true_grid.obstruct(i, 24);
    }
    
    Node* start = known_grid.get_node(0, 0);
    Node* goal = known_grid.get_node(29, 15);
    
    DStarLite d_star = DStarLite(start, goal, &known_grid);
    
    Path p = d_star.main_loop(start);
    
    
    int count = 0;
    while (p.nodes.size() > 1) {
        Node* next_pos = p.nodes[1];        
        
        std::vector<Edge*> changes = true_grid.get_changed_edges_about_node(next_pos, &known_grid, 3);
        
        d_star.queue_updated_edges(changes);
        d_star.map->update_grid_from_changed_edges(changes);

        //std::cout << "True vs Known: " << std::endl;
        //true_grid.log_grid();
        //std::cout << std::endl;
        //d_star.map->log_grid();
        
        p = d_star.main_loop(next_pos);
        
        p.export_to_file("./output/d_star_lite/path/path_" + std::to_string(count) + ".csv");
        d_star.map->export_obs_to_file("./output/d_star_lite/path/obs_" + std::to_string(count) + ".csv");
        true_grid.export_obs_to_file("./output/d_star_lite/path/true_obs_" + std::to_string(count) + ".csv");
        count++;
    }
    
    return 0;
}