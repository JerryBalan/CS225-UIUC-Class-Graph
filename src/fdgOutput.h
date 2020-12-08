#pragma once
#include "structures/graph.h"
#include "structures/PNG.h"
#include "structures/HSLAPixel.h"
#include <vector>
#include <iostream>
#include <string>
#include <tuple>
#include <thread>

class fdgOutput {
  public:
    fdgOutput(int version, Graph graph, unsigned iterations);
    fdgOutput(int version, Graph graph, int scale, unsigned iterations, int classAmnt);
    ~fdgOutput();

    void defineLocationsSerial(Graph graph, int scale, unsigned iterations, int classAmnt);
    void defineLocationsParallel(Graph graph, int scale, unsigned iterations, int classAmnt);
    void printLocations();

    cs225::PNG createOutputImage();

  private:
    int area, width;
    std::vector<std::pair<float, float>> disp;
    std::vector<std::pair<float, float>> pos;
    std::vector<std::tuple<double, double, double>> colors;
    std::vector<Vertex> v;
    std::vector<Edge> e;

    void setVariables(Graph graph, int classAmnt, int scale);
    void attractiveFunc(int i);
    void repulsionFunc(int i);
    cs225::HSLAPixel getRandColor();
};