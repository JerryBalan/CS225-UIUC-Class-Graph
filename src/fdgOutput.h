#pragma once
#include "../structures/graph.h"
#include <vector>
#include <iostream>
#include <string>

class fdgOutput {
  public:
    fdgOutput(Graph graph, int iterations);
    ~fdgOutput();

    void defineLocations(Graph graph, int iterations);
    void printLocations();
    void createOutputImage();

  private:
    int area, width;
    std::vector<int> disp;
    std::vector<std::pair<int, int>> pos;
    std::vector<Vertex> v;
    std::vector<Edge> e;
};