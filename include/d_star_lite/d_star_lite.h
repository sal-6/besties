#include <queue>
#include <vector>



class Node {
    public:
        int x;
        int y;
        
        // this is likely temporary
        bool is_obstacle;
        
        float g;
        float rhs;
        
        Node();
        Node(int x, int y);
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
        bool is_obstructed(int x, int y);
        Node* get_node(int x, int y);
        std::vector<Node*> pred(Node* node);
        std::vector<Node*> succ(Node* node);
        
        float c(Node* u, Node* v);
        
        std::vector<Edge*> get_changed_edges_about_node(Node* node, Grid* curr_state, int distance);
        void update_grid_from_changed_edges(std::vector<Edge*> changed_edges);
        bool export_obs_to_file(std::string filename);

        void log_grid();
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

class Path {
    public:
        std::vector<Node*> nodes;
        
        Path();
        void append(Node* node);
        bool export_to_file(std::string filename);
};

/*
Implementation of D* Lite according to Figure 3 of the paper: http://idm-lab.org/bib/abstracts/papers/aaai02b.pdf
*/
class DStarLite {
    public:
        Node* s_start;
        Node* s_goal;
        Grid* map;
        float km;
        Queue U;
        
        Node* s_last;
        
        std::vector<Edge*> changed_edge_costs;
        
        DStarLite(Node* start, Node* goal, Grid* map);
        
        Priority calculate_key(Node* s);
        void update_vertex(Node* u);
        float c(Node* u, Node* v);
        void compute_shortest_path();
        void queue_updated_edges(std::vector<Edge*> changed_edges);
        std::vector<Edge*> scan_for_changes();
        
        Path main_loop(Node* begin_loc);
};


float heuristic(Node* u, Node* v);