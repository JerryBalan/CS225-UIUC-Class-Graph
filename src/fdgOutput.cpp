#include "fdgOutput.h"
#include "../structures/PNG.h"
#include <math.h>

fdgOutput::fdgOutput(Graph graph, int iterations) { 
    defineLocations(graph, iterations); 
}

fdgOutput::~fdgOutput() {}

// Find best suited locations for each node - save location values into vectors
void fdgOutput::defineLocations(Graph graph, int iterations) {
    v = graph.getVertices();
    e = graph.getEdges();
    disp.clear();
    pos.clear(); // v.pos
    disp.resize(v.size(), 0);
    pos.resize(v.size(), {0, 0});

    width = v.size();
    area = width * width;

    // Assign random positions
    for(unsigned i = 0; i < pos.size(); i++) {
        pos[i].first = std::rand() % width;
        pos[i].second = std::rand() % width;
    }

    for(unsigned i = 0; i < iterations; i++) {
        // Repulsion forces
        for(unsigned j = 0; j < v.size(); j++) {
            for(unsigned k = 0; k < v.size(); k++) {
                int d = (pos[j].first - pos[k].first) * (pos[j].first - pos[k].first);
                int d2 = (pos[j].second - pos[k].second) * (pos[j].second - pos[k].second);
                disp[j] = std::sqrt(d + d2);
            }
        }

        // Attractive forces
        for(unsigned j = 0; j < e.size(); j++) {
            int loc1 = find(v.begin(), v.end(), e[i].source) - v.begin();
            int loc2 = find(v.begin(), v.end(), e[i].dest) - v.begin();
            int d = (pos[loc1].first - pos[loc2].first) * (pos[loc1].first - pos[loc2].first);
            int d2 = (pos[loc1].second - pos[loc2].second) * (pos[loc1].second - pos[loc2].second);
            d = std::sqrt(d + d2);

            int K = std::sqrt(area / iterations);

            int moveVal = d * d / K / iterations;

            disp[loc1] -= moveVal;
            disp[loc2] += moveVal;
        }

        // Change position values based on displacement
        for(unsigned j = 0; j < v.size(); j++) {

        }

    }

    return;
}

// Uses new locations to create output PNG using cs225's PNG class
void createOutputImage() {

}

// Debugging function used to print values within class vectors
void fdgOutput::printLocations() {
    for(unsigned i = 0; i < v.size(); i++)
        std::cout << "Vertex: " << v[i] << ", displacement: " << disp[i] << ", x: " << pos[i].first << ", y: " << pos[i].second << std::endl;

    std::cout << std::endl << std::endl << std::endl << std::endl << "EDGES" << std::endl << std::endl << std::endl << std::endl;

    // for(unsigned i = 0; i < e.size(); i++)
    //     std::cout << "source: " << e[i].source << ", dest: " << e[i].dest << ", label: " << e[i].getLabel() << std::endl;

    return;
}