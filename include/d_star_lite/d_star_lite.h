#include <queue>


class Node {
    public:
        int x;
        int y;
        
        float g;
        float rhs;
        
        Node(int x, int y);
};


class Grid {
    public:
        int width;
        int height;
        
        
        Grid(int width, int height);
        void succ();
};


class Priority {
    public:
        float k1;
        float k2;
        
        Priority(float k1, float k2);
};


class PriorityItem {
    public:
        Priority priority;
        Node* node;
        
        PriorityItem(Priority priority, Node* node);
};


class Queue {
    public:
    
        std::priority_queue<Priority> queue;
        
        Queue();
        void top();
        void top_key();
        void pop();
        void insert();
        void remove();
        
};


/*
Implementation of D* Lite according to Figure 3 of the paper: http://idm-lab.org/bib/abstracts/papers/aaai02b.pdf
*/
class DStarLite {
    public:
        Node* start;
        Node* goal;
        Grid* map;
        float km;
        Queue U;
        
        DStarLite(Node* start, Node* goal, Grid* map);
        
        void calculate_key(Node s);
        void initialize();
        void update_vertex(Node u);
        void compute_shortest_path();
        void c(Node u, Node v);
};