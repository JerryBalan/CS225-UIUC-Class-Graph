#pragma once
#include "../structures/graph.h"
#include "../structures/PNG.h"
#include <vector>
#include <iostream>
#include <string>

class fdgOutput {
  public:
    fdgOutput(Graph graph, int iterations);
    fdgOutput(Graph graph, int iterations, int classAmnt);
    ~fdgOutput();

    void defineLocations(Graph graph, int iterations, int classAmnt);
    void printLocations();
    cs225::PNG createOutputImage();

  private:
    int area, width;
    std::vector<std::pair<float, float>> disp;
    std::vector<std::pair<float, float>> pos;
    std::vector<Vertex> v;
    std::vector<Edge> e;
};