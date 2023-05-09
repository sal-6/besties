#include <limits>
#include <math.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include <cmath>

#include "edge_costs/d_star_lite.h"


float heuristic(Node* a, Node* b) {
    return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}


Rover::Rover(float drain_rate, float charge_rate, float radius) {
    this->battery_level = 100;
    this->drain_rate = drain_rate;
    this->charge_rate = charge_rate;
    this->radius = radius;
}

void Rover::update_battery_level(float d_time, float light_level) {
    if (light_level >= 0.9) {
        this->battery_level += this->charge_rate * d_time;
        this->battery_level = std::min(this->battery_level, 100.0f);
    } else {
        this->battery_level -= this->drain_rate * d_time;
        this->battery_level = std::max(this->battery_level, 0.0f);
    }
}

ShadowPillar::ShadowPillar(int x, int y, float height, float theta, float rotation_rate, float strength) {
    this->x = x;
    this->y = y;
    this->height = height;
    this->theta = theta;
    this->rotation_rate = rotation_rate;
    this->strength = strength;
}

float ShadowPillar::get_light_level_at_time(int x, int y, float time, float sun_elev) {
    int start_x = this->x;
    int start_y = this->y;
    
    float theta = fmod(this->theta + this->rotation_rate * time, 2 * PI );
    float len = this->height / tan(sun_elev);
    
    float end_x = start_x + len * cos(theta);
    float end_y = start_y + len * sin(theta);
    
    
    // compute closest point on line segment
    float closest_x, closest_y;
    float dx = end_x - start_x;
    float dy = end_y - start_y;
    float t = ((x - start_x) * dx + (y - start_y) * dy) / (dx * dx + dy * dy);
    
    if (t < 0) {
        closest_x = start_x;
        closest_y = start_y;
    }
    else if (t > 1) {
        closest_x = end_x;
        closest_y = end_y;
    }
    else {
        closest_x = start_x + t * dx;
        closest_y = start_y + t * dy;
    }
    
    // compute distance to closest point
    float dist = sqrt(pow(x - closest_x, 2) + pow(y - closest_y, 2));
    
    float l_lvl = 1 - ( - 1 / this->strength * dist + 1);
    
    if (l_lvl < 0) {
        l_lvl = 0;
    }
    
    if (l_lvl > 1) {
        l_lvl = 1;
    }
    
    //std::cout << "start: (" << start_x << ", " << start_y << ")" << std::endl;
    //std::cout << "end: (" << end_x << ", " << end_y << ")" << std::endl;
    //std::cout << "closest: (" << closest_x << ", " << closest_y << ")" << std::endl;
    return l_lvl;
    
}

ShadowManager::ShadowManager(float sun_elev) {
    this->pillars = std::vector<ShadowPillar*>();
    this->sun_elev = sun_elev;
}

void ShadowManager::add_pillar(ShadowPillar* pillar) {
    this->pillars.push_back(pillar);
}

float ShadowManager::get_light_level_at_time(int x, int y, float time) {
    float light_level = 1;
    
    // compute light level from each pillar
    for (int i = 0; i < this->pillars.size(); i++) {
        light_level *= this->pillars[i]->get_light_level_at_time(x, y, time, this->sun_elev);
    }
    
    return light_level;
}

bool ShadowManager::export_shadows_to_file(std::string filename, float time) {
    FILE* fp = fopen(filename.c_str(), "w");
    if (fp == NULL) {
        return false;
    }

    for (int i = 0; i < this->pillars.size(); i++) {
        int start_x = this->pillars[i]->x;
        int start_y = this->pillars[i]->y;
        
        float theta = fmod(this->pillars[i]->theta + this->pillars[i]->rotation_rate * time, 2 * PI );
        float len = this->pillars[i]->height / tan(this->sun_elev);
        
        float end_x = start_x + len * cos(theta);
        float end_y = start_y + len * sin(theta);
        
        fprintf(fp, "%d, %d, %f, %f, %f\n", start_x, start_y, end_x, end_y, this->pillars[i]->strength);
    }
    
    
    fclose(fp);
    return true;
}

TimeManager::TimeManager() {
    this->time = 0;
}

float TimeManager::get_time() {
    return this->time;
}

void TimeManager::set_time(float time) {
    this->time = time;
}

void TimeManager::step_time(float d_time) {
    this->time += d_time;
}

Node::Node() {
    this->x = 0;
    this->y = 0;
    this->is_obstacle = false;
}


Node::Node(int x, int y) {
    this->x = x;
    this->y = y;
    this->is_obstacle = false;
    
}


Edge::Edge(Node* start, Node* end, float cost) {
    this->start = start;
    this->end = end;
    this->cost = cost;
    this->old_cost = cost;
}


Edge::Edge(Node* start, Node* end, float cost, float old_cost) {
    this->start = start;
    this->end = end;
    this->cost = cost;
    this->old_cost = old_cost;
}

Obstacle::Obstacle() {
    this->nodes = std::vector<Node*>();
}

Grid::Grid(int width, int height) {
    this->width = width;
    this->height = height;
    this->shadow_manager = nullptr;
    
    this->grid = new Node*[width];
    for (int i = 0; i < width; i++) {
        this->grid[i] = new Node[height];
    }
    
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            this->grid[i][j] = Node(i, j);
        }
    }
    
}

Grid::Grid(int width, int height, ShadowManager* shadow_manager) {
    this->width = width;
    this->height = height;
    this->shadow_manager = shadow_manager;
    
    this->grid = new Node*[width];
    for (int i = 0; i < width; i++) {
        this->grid[i] = new Node[height];
    }
    
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            this->grid[i][j] = Node(i, j);
        }
    }
    
}

Node* Grid::get_node(int x, int y) {
    return &this->grid[x][y];
}

bool Grid::in_bounds(int x, int y) {
    return x >= 0 && x < this->width && y >= 0 && y < this->height;
}

void Grid::obstruct(int x, int y) {
    if (this->in_bounds(x, y)) {
        this->grid[x][y].is_obstacle = true;
    }
}

void Grid::set_height(int x, int y, float height) {
    this->grid[x][y].height = height;
}

void Grid::parse_grid_heights_from_file(std::string filename) {
    FILE* fp = fopen(filename.c_str(), "r");
    if (fp == NULL) {
        std::cout << "Error opening file" << std::endl;
        return;
    }
    
    char line[256];
    //fgets(line, sizeof(line), fp); // ignore the first line
    while (fgets(line, sizeof(line), fp)) {
        int x, y; 
        float h;
        sscanf(line, "%d,%d,%f\n", &x, &y, &h);
        //std::cout << x << ", " << y << ", " << h << std::endl;
        this->set_height(x, y, h);
    }
    
    fclose(fp);  
}

bool Grid::is_obstructed(int x, int y) {
    return this->in_bounds(x, y) && this->grid[x][y].is_obstacle;
}

std::vector<Node*> Grid::pred(Node* node) {
    std::vector<Node*> nodes;
    
    // 8 neighbors
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            
            int x = node->x + i;
            int y = node->y + j;
            
            if (this->in_bounds(x, y)) {
                nodes.push_back(this->get_node(x, y));
            }
        }
    }
    
    return nodes;
    
}

std::vector<Node*> Grid::succ(Node* node) {
    std::vector<Node*> nodes;
    
    // 8 neighbors
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            
            int x = node->x + i;
            int y = node->y + j;
            
            if (this->in_bounds(x, y)) {
                nodes.push_back(this->get_node(x, y));
            }
        }
    }
    
    return nodes;
    
}

float Grid::c(Node* u, Node* v, float t, Rover* rover) {
    float cost = 0;
    
    if (this->get_node(u->x, u->y)->is_obstacle || this->get_node(v->x, v->y)->is_obstacle) {
        return std::numeric_limits<float>::infinity();
    }
    
    cost += heuristic(u, v) * WEIGHT_DIST;
    cost += abs(this->get_node(u->x, u->y)->height - this->get_node(v->x, v->y)->height) * WEIGHT_HEIGHT;
    
    if (this->shadow_manager != nullptr) {
        float light_lvl = this->shadow_manager->get_light_level_at_time(v->x, v->y, t);
        cost += (1.0 - light_lvl) * WEIGHT_SHADOW;
        if (light_lvl < .95) {
            return std::numeric_limits<float>::infinity();
        }
    }
    
    return cost;
}

std::vector<Edge*> Grid::get_changed_edges_about_node(Node* node, Grid* compare_state, int distance, Rover* rover, float t_0, float t_1) {
    std::vector<Edge*> edges;
    
    // check to see if the costs of any of the edges around the node have changed
    for (int i = -distance; i <= distance; i++) {
        for (int j = -distance; j <= distance; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            
            int x = node->x + i;
            int y = node->y + j;
            
            if (this->in_bounds(x, y)) {
                Node* n_this = this->get_node(x, y);
                Node* n_compare = compare_state->get_node(x, y);
                
                // get n_this successors
                std::vector<Node*> n_this_succ = this->succ(n_this);
                for (int k = 0; k < n_this_succ.size(); k++) {
                    Node* n_this_succ_node = n_this_succ[k];
                    Node* n_compare_succ_node = compare_state->get_node(n_this_succ_node->x, n_this_succ_node->y);
                    
                    if (this->c(n_this, n_this_succ_node, t_1, rover) != compare_state->c(n_compare, n_compare_succ_node, t_0, rover)) {
                        edges.push_back(new Edge(n_this, n_this_succ_node, this->c(n_this, n_this_succ_node, t_1, rover), compare_state->c(n_compare, n_compare_succ_node, t_0, rover)));
                    }
                }
            }
        }
    }
    
    return edges;
}

void Grid::update_grid_from_changed_edges(std::vector<Edge*> edges) {
    for (int i = 0; i < edges.size(); i++) {
        
        Edge* edge = edges[i];
        
        // update obstructions
        Node* n_s = this->get_node(edge->start->x, edge->start->y);
        Node* n_e = this->get_node(edge->end->x, edge->end->y);
        
        
        n_s->is_obstacle = edge->start->is_obstacle;
        n_e->is_obstacle = edge->end->is_obstacle;        
        
    }
    
}

bool Grid::export_obs_to_file(std::string filename) {
    FILE* fp = fopen(filename.c_str(), "w");
    if (fp == NULL) {
        return false;
    }

    // loop through all nodes in grid
    for (int i = 0; i < this->width; i++) {
        for (int j = 0; j < this->height; j++) {
            if (this->grid[i][j].is_obstacle) {
                fprintf(fp, "%d,%d\n", i, j);
            }
        }
    }
    
    
    fclose(fp);
    return true;
}

bool Grid::export_topology_to_file(std::string filename) {
    FILE* fp = fopen(filename.c_str(), "w");
    if (fp == NULL) {
        return false;
    }

    for (int i = 0; i < this->width; i++) {
        for (int j = 0; j < this->height; j++) {
            Node* n = this->get_node(i, j);
                fprintf(fp, "%d,%d,%f\n", i, j, n->height);
        }
    }

    fclose(fp);
    return true;
}

void Grid::log_grid() {
    // print last row first (y = 0 is at the bottom)
    for (int j = this->height - 1; j >= 0; j--) {
        for (int i = 0; i < this->width; i++) {
            if (this->grid[i][j].is_obstacle) {
                printf("X");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
}

Priority::Priority() {
    this->k1 = 0;
    this->k2 = 0;
}

bool Priority::operator<(const Priority& other) const {
    //std::cout << "here" << std::endl;
    return this->k1 + 0.001 < other.k1 || (this->k1 == other.k1 && this->k2 + 0.001 < other.k2 );
}

Priority::Priority(float k1, float k2) {
    this->k1 = k1;
    this->k2 = k2;
}

PriorityItem::PriorityItem(Priority priority, Node* node) {
    this->priority = priority;
    this->node = node;
}

bool PriorityItem::operator<(const PriorityItem& other) const {
    return priority.k1 > other.priority.k1 || (priority.k1 == other.priority.k1 && priority.k2 > other.priority.k2);
}

Queue::Queue() {
    this->queue = std::priority_queue<PriorityItem, std::vector<PriorityItem>>();
}

Node* Queue::top() {
    return this->queue.top().node;
}

Priority Queue::top_key() {
    //if (this->queue.empty()) {
    //    // infinity
    //    return Priority(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
    //}
    return this->queue.top().priority;
}

PriorityItem Queue::pop() {
    PriorityItem item = this->queue.top();
    this->queue.pop();
    return item;
}

void Queue::insert(PriorityItem item) {
    this->queue.push(item);
}

void Queue::update(Node* node, Priority priority) {
    //std::cout << this->queue.size() << std::endl;
    this->remove(node);
    //std::cout << this->queue.size() << std::endl;
    
    this->insert(PriorityItem(priority, node));
}

bool Queue::contains(Node* node) {
    std::priority_queue<PriorityItem, std::vector<PriorityItem>> new_queue = std::priority_queue<PriorityItem, std::vector<PriorityItem>>();
    bool contains = false;
    while (!this->queue.empty()) {
        PriorityItem item = this->queue.top();
        this->queue.pop();
        if (item.node == node) {
            contains = true;
        }
        new_queue.push(item);
    }
    this->queue = new_queue;
    return contains;
}

void Queue::remove(Node* node) {
    std::priority_queue<PriorityItem, std::vector<PriorityItem>> new_queue = std::priority_queue<PriorityItem, std::vector<PriorityItem>>();
    while (!this->queue.empty()) {
        PriorityItem item = this->queue.top();
        this->queue.pop();
        if (item.node != node) {
            new_queue.push(item);
        }
    }
    this->queue = new_queue;
}

int Queue::size() {
    return this->queue.size();
}

void Queue::clear(){
    std::priority_queue<PriorityItem, std::vector<PriorityItem>> new_queue = std::priority_queue<PriorityItem, std::vector<PriorityItem>>();
    this->queue = new_queue;
}

Path::Path() {
    this->nodes = std::vector<Node*>();
}

void Path::append(Node* node) {
    this->nodes.push_back(node);
}

bool Path::export_to_file(std::string filename) {
    FILE* fp = fopen(filename.c_str(), "w");
    if (fp == NULL) {
        return false;
    }

    // log each trajectory
    for (Node* node : this->nodes) {
        fprintf(fp, "%d, %d\n", node->x, node->y);
    }

    
    fclose(fp);
    return true;
}

DStarLite::DStarLite(Node* start, Node* goal, Grid* map, Rover* rover) {
    this->s_start = start;
    this->s_goal = goal;
    this->map = map;
    this->U = Queue();
    this->km = 0;
    this->rover = rover;
    
    this->changed_edge_costs = std::vector<Edge*>();
    
    this->s_last = start;
    
    for (int i = 0; i < this->map->width; i++) {
        for (int j = 0; j < this->map->height; j++) {
            this->map->grid[i][j].g = std::numeric_limits<float>::infinity();
            this->map->grid[i][j].rhs = std::numeric_limits<float>::infinity();
        }
    }
    
    this->s_goal->rhs = 0;
    
    Priority p = Priority(heuristic(this->s_start, this->s_goal), 0);
    PriorityItem item = PriorityItem(p, this->s_goal);
    this->U.insert(item);
}


Priority DStarLite::calculate_key(Node* s) {
    float k1 = std::min(s->g, s->rhs + heuristic(this->s_start, s)) + this->km;
    float k2 = std::min(s->g, s->rhs);
    return Priority(k1, k2);
}

void DStarLite::update_vertex(Node* u) {
    if (u->g != u->rhs && this->U.contains(u)) {
        this->U.update(u, this->calculate_key(u));
    } else if ( u->g != u->rhs && !this->U.contains(u)) {
        this->U.insert(PriorityItem(this->calculate_key(u), u));
    } else if (u->g == u->rhs && this->U.contains(u)) {
        this->U.remove(u);
    }
}

float DStarLite::c(Node* a, Node* b, float t) {

    return this->map->c(a, b, t, this->rover);
}

void DStarLite::compute_shortest_path(float t) {
    
    while (this->U.top_key() < this->calculate_key(this->s_start) || this->s_start->rhs > this->s_start->g) {
        Node* u = this->U.top();
        u = this->map->get_node(u->x, u->y);
        Priority k_old = this->U.top_key();
        Priority k_new = this->calculate_key(u);
        
        if (k_old < k_new) {
            this->U.update(u, k_new);
        } else if (u->g > u->rhs) {
            u->g = u->rhs;
            this->U.remove(u);
            std::vector<Node*> preds = this->map->pred(u);
            for (Node* s : preds) {
                if (s != this->s_goal){
                    s->rhs = std::min(s->rhs, this->c(s, u, t) + u->g);
                }
                this->update_vertex(s);
            }
        } else {
            float g_old = u->g;
            u->g = std::numeric_limits<float>::infinity();
            
            std::vector<Node*> preds = this->map->pred(u);
            preds.push_back(u);
            
            for (Node* s : preds) {
                if (s->rhs == this->c(s, u, t) + g_old) {
                    if (s != this->s_goal) {
                        
                        std::vector<Node*> succs = this->map->succ(s);
                        float min_s = std::numeric_limits<float>::infinity();
                        for (Node* s_prime : succs) {
                            float curr = this->c(s, s_prime, t) + s_prime->g;
                            if (curr < min_s) {
                                min_s = curr;
                            }
                        }
                        s->rhs = min_s;
                    } 
                }
                this->update_vertex(s);
            }
        }
    }

}

void DStarLite::queue_updated_edges(std::vector<Edge*> changed_edges) {
    for (Edge* edge : changed_edges) {
        this->changed_edge_costs.push_back(edge);
    }
}

std::vector<Edge*> DStarLite::scan_for_changes() {
    std::vector<Edge*> changed_edges = std::vector<Edge*>();
    for (Edge* edge : this->changed_edge_costs) {
        Edge* new_edge = new Edge(edge->start, edge->end, edge->cost, edge->old_cost);
        changed_edges.push_back(new_edge);
    }
    this->changed_edge_costs.clear();
    return changed_edges;
}

/* Path DStarLite::main_loop(Node* begin_loc, float t) {
    Path path = Path();
    
    //this->U.clear();
    this->s_start = this->map->get_node(begin_loc->x, begin_loc->y);
    path.append(this->s_start);
    this->s_last = this->s_start;
    this->compute_shortest_path(t);
    
    while (s_start != s_goal){
        if (s_start->rhs == std::numeric_limits<float>::infinity()) {
            std::cout << "No path found" << std::endl;
            return path;
        }
        
        std::vector<Node*> succs = this->map->succ(s_start);

        float min_s = std::numeric_limits<float>::infinity();
        Node* s_min = nullptr;
        Node* last = path.nodes.back();
        for (Node*s_prime : succs) {
            //std::cout << s_prime->x << " " << s_prime->y << ". is_obs: " << s_prime->is_obstacle << std::endl;
            float curr = this->c(s_start, s_prime, t) + s_prime->g;
            
            //bool node_in_path = false;
            //for (Node* node : path.nodes) {
            //    if (node->x == s_prime->x && node->y == s_prime->y) {
            //        node_in_path = true;
            //        std::cout << "node in path" << std::endl;
            //    }
            //}
            
            
            if (curr < min_s) {

                min_s = curr;
                s_min = s_prime;
            }
        }
        
        std::cout << "here" << std::endl;
        std::cout << s_start->x << " " << s_start->y << std::endl;

        s_start = s_min;
        //s_start = new Node(s_min->x, s_min->y);
        path.append(s_start);
        
        std::vector<Edge*> changed_edges = this->scan_for_changes();
        std::cout << "changed edges size: " << changed_edges.size() << std::endl;
        // log all changes
        for (Edge* edge : changed_edges) {
            std::cout << "start: " << edge->start->x << " " << edge->start->y << std::endl;
            std::cout << "end: " << edge->end->x << " " << edge->end->y << std::endl;
            std::cout << "old cost: " << edge->old_cost << std::endl;
            std::cout << "new cost: " << edge->cost << std::endl;
            std::cout << "----------------" << std::endl;
        }
        //this->map->log_grid();
        if (changed_edges.size() > 0) {
            //std::cin.get();
            this->map->log_grid();
            std::cout << " changes detected" << std::endl;
            this->km = this->km + heuristic(this->s_last, this->s_start);
            std::cout << "km: " << this->km << std::endl;
            this->s_last = this->s_start;
            

            for (Edge* edge : changed_edges) {
                std::cout << "updating" << std::endl;

                Node* u = this->map->get_node(edge->start->x, edge->start->y);
                Node* v = this->map->get_node(edge->end->x, edge->end->y);
                
                float c_old = edge->old_cost;
                float c_new = edge->cost;
            
                if (c_old > c_new) {
                    if (u != this->s_goal) {
                        u->rhs = std::min(u->rhs, c_new + v->g);
                    }
                } else if (u->rhs == c_old + v->g) {
                    if (u != this->s_goal) {
                        std::vector<Node*> succs = this->map->succ(u);
                        float min_s = std::numeric_limits<float>::infinity();
                        for (Node* s_prime : succs) {
                            float curr = this->c(u, s_prime, t) + s_prime->g;
                            if (curr < min_s) {
                                min_s = curr;
                            }
                        }
                        u->rhs = min_s;

                    }
                    this->update_vertex(u);
                }
                
            }
            this->compute_shortest_path(t);        
                        
        }
        this->compute_shortest_path(t);   
    }
    
    
    std::cout << "Path found" << std::endl;
    std::cout << "-----------" << std::endl;

    
    return path;
} */


Path DStarLite::main_loop(Node* begin_loc, float t) {
    Path path = Path();
    
    //this->U.clear();
    this->s_start = this->map->get_node(begin_loc->x, begin_loc->y);
    path.append(this->s_start);
    this->s_last = this->s_start;
    this->compute_shortest_path(t);
    
    while (s_start != s_goal){
        if (s_start->rhs == std::numeric_limits<float>::infinity()) {
            std::cout << "No path found" << std::endl;
            return path;
        }
        
        std::vector<Node*> succs = this->map->succ(s_start);

        float min_s = std::numeric_limits<float>::infinity();
        Node* s_min = nullptr;
        Node* last = path.nodes.back();
        for (Node*s_prime : succs) {
            //std::cout << s_prime->x << " " << s_prime->y << ". is_obs: " << s_prime->is_obstacle << std::endl;
            float curr = this->c(s_start, s_prime, t) + s_prime->g;
            
            //bool node_in_path = false;
            //for (Node* node : path.nodes) {
            //    if (node->x == s_prime->x && node->y == s_prime->y) {
            //        node_in_path = true;
            //        std::cout << "node in path" << std::endl;
            //    }
            //}
            
            
            if (curr < min_s) {
                // check to make sure we are not going back to the previous node
                /* if (s_prime->x != last->x && s_prime->y != last->y) {
                    min_s = curr;
                    s_min = s_prime;
                } */
                min_s = curr;
                s_min = s_prime;
            }
        }
        
        std::cout << "here" << std::endl;
        std::cout << s_start->x << " " << s_start->y << std::endl;

        s_start = s_min;
        //s_start = new Node(s_min->x, s_min->y);
        path.append(s_start);
        
        std::vector<Edge*> changed_edges = this->scan_for_changes();
        std::cout << "changed edges size: " << changed_edges.size() << std::endl;
        // log all changes
        for (Edge* edge : changed_edges) {
            std::cout << "start: " << edge->start->x << " " << edge->start->y << std::endl;
            std::cout << "end: " << edge->end->x << " " << edge->end->y << std::endl;
            std::cout << "old cost: " << edge->old_cost << std::endl;
            std::cout << "new cost: " << edge->cost << std::endl;
            std::cout << "----------------" << std::endl;
        }
        //this->map->log_grid();
        if (changed_edges.size() > 0) {
            //std::cin.get();
            this->map->log_grid();
            std::cout << " changes detected" << std::endl;
            this->km = this->km + heuristic(this->s_last, this->s_start);
            std::cout << "km: " << this->km << std::endl;
            this->s_last = this->s_start;
            

            for (Edge* edge : changed_edges) {
                std::cout << "updating" << std::endl;

                Node* u = this->map->get_node(edge->start->x, edge->start->y);
                Node* v = this->map->get_node(edge->end->x, edge->end->y);
                
                float c_old = edge->old_cost;
                float c_new = edge->cost;
            
                if (c_old > c_new) {
                    if (u != this->s_goal) {
                        u->rhs = std::min(u->rhs, c_new + v->g);
                    }
                } else if (u->rhs == c_old + v->g) {
                    if (u != this->s_goal) {
                        std::vector<Node*> succs = this->map->succ(u);
                        float min_s = std::numeric_limits<float>::infinity();
                        for (Node* s_prime : succs) {
                            float curr = this->c(u, s_prime, t) + s_prime->g;
                            if (curr < min_s) {
                                min_s = curr;
                            }
                        }
                        u->rhs = min_s;

                    }
                    this->update_vertex(u);
                }
                
            }
            this->compute_shortest_path(t);        
                        
        }
        this->compute_shortest_path(t);   
    }
    
    
    std::cout << "Path found" << std::endl;
    std::cout << "-----------" << std::endl;

    
    return path;
}



Obstacle* generate_random_obstacle(int x, int y, int max_side) {
    Obstacle* obs = new Obstacle();
    
    // create a random distribution of nodes with x, y as the bottom left corner
    // and max_side as the max side length
    
    int side_length = rand() % max_side + 1;
    int x_offset = rand() % (max_side - side_length + 1);
    int y_offset = rand() % (max_side - side_length + 1);
    
    for (int i = 0; i < side_length; i++) {
        for (int j = 0; j < side_length; j++) {
            Node* node = new Node(x + x_offset + i, y + y_offset + j);
            int include_node = rand() % 2;
            if (include_node == 1) {
                obs->nodes.push_back(node);
            }
            
        }
    }
    
    return obs;
}

Obstacle* expand_obstacle(Obstacle* obstacle, Rover rover) {
    Obstacle* expanded_obstacle = new Obstacle();
    
    // expand the obstacle by the rover's radius    
    for (const auto& node : obstacle->nodes) {
        int x = node->x;
        int y = node->y;
        for (int i = -rover.radius; i <= rover.radius; i++) {
            for (int j = -rover.radius; j <= rover.radius; j++) {
                if (std::sqrt(i*i + j*j) <= rover.radius && std::floor(x+i) == x+i && std::floor(y+j) == y+j) {
                    expanded_obstacle->nodes.push_back(new Node(x+i, y+j));
                }
            }
        }
    }
    
    return expanded_obstacle;
}