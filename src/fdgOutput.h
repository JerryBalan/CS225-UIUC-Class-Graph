#pragma once
#include "../structures/graph.h"
#include "../structures/PNG.h"
#include "../structures/HSLAPixel.h"
#include <vector>
#include <iostream>
#include <string>
#include <tuple>
//#include <pthread.h>
#include <thread>

class fdgOutput {
  public:
    fdgOutput(Graph graph, unsigned iterations);
    fdgOutput(Graph graph, int scale, unsigned iterations, int classAmnt);
    ~fdgOutput();

    void defineLocations(Graph graph, int scale, unsigned iterations, int classAmnt);
    void printLocations();
    cs225::HSLAPixel getRandColor();
    cs225::PNG createOutputImage();

    void attractiveFunc2(int i);
    void repulsionFunc2(int i);

  private:
    int area, width;
    std::vector<std::pair<float, float>> disp;
    std::vector<std::pair<float, float>> pos;
    std::vector<std::tuple<double, double, double>> colors;
    std::vector<Vertex> v;
    std::vector<Edge> e;


};