#include <iostream>
#include <string>

#include "edge_costs/d_star_lite.h"

// primar func
int main() {
    srand(time(0));
    
    int start_x = 10;
    int start_y = 10;
    int goal_x = 115;
    int goal_y = 115;
    
    int width = 129;
    int height = 129;
    
    int num_random_obs = 100;
    int max_obs_side = 1;
    
    float time_step = 1;
    float sun_angle = PI / 4;
    
    float charge_rate = 1;
    float discharge_rate = 1;
    float r_rad = 2;
    
    int scan_radius = 3;
    
    std::string topo_file = "./data/height_128_128_proc.txt";
    
    TimeManager time_manager = TimeManager();
    ShadowManager shadow_manager = ShadowManager(sun_angle);
    Rover rover = Rover(discharge_rate, charge_rate, r_rad);
    
    ShadowPillar sp1 = ShadowPillar(20, 14, 7, 0, 0.2, 10);
    shadow_manager.add_pillar(&sp1); 
    
    Grid true_grid = Grid(width, height, &shadow_manager);
    Grid known_grid = Grid(width, height, &shadow_manager);

    printf("Parsing grid heights from file...\n");    
    
    true_grid.parse_grid_heights_from_file(topo_file);
    known_grid.parse_grid_heights_from_file(topo_file);
        
    true_grid.export_topology_to_file("./output/edge_costs/path/topo.csv");
    
    printf("Parsing grid heights from file...done\n");
    
    printf("Adding unknown obstacles...\n");
    
    // add 200 random obstacles that are not on the start or goal
    for (int i = 0; i < num_random_obs; i++) {
        int x = rand() % width;
        int y = rand() % height;
        
        Obstacle* rand_o = generate_random_obstacle(x, y, max_obs_side);
        Obstacle* expanded_o = expand_obstacle(rand_o, rover);
        
        // loop through all the points in the expanded obstacle and obstruct them
        for (Node* n : expanded_o->nodes) {
            bool is_valid = true;
            
            // check if the node is start
            if (n->x == start_x && n->y == start_y) {
                is_valid = false;
            }
            
            // check if the node is goal
            if (n->x == goal_x && n->y == goal_y) {
                is_valid = false;
            }
            
            if (is_valid) {
                true_grid.obstruct(n->x, n->y);
            }
        }

    }        
    
    
    printf("Adding unknown obstacles...done\n");
    
    Node* start = known_grid.get_node(start_x, start_y);
    Node* goal = known_grid.get_node(goal_x, goal_y);
    
    std::cout << "Initializing D* Lite ..." << std::endl;
    DStarLite d_star = DStarLite(start, goal, &known_grid, &rover);
    std::vector<Edge*> changes = true_grid.get_changed_edges_about_node(start, &known_grid, 3, &rover, 0, time_manager.get_time());
    d_star.queue_updated_edges(changes);
    d_star.map->update_grid_from_changed_edges(changes);
    
    std::cout << "Done initializing D* Lite." << std::endl;
    
    std::cout << "Running D* Lite ..." << std::endl;
    
    Path p = d_star.main_loop(start, 0);
    
    std::vector<float> block_points = std::vector<float>();
    block_points.push_back(0.2);
    block_points.push_back(0.35);
    block_points.push_back(0.4);
    block_points.push_back(0.6);
    block_points.push_back(0.8);
    
    for (float bp : block_points) {
        int ind = (int) (p.nodes.size() * bp);
        Obstacle* o = generate_random_obstacle(p.nodes[ind]->x, p.nodes[ind]->y, max_obs_side);
        Obstacle* expanded_o = expand_obstacle(o, rover);
    
        // loop through all the points in the expanded obstacle and obstruct them
        for (Node* n : expanded_o->nodes) {
            bool is_valid = true;
            
            // check if the node is start
            if (n->x == start_x && n->y == start_y) {
                is_valid = false;
            }
            
            // check if the node is goal
            if (n->x == goal_x && n->y == goal_y) {
                is_valid = false;
            }
            
            if (is_valid) {
                true_grid.obstruct(n->x, n->y);
            }
        }
    }
    
    true_grid.export_obs_to_file("./output/edge_costs/path/true_obs.csv");
    
    std::cout << "Hee" << std::endl;
    
    int count = 0;
    while (p.nodes.size() > 1) {
        float old_time = time_manager.get_time();
        time_manager.step_time(time_step);
        Node* next_pos = p.nodes[1];
        float light = shadow_manager.get_light_level_at_time(next_pos->x, next_pos->y, time_manager.get_time());
        rover.update_battery_level(time_step, light);     
        
        std::vector<Edge*> changes = true_grid.get_changed_edges_about_node(next_pos, &known_grid, scan_radius, &rover, old_time, time_manager.get_time());
        
        d_star.queue_updated_edges(changes);
        d_star.map->update_grid_from_changed_edges(changes);
        
        p = d_star.main_loop(next_pos, time_manager.get_time());
        
        p.export_to_file("./output/edge_costs/path/path_" + std::to_string(count) + ".csv");
        d_star.map->export_obs_to_file("./output/edge_costs/path/obs_" + std::to_string(count) + ".csv");
        shadow_manager.export_shadows_to_file("./output/edge_costs/path/shadow_" + std::to_string(count) + ".csv", time_manager.get_time());
        count++;
    }
    
    return 0;
}

int main1() {
    
    srand(time(0));
    Rover r = Rover(1, 1, 2);
    
    Obstacle* o = generate_random_obstacle(0, 0, 4);
    
    std::cout << "Obstacle: " << std::endl;
    // print all node x and y values in the obstacle
    for (int i = 0; i < o->nodes.size(); i++) {
        std::cout << o->nodes[i]->x << ", " << o->nodes[i]->y << std::endl;
    }
    
    Obstacle* exp_obs = expand_obstacle(o, r);
    
    
    std::cout << "Exp. Obstacle: " << std::endl;
    // print all node x and y values in the obstacle
    for (int i = 0; i < exp_obs->nodes.size(); i++) {
        std::cout << exp_obs->nodes[i]->x << ", " << exp_obs->nodes[i]->y << std::endl;
    }
    
    
    return 0;
}