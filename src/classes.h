#pragma once
#include "structures/graph.h"
#include "fileIO.h"
#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>

class Classes {
  public:
    // constructors
    Classes();
    Classes(std::string filePath);
    Classes(std::string filePath, vector<std::string> subsetOfClasses); // for subset only
    ~Classes();

    void print();
    void savePNG();

    Graph getGraph();
    std::unordered_map<std::string, double> getFrequencies();

    /**
     * Print shortest path from two classes (origin, destination)
     */
    std::vector<std::string> shortestPath(std::string, std::string);
    /**
     * List all classes up to a certain depth from an origin class
     */
    std::vector<std::string> bfs(std::string originClass);
    /**
     * Print shortest path from two classes (origin, destination)
     */
    std::vector<std::string> warshall(std::string, std::string);
    void displayClass(std::string classStr, unsigned depth);
    /*
    ** Generate output image of the current class
    ** You can specify if you run it serially or parallelized
    */
    void createOutputImg(int argVal, std::string imgName);
    

  private:
    Graph g_;
    // helper class constructors
    void buildClassesGraph(std::string filePath);
    void buildClassesGraphSubset(std::string filePath, vector<std::string> subsetOfClasses);
    std::unordered_map<std::string, double> subjectFrequencies;

    std::string filePath;
};