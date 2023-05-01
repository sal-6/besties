#include <iostream>
#include <string>

#include "edge_costs/d_star_lite.h"



int main1() {
    srand(time(0));
    
    int start_x = 0;
    int start_y = 0;
    int goal_x = 29;
    int goal_y = 29;
    
    int width = 30;
    int height = 30;
    
    float time_step = 1;
    float sun_angle = PI / 4;
    
    float charge_rate = 1;
    float discharge_rate = 1;
    
    TimeManager time_manager = TimeManager();
    ShadowManager shadow_manager = ShadowManager(sun_angle);
    Rover rover = Rover(discharge_rate, charge_rate);
    
    ShadowPillar sp1 = ShadowPillar(20, 14, 7, 0, 0.2, 10);
    shadow_manager.add_pillar(&sp1); 
    
    Grid true_grid = Grid(width, height, &shadow_manager);
    Grid known_grid = Grid(width, height, &shadow_manager);

    for (int i = 0; i < true_grid.width; i++) {
        for (int j = 0; j < true_grid.height; j++) {
            int h = rand() % 20;
            h = 1;
            known_grid.set_height(i, j, h);
            true_grid.set_height(i, j, h);
        }
    }
    
    /* for (int i = 0; i < 25; i++) {
        true_grid.obstruct(15, i);
        known_grid.obstruct(15, i);
    } */
    
    // add 100 random obstacles that are not on the start or goal
    /* for (int i = 0; i < 100; i++) {
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
    } */
    
    
    true_grid.export_topology_to_file("./output/edge_costs/path/topo.csv");
    true_grid.export_obs_to_file("./output/edge_costs/path/true_obs.csv");
    
    
    Node* start = known_grid.get_node(start_x, start_y);
    Node* goal = known_grid.get_node(goal_x, goal_y);
    
    DStarLite d_star = DStarLite(start, goal, &known_grid, &rover);
    std::vector<Edge*> changes = true_grid.get_changed_edges_about_node(start, &known_grid, 3, &rover, 0, time_manager.get_time());
    d_star.queue_updated_edges(changes);
    d_star.map->update_grid_from_changed_edges(changes);
    
    Path p = d_star.main_loop(start, 0);
    
    int count = 0;
    while (p.nodes.size() > 1) {
        float old_time = time_manager.get_time();
        time_manager.step_time(time_step);
        Node* next_pos = p.nodes[1];
        float light = shadow_manager.get_light_level_at_time(next_pos->x, next_pos->y, time_manager.get_time());
        rover.update_battery_level(time_step, light);     
        
        std::vector<Edge*> changes = true_grid.get_changed_edges_about_node(next_pos, &known_grid, 3, &rover, old_time, time_manager.get_time());
        
        d_star.queue_updated_edges(changes);
        d_star.map->update_grid_from_changed_edges(changes);

        //std::cout << "True vs Known: " << std::endl;
        //true_grid.log_grid();
        //std::cout << std::endl;
        //d_star.map->log_grid();
        
        p = d_star.main_loop(next_pos, time_manager.get_time());
        
        p.export_to_file("./output/edge_costs/path/path_" + std::to_string(count) + ".csv");
        d_star.map->export_obs_to_file("./output/edge_costs/path/obs_" + std::to_string(count) + ".csv");
        shadow_manager.export_shadows_to_file("./output/edge_costs/path/shadow_" + std::to_string(count) + ".csv", time_manager.get_time());
        count++;
    }
    
    return 0;
}