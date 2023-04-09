#include <iostream>
#include <string>

#include "edge_costs/d_star_lite.h"


int main() {
    srand(time(0));
    
    Grid true_grid = Grid(30, 30);
    Grid known_grid = Grid(30, 30);
    
    /* for (int i = 0; i < 25; i++) {
        true_grid.obstruct(15, i);
        known_grid.obstruct(15, i);
    } */
    
    /*
    for (int i = 16; i < 26; i++) {
        true_grid.obstruct(i, 24);
    } 
    for (int i = 6; i < 15; i++) {
        true_grid.obstruct(i, 24);
    } */

    for (int i = 0; i < true_grid.width; i++) {
        for (int j = 0; j < true_grid.height; j++) {
            int h = rand() % 20;
            known_grid.set_height(i, j, h);
            true_grid.set_height(i, j, h);
        }
    }
    
    // add 100 random obstacles that are not on the start or goal
    for (int i = 0; i < 100; i++) {
        int x = rand() % 30;
        int y = rand() % 30;
        
        if (x == 0 && y == 0) {
            i--;
            continue;
        }
        
        if (x == 29 && y == 15) {
            i--;
            continue;
        }
        
        true_grid.obstruct(x, y);
        //known_grid.obstruct(x, y);
    }
    
    /* for (int i = 0; i < 30; i++) {
        known_grid.set_height(i, 0, 0);
        true_grid.set_height(i, 0, 0);
    }
    
    for (int i =0; i < 16; i++) {
        known_grid.set_height(29, i, 0);
        true_grid.set_height(29, i, 0);
    } */

    true_grid.export_topology_to_file("./output/edge_costs/path/topo.csv");
    true_grid.export_obs_to_file("./output/edge_costs/path/true_obs.csv");
    
    
    Node* start = known_grid.get_node(0, 0);
    Node* goal = known_grid.get_node(29, 15);
    
    DStarLite d_star = DStarLite(start, goal, &known_grid);
    std::vector<Edge*> changes = true_grid.get_changed_edges_about_node(start, &known_grid, 3);
    d_star.queue_updated_edges(changes);
    d_star.map->update_grid_from_changed_edges(changes);
    
    
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
        
        p.export_to_file("./output/edge_costs/path/path_" + std::to_string(count) + ".csv");
        d_star.map->export_obs_to_file("./output/edge_costs/path/obs_" + std::to_string(count) + ".csv");
        count++;
    }
    
    return 0;
}