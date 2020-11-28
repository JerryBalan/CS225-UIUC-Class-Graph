/* Your code here! */
#include "disjoint.h"

DisjointSets::DisjointSets() { }

void DisjointSets::addelements(int num) {
    for(int i = 0; i < num; ++i) {
        data_.push_back(-1);
    }
}

int DisjointSets::find(int elem) {
    int curr_node = elem;
    while(data_[curr_node] > 0) {
        curr_node = data_[curr_node];
    }
    if (data_[elem] > 0) {
        data_[elem] = curr_node;
    }

    return curr_node;
}

void DisjointSets::setunion(int a , int b) {
    int size_a = size(a);
    int size_b = size(b);
    int root_a = find(a);
    int root_b = find(b);
    int new_size = (size_a + size_b) * (-1);
    if (size_a < size_b) {
        
        data_[root_a] = root_b;
        data_[root_b] = new_size;
    }
    else {
        data_[root_b] = root_a;
        data_[root_a] = new_size;
    }
}

int DisjointSets::size(int elem) {
    int root = find(elem);
    return (data_[root] * (-1));
}