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
    fdgOutput(int version, Graph graph, int sideSpace, std::unordered_map<std::string, double> subjectFrequencies);
    ~fdgOutput();

    void defineLocationsSerial(Graph graph, std::unordered_map<std::string, double> &subjectFrequencies, int sideSpace);
    void defineLocationsParallel(Graph graph, std::unordered_map<std::string, double> &subjectFrequencies, int sideSpace);
    void printLocations();

    cs225::PNG createOutputImage(std::unordered_map<std::string, double> subjectFrequencies);

  private:
    int area, width, height;
    //std::vector<std::pair<float, float>> disp;
    std::vector<std::pair<double, double>> pos;
    std::vector<std::pair<double, double>> forces;
    std::vector<std::tuple<double, double, double>> colors;
    std::vector<Vertex> v;
    std::vector<Edge> e;

    std::string getCourseSubject(std::string course);
    double fRand(double fMin, double fMax);

    void setVariables(Graph graph, int scale, std::unordered_map<std::string, double> &subjectFrequencies, bool setCompletlyRandom);
    void recenterPts(int sideSpace);
    void springFunc(int springConstant, int springRestLength);
    void repulsionFunc(int repulsiveForceConstant);
    void centerFunc(double centerConstant);
    void updatePositions(double deltaT, int maxDisplacementSquared);
    cs225::HSLAPixel getRandColor();
    Graph g_;
    bool calculateWithinRadius(int x, int y, int ctr_idx, int rad);
  

};