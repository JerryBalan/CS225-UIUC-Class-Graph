#include "fdgOutput.h"
#include "../structures/PNG.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <mutex>
//#include "fdgHelper.h"


#define NUM_THREADS 2
std::mutex mtx;

fdgOutput::fdgOutput(Graph graph, unsigned iterations) {
    defineLocations(graph, 5, iterations, graph.getVertices().size()); 
}

fdgOutput::fdgOutput(Graph graph, int scale, unsigned iterations, int classAmnt) {
    defineLocations(graph, scale, iterations, classAmnt); 
}

fdgOutput::~fdgOutput() {}

// Find best suited locations for each node - save location values into vectors
// void fdgOutput::defineLocations(Graph graph, int scale, unsigned iterations, int classAmnt) {
//     v = graph.getVertices();
//     e = graph.getEdges();

//     classAmnt = classAmnt > v.size() ? v.size() : classAmnt;

//     v.resize(classAmnt);

//     disp.clear();
//     pos.clear();
//     disp.resize(v.size(), {0, 0});
//     pos.resize(v.size(), {0, 0});

//     width = v.size() * scale;
//     area = width * width;
//     float t = area, K = std::sqrt(area / v.size());

//     if(iterations < 0 || iterations > v.size())
//         iterations = v.size();

//     // Assign random positions
//     for(unsigned i = 0; i < pos.size(); i++) {
//         pos[i].first = std::rand() % width;
//         pos[i].second = std::rand() % width;
//     }

//     for(unsigned i = 0; i < iterations; i++) {
//         // Repulsion forces
//         for(unsigned j = 0; j < v.size(); j++) {
//             disp[j] = {0, 0};

//             for(unsigned k = 0; k < v.size(); k++) {
//                 float d = pos[j].first - pos[k].first;
//                 float d2 = pos[j].second - pos[k].second;
//                 float dist = std::max((float)0.001, std::sqrt(d * d + d2 * d2));
//                 float aForce = K * K / dist / v.size() / 100.0;
//                 disp[j].first += d / dist * aForce;
//                 disp[j].second += d2 / dist * aForce;
//             }
//         }

//         // Attractive forces
//         for(unsigned j = 0; j < e.size(); j++) {
//             auto temp1 = find(v.begin(), v.end(), e[j].source);
//             auto temp2 = find(v.begin(), v.end(), e[j].dest);
//             if(temp1 == v.end() || temp2 == v.end())
//                 continue;
//             float loc1 = temp1 - v.begin();
//             float loc2 = temp2 - v.begin();

//             float x = pos[i].first - pos[loc1].first;
//             float y = pos[i].second - pos[loc2].second;

//             float dist = std::max((float)0.001, std::sqrt(x * x + y * y));
//             float aForce = dist * dist / K / v.size();

//             disp[i].first -= x / dist * aForce;
//             disp[i].second += y / dist * aForce;
//         }

//         // Change position values based on displacement
//         for(unsigned j = 0; j < v.size(); j++) {
//             float d = std::sqrt(disp[j].first * disp[j].first + disp[j].second * disp[j].second);
//             pos[j].first += d > t ? disp[j].first / d * t : disp[j].first;
//             pos[j].second += d > t ? disp[j].second / d * t : disp[j].second;
//             // pos[j].first = (int)pos[j].first % width;
//             // pos[j].second = (int)pos[j].second % width;

//             // // pos[j].first = std::min((float)width - 1, pos[j].first);
//             // // pos[j].second = std::min((float)width - 1, pos[j].second);

//             // while(pos[j].first > width)
//             //     pos[j].first = (int)pos[j].first % width;
//             // while(pos[j].second > width)
//             //     pos[j].second = (int)pos[j].second % width;
//             // pos[j].first = std::max((float)1, pos[j].first);
//             // pos[j].second = std::max((float)1, pos[j].second);

//             pos[j].first     fdgOutput(Graph graph, unsigned iterations);
//             if((pos[j].first == 0 || pos[j].second == 0) || (pos[j].first == width || pos[j].second == width)) {
//                 pos[i].first = std::rand() % width;
//                 pos[i].second = std::rand() % width;
//             }
//         }

//         t *= 0.99;
//     }

//     return;
// }


void fdgOutput::attractiveFunc2(int i) {
    // Attractive forces
    //int i = *(int *)arg;
    float K = std::sqrt(area / v.size());
    for(unsigned j = 0; j < e.size(); j++) {
        auto temp1 = find(v.begin(), v.end(), e[j].source);
        auto temp2 = find(v.begin(), v.end(), e[j].dest);
        if(temp1 == v.end() || temp2 == v.end())
            continue;
        float loc1 = temp1 - v.begin();
        float loc2 = temp2 - v.begin();

        float x = pos[i].first - pos[loc1].first;
        float y = pos[i].second - pos[loc2].second;

        float dist = std::max((float)0.001, std::sqrt(x * x + y * y));
        float aForce = dist * dist / K / v.size();
        mtx.lock();
        disp[i].first -= x / dist * aForce;
        disp[i].second += y / dist * aForce;
        mtx.unlock();
    }
}

void fdgOutput::repulsionFunc2(int i) {
    // Repulsion forces
    float K = std::sqrt(area / v.size());
    for(unsigned j = 0; j < v.size(); j++) {
        disp[j] = {0, 0};

        for(unsigned k = 0; k < v.size(); k++) {
            float d = pos[j].first - pos[k].first;
            float d2 = pos[j].second - pos[k].second;
            float dist = std::max((float)0.001, std::sqrt(d * d + d2 * d2));
            float aForce = K * K / dist / v.size() / 100.0;
            mtx.lock();
            disp[j].first += d / dist * aForce;
            disp[j].second += d2 / dist * aForce;
            mtx.unlock();
        }
    }
}

void fdgOutput::defineLocations(Graph graph, int scale, unsigned iterations, int classAmnt) {


    v = graph.getVertices();
    e = graph.getEdges();

    classAmnt = classAmnt > v.size() ? v.size() : classAmnt;

    v.resize(classAmnt);

    disp.clear();
    pos.clear();
    disp.resize(v.size(), {0, 0});
    pos.resize(v.size(), {0, 0});

    width = v.size() * scale;
    area = width * width;
    float t = area, K = std::sqrt(area / v.size());

    if(iterations < 0 || iterations > v.size())
        iterations = v.size();

    // Assign random positions
    for(unsigned i = 0; i < pos.size(); i++) {
        pos[i].first = std::rand() % width;
        pos[i].second = std::rand() % width;
    }

    //fdgHelper * fPtr = new fdgHelper();

    for(unsigned i = 0; i < iterations; i++) {
        // pthread_t threads[NUM_THREADS];
        // int val[NUM_THREADS];
        // pthread_create(&threads[0], NULL, repulsionFunc, NULL);
        // pthread_create(&threads[1], NULL, attractiveFunc, &i);
        // void *(*start_routine) (void *),

        // pthread_join(threads[0], NULL);
        // pthread_join(threads[1], NULL);

        // std::thread th1(&fdgHelper::attractiveFunc, fPtr, i, e, v, disp, pos, area);
        // std::thread th2(&fdgHelper::repulsionFunc, fPtr, v, disp, pos, area);
        std::thread th1(&fdgOutput::attractiveFunc2, this, i);
        std::thread th2(&fdgOutput::repulsionFunc2, this, i);

        th1.join();
        th2.join();

        // Change position values based on displacement
        for(unsigned j = 0; j < v.size(); j++) {
            float d = std::sqrt(disp[j].first * disp[j].first + disp[j].second * disp[j].second);
            pos[j].first += d > t ? disp[j].first / d * t : disp[j].first;
            pos[j].second += d > t ? disp[j].second / d * t : disp[j].second;
            // pos[j].first = (int)pos[j].first % width;
            // pos[j].second = (int)pos[j].second % width;

            // // pos[j].first = std::min((float)width - 1, pos[j].first);
            // // pos[j].second = std::min((float)width - 1, pos[j].second);

            // while(pos[j].first > width)
            //     pos[j].first = (int)pos[j].first % width;
            // while(pos[j].second > width)
            //     pos[j].second = (int)pos[j].second % width;
            // pos[j].first = std::max((float)1, pos[j].first);
            // pos[j].second = std::max((float)1, pos[j].second);

            pos[j].first = std::min((float)width, std::max((float)0, pos[j].first));
            pos[j].second = std::min((float)width, std::max((float)0, pos[j].second));

            if((pos[j].first == 0 || pos[j].second == 0) || (pos[j].first == width || pos[j].second == width)) {
                pos[i].first = std::rand() % width;
                pos[i].second = std::rand() % width;
            }
        }

        t *= 0.99;
    }

    return;
}


// Uses new locations to create output PNG using cs225's PNG class
cs225::PNG fdgOutput::createOutputImage() {
    cs225::PNG out(width + 2, width + 2);

    // Draw verticies
    for(unsigned i = 0; i < v.size(); i++) {
        // Square of size 3x3 instead of single pixel
        cs225::HSLAPixel curr = getRandColor();
        colors.push_back({curr.h, curr.s, curr.l});
        for(int j = -1; j < 2; j++) {
            for(int k = -1; k < 2; k++) {
                int x = std::max(0, (int)pos[i].first + j);
                int y = std::max(0, (int)pos[i].second + k);
                //out.getPixel(x, y).l = 0;
                out.getPixel(x, y) = curr;
            }
        }

        //out->getPixel(pos[i].first, pos[i].second).l = 0;
    }

    // Draw edges
    for(unsigned i = 0; i < e.size(); i++) {
        auto temp1 = find(v.begin(), v.end(), e[i].source);
        auto temp2 = find(v.begin(), v.end(), e[i].dest);
        if(temp1 == v.end() || temp2 == v.end())
            continue;
        std::pair<float, float> pt1 = pos[temp1 - v.begin()];
        std::pair<float, float> pt2 = pos[temp2 - v.begin()];

        float slope = (pt1.second - pt2.second) / (pt1.first - pt2.first);
        float yIntercept = pt1.second - (slope * pt1.first);

        int end = std::max(pt1.first, pt2.first);
        for(int j = std::min(pt1.first, pt2.first); j < end; j++) {
            int y = (slope * j) + yIntercept;
            if(j < 0 || j > width + 2 || y < 0 || y > width + 2)
                continue;
            out.getPixel(j, y).l = 0.6;
            //out.getPixel(j, y) = getRandColor();
        }
    }

    return out;
}

cs225::HSLAPixel fdgOutput::getRandColor() {
    double f = (double)rand() / RAND_MAX;
    double f2 = (double)rand() / RAND_MAX;
    double f3 = (double)rand() / RAND_MAX;
    cs225::HSLAPixel out(f, f2, f3);

    return out;
}

// Debugging function used to print values within class vectors
void fdgOutput::printLocations() {
    std::ofstream outFile;
    outFile.open("outputImgInfo.txt");
    for(unsigned i = 0; i < v.size(); i++)
        outFile << "Vertex: " << v[i] << ", location: (" << pos[i].first << ", " << pos[i].second << ")" << ", color (HSL): (" << std::get<0>(colors[i]) << ", " << std::get<1>(colors[i]) << ", " << std::get<2>(colors[i]) << ")\n";

    outFile << "width: " << width << " (size : " << area << ")\n";
    outFile << "vertex amount: " << v.size() << ", edge amount: " << e.size() << "\n";

    std::cout << "Wrote info to outputImgInfo.txt" << std::endl;

    return;
}