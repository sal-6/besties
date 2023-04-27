/*
Header file for field d* implementation
*/
#include <queue>
#include <vector>
#include <string>

#define WEIGHT_DIST 1;
#define WEIGHT_HEIGHT 1;

class Node {
    public:
        int x;
        int y;
        int height;
        
        bool visited;
        
        // this is likely temporary
        bool is_obstacle;
        
        float g;
        float rhs;
        
        Node();
        Node(int x, int y);
        
};


struct NeighborPair {
    Node* n1;
    Node* n2;
    Node* node;
    
    NeighborPair(Node* node, Node* n1, Node* n2);
};

class Priority {
    public:
        float k1;
        float k2;
        
        Priority();
        Priority(float k1, float k2);
        bool operator<(const Priority& other) const;
};


class PriorityItem {
    public:
        Priority priority;
        Node* node;
        
        PriorityItem(Priority priority, Node* node);
        
        bool operator<(const PriorityItem& other) const;
};


class Queue {
    public:
    
        std::priority_queue<PriorityItem, std::vector<PriorityItem>> queue;
        
        Queue();
        Node* top();
        Priority top_key();
        PriorityItem pop();
        void insert(PriorityItem item);
        void update(Node* node, Priority priority);
        void remove(Node* node);
        bool contains(Node* node);
        void clear();
        
        int size();
        
};

class Edge {
    public:
        Node* start;
        Node* end;
        float cost;
        float old_cost;
        
        Edge(Node* start, Node* end, float cost);
        Edge(Node* start, Node* end, float cost, float old_cost);
};

class Grid {
    public:
        int width;
        int height;
        
        Node **grid;

        Grid(int width, int height);
        
        bool in_bounds(int x, int y);
        void obstruct(int x, int y);
        void set_height(int x, int y, float height);
        bool is_obstructed(int x, int y);
        Node* get_node(int x, int y);
        std::vector<Node*> pred(Node* node);
        std::vector<Node*> succ(Node* node);
        std::vector<Node*> nbrs(Node* node);
        std::vector<NeighborPair> connbrs(Node* node);
        
        float c(Node* u, Node* v);
        
        std::vector<Edge*> get_changed_edges_about_node(Node* node, Grid* curr_state, int distance);
        void update_grid_from_changed_edges(std::vector<Edge*> changed_edges);
        bool export_obs_to_file(std::string filename);
        bool export_topology_to_file(std::string filename);

        void log_grid();
        void log_cost_map();
        
};


class Path {
    public:
        std::vector<Node*> nodes;
        
        Path();
        void append(Node* node);
        bool contains(Node* node);
        bool export_to_file(std::string filename);
};

class FieldDStar {
    public:
        Node* s_start;
        Node* s_goal;
        Grid* map;
        Queue U;
        
        Node* s_last;
        
        std::vector<Edge*> changed_edge_costs;
        
        FieldDStar(Node* start, Node* goal, Grid* map);
        
        Priority key(Node* s);
        void update_state(Node* s);
        float c(Node* u, Node* v);
        float compute_cost(Node* s, Node* sa, Node* sb);
        void compute_shortest_path();
        void queue_updated_edges(std::vector<Edge*> changed_edges);
        std::vector<Edge*> scan_for_changes();
        
        Path main(Node* begin_loc);
};

float heuristic(Node* u, Node* v);