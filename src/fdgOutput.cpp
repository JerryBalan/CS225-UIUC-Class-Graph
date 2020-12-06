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
    disp.resize(v.size(), {0, 0});
    pos.resize(v.size(), {0, 0});

    width = v.size() * 5;
    area = width * width;
    float thr = area;

    // Assign random positions
    for(unsigned i = 0; i < pos.size(); i++) {
        pos[i].first = std::rand() % width;
        pos[i].second = std::rand() % width;
    }

    for(unsigned i = 0; i < iterations; i++) {
        // Repulsion forces
        for(unsigned j = 0; j < v.size(); j++) {
            for(unsigned k = 0; k < v.size(); k++) {
                float d = (pos[j].first - pos[k].first);
                float d2 = (pos[j].second - pos[k].second);
                disp[j] = {d, d2};
            }
        }

        // Attractive forces
        for(unsigned j = 0; j < e.size(); j++) {
            // int loc1 = find(v.begin(), v.end(), e[j].source) - v.begin();
            // int loc2 = find(v.begin(), v.end(), e[j].dest) - v.begin();
            // int d = (pos[loc1].first - pos[loc2].first) * (pos[loc1].first - pos[loc2].first);
            // int d2 = (pos[loc1].second - pos[loc2].second) * (pos[loc1].second - pos[loc2].second);
            // d = std::sqrt(d + d2);

            // int K = std::sqrt(area / iterations);

            // int moveVal = d * d / K / iterations;

            // disp[loc1].first -= moveVal;
            // disp[loc2].second += moveVal;

            float loc1 = find(v.begin(), v.end(), e[j].source) - v.begin();
            float loc2 = find(v.begin(), v.end(), e[j].dest) - v.begin();

            float x = pos[loc1].first - pos[loc2].first;
            float y = pos[loc1].second - pos[loc2].second;

            float dist = std::max(std::sqrt(x * x + y * y), (float)0.001);
            float K = std::sqrt(area / iterations);
            float aForce = dist * dist / K / iterations;

            disp[loc1].first -= x / dist * aForce;
            disp[loc1].second += y / dist * aForce;
        }

        // Change position values based on displacement
        // for(unsigned j = 0; j < v.size(); j++) {
        //     float d = std::sqrt(disp[j].first * disp[j].first + disp[j].second * disp[j].second);
        //     pos[j].first += d > thr ? pos[j].first / d * thr : pos[j].first;
        //     pos[j].second += d > thr ? pos[j].second / d * thr : pos[j].second;
        //     pos[j].first = std::min((float)width / 2, std::max(-(float)width / 2, pos[j].first));
        //     pos[j].second = std::min((float)width / 2, std::max(-(float)width / 2, pos[j].second));
        //     disp[j] = {0, 0};
        // }

        // thr *= 0.99;
    }

    for(unsigned j = 0; j < v.size(); j++) {
        pos[j].first += disp[j].first;
        pos[j].second += disp[j].second;

        pos[j].first = std::max((float)0, pos[j].first);
        pos[j].second = std::max((float)0, pos[j].second);
        pos[j].first = std::min((float)width, pos[j].first);
        pos[j].second = std::min((float)width, pos[j].second);
    }

    return;
}

// Uses new locations to create output PNG using cs225's PNG class
cs225::PNG * fdgOutput::createOutputImage() {
    cs225::PNG * out = new cs225::PNG(width + 2, width + 2);

    for(int i = 0; i < width + 2; i++) {
        for(int j = 0; j < width + 2; j++)
            out->getPixel(i, j).l = 1;
    }

    for(int i = 0; i < v.size(); i++) {
        // Square of size 3x3 instead of single pixel
        for(int j = -1; j < 2; j++) {
            for(int k = -1; k < 2; k++)
                out->getPixel(pos[i].first + j, pos[i].second + k).l = 0;
        }

        //out->getPixel(pos[i].first, pos[i].second).l = 0;
    }

    return out;
}

// Debugging function used to print values within class vectors
void fdgOutput::printLocations() {
    for(unsigned i = 0; i < v.size(); i++)
        std::cout << "Vertex: " << v[i] << ", displacement: (" << disp[i].first << ", " << disp[i].second << "), loc: (" << pos[i].first << ", " << pos[i].second << ")" << std::endl;
    
    
    // for(unsigned i = 0; i < e.size(); i++)
    //     std::cout << "source: " << e[i].source << ", dest: " << e[i].dest << ", label: " << e[i].getLabel() << std::endl;

    std::cout << "width: " << width << " (size : " << area << ")" << std::endl;

    return;
}