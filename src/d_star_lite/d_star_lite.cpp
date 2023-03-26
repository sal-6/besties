#include <limits>
#include <math.h>
#include <algorithm>
#include <vector>
#include <iostream>

#include "d_star_lite/d_star_lite.h"


float heuristic(Node* a, Node* b) {
    return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
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

Grid::Grid(int width, int height) {
    this->width = width;
    this->height = height;
    
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
    this->grid[x][y].is_obstacle = true;
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

float Grid::c(Node* u, Node* v) {
    if (this->get_node(u->x, u->y)->is_obstacle || this->get_node(v->x, v->y)->is_obstacle) {
        return std::numeric_limits<float>::infinity();
    }
    
    return heuristic(u, v);
}

std::vector<Edge*> Grid::get_changed_edges_about_node(Node* node, Grid* compare_state, int distance) {
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
                    
                    if (this->c(n_this, n_this_succ_node) != compare_state->c(n_compare, n_compare_succ_node)) {
                        edges.push_back(new Edge(n_this, n_this_succ_node, this->c(n_this, n_this_succ_node), compare_state->c(n_compare, n_compare_succ_node)));
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
    return this->k1 < other.k1 || (this->k1 == other.k1 && this->k2 < other.k2);
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
    this->remove(node);
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

DStarLite::DStarLite(Node* start, Node* goal, Grid* map) {
    this->s_start = start;
    this->s_goal = goal;
    this->map = map;
    this->U = Queue();
    this->km = 0;
    
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

float DStarLite::c(Node* a, Node* b) {

    return this->map->c(a, b);
}

void DStarLite::compute_shortest_path() {
    
    while (this->U.top_key() < this->calculate_key(this->s_start) || this->s_start->rhs > this->s_start->g) {
        Node* u = this->U.top();
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
                    s->rhs = std::min(s->rhs, this->c(s, u) + u->g);
                }
                this->update_vertex(s);
            }
        } else {
            float g_old = u->g;
            u->g = std::numeric_limits<float>::infinity();
            std::vector<Node*> preds = this->map->pred(u);
            preds.push_back(u);
            for (Node* s : preds) {
                if (s->rhs == this->c(s, u) + g_old) {
                    if (s != this->s_goal) {
                        
                        std::vector<Node*> succs = this->map->succ(s);
                        float min_s = std::numeric_limits<float>::infinity();
                        for (Node* s_prime : succs) {
                            float curr = this->c(s, s_prime) + s_prime->g;
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

Path DStarLite::main_loop(Node* begin_loc) {
    Path path = Path();
    
    this->s_start = begin_loc;
    path.append(this->s_start);
    this->s_last = this->s_start;
    this->compute_shortest_path();
    
    while (s_start != s_goal){
        if (s_start->rhs == std::numeric_limits<float>::infinity()) {
            std::cout << "No path found" << std::endl;
            return path;
        }
        
        std::vector<Node*> succs = this->map->succ(s_start);
        float min_s = std::numeric_limits<float>::infinity();
        Node* s_min = nullptr;
        for (Node*s_prime : succs) {
            float curr = this->c(s_start, s_prime) + s_prime->g;
            if (curr < min_s) {
                min_s = curr;
                s_min = s_prime;
            }
        }
        
        std::cout << "here" << std::endl;
        std::cout << s_start->x << " " << s_start->y << std::endl;

        s_start = s_min;
        path.append(s_start);
        
        // TODO: scan for changes
        std::vector<Edge*> changed_edges = this->scan_for_changes();
        if (changed_edges.size() > 0) {
            this->map->log_grid();
            std::cout << " changes detected" << std::endl;
            this->km += heuristic(this->s_last, this->s_start);
            this->s_last = this->s_start;
            
            for (Edge* edge : changed_edges) {
                std::cout << "updating" << std::endl;

                
                Node* u = this->map->get_node(edge->start->x, edge->start->y);
                Node* v = this->map->get_node(edge->end->x, edge->end->y);
                
                float c_old = edge->old_cost;
                float c_new = this->map->c(u, v);
                
                if (c_old > c_new) {
                    if (u != this->s_goal) {
                        u->rhs = std::min(u->rhs, c_new + v->g);
                    }
                } else if (u->rhs == c_old + v->g) {
                    if (u != this->s_goal) {
                        std::vector<Node*> succs = this->map->succ(u);
                        float min_s = std::numeric_limits<float>::infinity();
                        for (Node* s_prime : succs) {
                            float curr = this->c(u, s_prime) + s_prime->g;
                            if (curr < min_s) {
                                min_s = curr;
                            }
                        }
                        u->rhs = min_s;
                    }
                    this->update_vertex(u);
                }
                
            }
            
        }
        this->compute_shortest_path();        
    }
    
    
    std::cout << "Path found" << std::endl;
    std::cout << "-----------" << std::endl;

    
    return path;
}