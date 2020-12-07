#pragma once
#include "structures/graph.h"
#include "fileIO.h"
#include <vector>
#include <iostream>
#include <string>

class Classes {
  public:
    Classes();
    Classes(std::string filePath);
    ~Classes();

    void print();
    void savePNG();

    Graph getGraph();

    /**
     * Print shortest path from two classes
     */
    std::vector<std::string> shortestPath(std::string, std::string);
    /**
     * List all classes up to a certain depth from an origin class
     */
    std::vector<std::string> bfs(std::string originClass, unsigned depth);

    void displayClass(std::string classStr, unsigned depth);


  private:
    Graph g_;
    void buildClassesGraph(std::string filePath);
};