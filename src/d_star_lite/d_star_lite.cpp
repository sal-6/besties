#include "d_star_lite/d_star_lite.h"

Node::Node(int x, int y) {
    this->x = x;
    this->y = y;
}

Grid::Grid(int width, int height) {
    this->width = width;
    this->height = height;
}

Priority::Priority(float k1, float k2) {
    this->k1 = k1;
    this->k2 = k2;
}

PriorityItem::PriorityItem(Priority priority, Node* node) {
    this->priority = priority;
    this->node = node;
}

Queue::Queue() {
    // TODO
}

DStarLite::DStarLite(Node* start, Node* goal, Grid* map) {
    this->start = start;
    this->goal = goal;
    this->map = map;
}