#include <queue>
#include <vector>

#define PI 3.14159265f

// weight constants
#define WEIGHT_DIST 75;
#define WEIGHT_HEIGHT .03;
#define WEIGHT_SHADOW 0;

class TimeManager {
    public:
        float time;
    
        TimeManager();
        float get_time();
        void set_time(float time);
        void step_time(float time);
};


class Rover {
    public:
        float battery_level;
        float drain_rate;
        float charge_rate; 
        float radius;
        
        Rover(float drain_rate, float charge_rate, float radius);
        void update_battery_level(float d_time, float light_level);
};


class ShadowPillar {
    public:
        int x;
        int y;
        float height; 
        float theta; // rads
        float rotation_rate; // rads per second
        float strength;
        
        ShadowPillar(int x, int y, float height, float theta, float rotation_rate, float strength);
        float get_light_level_at_time(int x, int y, float time, float sun_elev);
};

class ShadowManager {
    public:
        std::vector<ShadowPillar*> pillars;
        float sun_elev;
        
        ShadowManager(float sun_elev);
        void add_pillar(ShadowPillar* pillar);
        float get_light_level_at_time(int x, int y, float time);
        bool export_shadows_to_file(std::string filename, float time);  
};


class Node {
    public:
        int x;
        int y;
        float height;
        
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
        ShadowManager* shadow_manager;
        
        Node **grid;

        Grid(int width, int height);
        Grid(int width, int height, ShadowManager* shadow_manager);
        
        bool in_bounds(int x, int y);
        void obstruct(int x, int y);
        void set_height(int x, int y, float height);
        void parse_grid_heights_from_file(std::string filename);
        bool is_obstructed(int x, int y);
        Node* get_node(int x, int y);
        std::vector<Node*> pred(Node* node);
        std::vector<Node*> succ(Node* node);
        
        float c(Node* u, Node* v, float t, Rover* rover);
        
        std::vector<Edge*> get_changed_edges_about_node(Node* node, Grid* curr_state, int distance, Rover* rover, float t_0, float t_1 );
        void update_grid_from_changed_edges(std::vector<Edge*> changed_edges);
        bool export_obs_to_file(std::string filename);
        bool export_topology_to_file(std::string filename);

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
        bool contains(Node* node);
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
        Rover* rover;
        
        Node* s_last;
        
        std::vector<Edge*> changed_edge_costs;
        
        DStarLite(Node* start, Node* goal, Grid* map, Rover* rover);
        
        Priority calculate_key(Node* s);
        void update_vertex(Node* u);
        float c(Node* u, Node* v, float t);
        void compute_shortest_path(float t);
        void queue_updated_edges(std::vector<Edge*> changed_edges);
        std::vector<Edge*> scan_for_changes();
        
        Path main_loop(Node* begin_loc, float t);
};


class Obstacle {
    public:
        std::vector<Node*> nodes;
        
        Obstacle();
};


float heuristic(Node* u, Node* v);
Obstacle* generate_random_obstacle(int x, int y, int max_side);
Obstacle* expand_obstacle(Obstacle* obstacle, Rover rover);