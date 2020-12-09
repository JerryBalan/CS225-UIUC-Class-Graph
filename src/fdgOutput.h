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
    fdgOutput(int version, Graph graph, unsigned iterations, std::unordered_map<std::string, double> subjectFrequencies);
    fdgOutput(int version, Graph graph, int scale, unsigned iterations, int classAmnt, std::unordered_map<std::string, double> subjectFrequencies);
    ~fdgOutput();

    // void defineLocationsSerial(Graph graph, std::unordered_map<std::string, double> &subjectFrequencies, int scale, unsigned iterations, int classAmnt);

    void defineLocationsSerial(Graph graph, std::unordered_map<std::string, double> subjectFrequencies);
    void printLocations();

    cs225::PNG createOutputImage(std::unordered_map<std::string, double> subjectFrequencies);

  private:
    int area, width;
    std::vector<std::pair<float, float>> disp;
    std::vector<std::pair<float, float>> pos;
    std::vector<std::tuple<double, double, double>> colors;
    std::vector<Vertex> v;
    std::vector<Edge> e;

    std::string getCourseSubject(std::string course);
    double fRand(double fMin, double fMax);

    float fA(float k, float x);
    float fT(float k, float x);



    cs225::HSLAPixel getRandColor();
};