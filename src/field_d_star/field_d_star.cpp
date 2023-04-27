/*
Implementations for the associated header file
*/

#include "field_d_star/field_d_star.h"

#include <vector>
#include <math.h>
#include <limits>
#include <algorithm>
#include <iostream>


float heuristic(Node* a, Node* b) {
    return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}

Node::Node() {
    this->x = 0;
    this->y = 0;
    this->is_obstacle = false;
    this->visited = false;
}


Node::Node(int x, int y) {
    this->x = x;
    this->y = y;
    this->is_obstacle = false;
    this->visited = false;
}

NeighborPair::NeighborPair(Node* node, Node* n1, Node* n2) {
    this->node = node;
    this->n1 = n1;
    this->n2 = n2;
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

void Grid::set_height(int x, int y, float height) {
    this->grid[x][y].height = height;
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

std::vector<Node*> Grid::nbrs(Node* node){
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

std::vector<NeighborPair> Grid::connbrs(Node* node){
    std::vector<Node*> nodes;
    
    // get 8 sets of consecutive neighbors in counter-clockwise order
    // 4 3 2
    // 5   1
    // 6 7 8
    
    std::vector<NeighborPair> nbr_pairs;
    
    std::vector<std::pair<int, int>> nbrs;
    nbrs.push_back(std::make_pair(node->x + 1, node->y));
    nbrs.push_back(std::make_pair(node->x + 1, node->y + 1));
    nbrs.push_back(std::make_pair(node->x, node->y + 1));
    nbrs.push_back(std::make_pair(node->x - 1, node->y + 1));
    nbrs.push_back(std::make_pair(node->x - 1, node->y));
    nbrs.push_back(std::make_pair(node->x - 1, node->y - 1));
    nbrs.push_back(std::make_pair(node->x, node->y - 1));
    nbrs.push_back(std::make_pair(node->x + 1, node->y - 1));
    nbrs.push_back(std::make_pair(node->x + 1, node->y));
    
    for (int i = 0; i < nbrs.size() - 1; i++) {
        std::pair<int, int> nbr1 = nbrs[i];
        std::pair<int, int> nbr2 = nbrs[i + 1];
        
        if (this->in_bounds(nbr1.first, nbr1.second) && this->in_bounds(nbr2.first, nbr2.second)) {
            nbr_pairs.push_back(NeighborPair(node, this->get_node(nbr1.first, nbr1.second), this->get_node(nbr2.first, nbr2.second)));
        }
    }
    
    
    return nbr_pairs;
}




float Grid::c(Node* u, Node* v) {
    float cost = 0;
    
    if (this->get_node(u->x, u->y)->is_obstacle || this->get_node(v->x, v->y)->is_obstacle) {
        return std::numeric_limits<float>::infinity();
    }
    
    cost += heuristic(u, v) * WEIGHT_DIST;
    cost += abs(this->get_node(u->x, u->y)->height - this->get_node(v->x, v->y)->height) * WEIGHT_HEIGHT;
    
    return cost;
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

bool Grid::export_topology_to_file(std::string filename) {
    FILE* fp = fopen(filename.c_str(), "w");
    if (fp == NULL) {
        return false;
    }

    for (int i = 0; i < this->width; i++) {
        for (int j = 0; j < this->height; j++) {
            Node* n = this->get_node(i, j);
                fprintf(fp, "%d,%d,%d\n", i, j, n->height);
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

void Grid::log_cost_map() {
    // print last row first (y = 0 is at the bottom)
    for (int j = this->height - 1; j >= 0; j--) {
        for (int i = 0; i < this->width; i++) {
            printf("%f ", this->grid[i][j].g);
        }
        printf("\n");
    }
}

Priority FieldDStar::key(Node* s) {
    float k1 = std::min(s->g, s->rhs) + heuristic(this->s_start, s);
    float k2 = std::min(s->g, s->rhs);
    
    return Priority(k1, k2);
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


FieldDStar::FieldDStar(Node* start, Node* goal, Grid* map) {
    this->s_start = start;
    this->s_goal = goal;
    this->map = map;
    this->U = Queue();

    
    this->changed_edge_costs = std::vector<Edge*>();
    
    this->s_last = start;
    
    //for (int i = 0; i < this->map->width; i++) {
    //    for (int j = 0; j < this->map->height; j++) {
    //        this->map->grid[i][j].g = std::numeric_limits<float>::infinity();
    //        this->map->grid[i][j].rhs = std::numeric_limits<float>::infinity();
    //    }
    //}
    
    this->map->grid[start->x][start->y].g = std::numeric_limits<float>::infinity();
    this->map->grid[start->x][start->y].rhs = std::numeric_limits<float>::infinity();
    
    this->map->grid[goal->x][goal->y].g = std::numeric_limits<float>::infinity();
    this->s_goal->rhs = 0;
    
    Priority p = this->key(this->s_goal);
    PriorityItem item = PriorityItem(p, &this->map->grid[goal->x][goal->y]);
    this->U.insert(item);
}


void FieldDStar::update_state(Node* s){
    // TODO: figure out how to handle not visited b4
    if (!s->visited){
        s->visited = true;
        s->g = std::numeric_limits<float>::infinity();
    }
    
    if (s->x != this->s_goal->x || s->y != this->s_goal->y) { // s is not goal
        float min_c = std::numeric_limits<float>::infinity();
        std::vector<NeighborPair> nbr_pairs = this->map->connbrs(s);
        for (int i = 0; i < nbr_pairs.size(); i++) {
            NeighborPair nbr_pair = nbr_pairs[i];
            Node* s_prime = nbr_pair.n1;
            Node* s_prime_prime = nbr_pair.n2;
            float c = this->compute_cost(s, s_prime, s_prime_prime);
            if (c < min_c) {
                min_c = c;
            }
        }
        s->rhs = min_c;
    }
    if (this->U.contains(s)) {
        this->U.remove(s);
    }
    if (s->g != s->rhs) {
        Priority p = this->key(s);
        PriorityItem item = PriorityItem(p, s);
        this->U.insert(item);
    }
}

void FieldDStar::compute_shortest_path() {
    while (this->U.top_key() < this->key(this->s_start) || this->s_start->rhs != this->s_start->g){
        PriorityItem item = this->U.pop();
        Node* s = item.node;
        if (s->g > s->rhs){
            s->g = s->rhs;
            std::vector<Node*> neighbors = this->map->nbrs(s);
            for (int i = 0; i < neighbors.size(); i++) {
                Node* s_prime = neighbors[i];
                this->update_state(s_prime);
            }
        } else {
            s->g = std::numeric_limits<float>::infinity();
            std::vector<Node*> neighbors = this->map->nbrs(s);
            for (int i = 0; i < neighbors.size(); i++) {
                Node* s_prime = neighbors[i];
                this->update_state(s_prime);
            }
            this->update_state(s);
        }
        
    }
}


float FieldDStar::c(Node* a, Node* b) {

    return this->map->c(a, b);
}

float FieldDStar::compute_cost(Node* s, Node* sa, Node* sb) {
    // diagonal check
    Node* s1;
    Node* s2;
    if (s->x != sa->x && s->y != sa->y) {
        s1 = sb;
        s2 = sa;
    } else {
        s1 = sa;
        s2 = sb;
    }
    
    float v_s;
    
    // TODO: calculate c and b
    float c = this->c(s, s2);
    float b = this->c(s, s1);
    
    if (std::min(c, b) == std::numeric_limits<float>::infinity()) {
        v_s = std::numeric_limits<float>::infinity();
    } else if (s1->g <= s2->g) {
        v_s = std::min(c, b) + s1->g;
    }
    else {
        float f = s1->g - s2->g;
        if (f <= b) {
            if (c <= f) {
                v_s = c * sqrt(2) + s2->g;
            } else {
                float val = f / (sqrt(pow(c, 2) - pow(f, 2)));
                float y = std::min(val, 1.0f);
                v_s = c * sqrt(1 + pow(y, 2)) + f * (1 - y) + s2->g;
            }
        } else {
            if (c <= b) {
                v_s = c* sqrt(2) + s2->g;
            } else {
                float val = b / (sqrt(pow(c, 2) - pow(b, 2)));
                float x = 1 - std::min(val, 1.0f);
                v_s = c * sqrt(1 + pow(1 - x, 2)) + b * x + s2->g;
            }
        }
    }
    
    return v_s;
}

Path FieldDStar::main(Node* begin) {
    Path path = Path();
    path.append(begin);
    
    
    while (true) {
        this->compute_shortest_path();
        
        if (this->s_start->rhs == std::numeric_limits<float>::infinity()) {
            return path;
        }
        
        std::vector<Node*> succs = this->map->succ(s_start);

        float min_s = std::numeric_limits<float>::infinity();
        Node* s_min = nullptr;
        for (Node*s_prime : succs) {
            //std::cout << s_prime->x << " " << s_prime->y << ". is_obs: " << s_prime->is_obstacle << std::endl;
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
        
        if (s_start->x == this->s_goal->x && s_start->y == this->s_goal->y) {
            return path;
        }
    }
    
}