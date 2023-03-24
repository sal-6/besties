#include <limits>
#include <math.h>
#include <algorithm>

#include "d_star_lite/d_star_lite.h"


float heuristic(Node* a, Node* b) {
    return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}



Node::Node() {
    this->x = 0;
    this->y = 0;
}

Node::Node(int x, int y) {
    this->x = x;
    this->y = y;
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

Priority::Priority() {
    this->k1 = 0;
    this->k2 = 0;
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

DStarLite::DStarLite(Node* start, Node* goal, Grid* map) {
    this->s_start = start;
    this->s_goal = goal;
    this->map = map;
    this->U = Queue();
    this->km = 0;
    
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

void DStarLite::compute_shortest_path() {
    
}