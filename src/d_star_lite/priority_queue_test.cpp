#include <iostream>
#include <queue>

#include "d_star_lite/d_star_lite.h"


int main() {
    
    Queue U = Queue();
    
    Node* n1 = new Node(1, 1);
    Priority p1 = Priority(1, 1);
    
    Node* n2 = new Node(2, 2);
    Priority p2 = Priority(2, 2);
    
    PriorityItem pi1 = PriorityItem(p1, n1);
    PriorityItem pi2 = PriorityItem(p2, n2);
    
    U.insert(pi1);
    U.insert(pi2);
    
    std::cout << U.size() << std::endl;
    
    Node* n3 = U.top();
    
    std::cout << n3->x << ", " << n3->y << std::endl;
    
    std::cout << (pi2 < pi1) << std::endl;
    return 0;
}