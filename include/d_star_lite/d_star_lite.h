#include <queue>



class Node {
    public:
        int x;
        int y;
        
        float g;
        float rhs;
        
        Node();
        Node(int x, int y);
};


class Grid {
    public:
        int width;
        int height;
        
        Node **grid;
        
        Grid(int width, int height);
        Node* get_node(int x, int y);
        void succ();
};


class Priority {
    public:
        float k1;
        float k2;
        
        Priority();
        Priority(float k1, float k2);
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
        
        int size();
        
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
        
        DStarLite(Node* start, Node* goal, Grid* map);
        
        Priority calculate_key(Node* s);
        void update_vertex(Node* u);
        void compute_shortest_path();
        void c(Node u, Node v);
};


float heuristic(Node* u, Node* v);