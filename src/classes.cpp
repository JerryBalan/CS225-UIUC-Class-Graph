#include "classes.h"
using std::string;
using std::vector;

void Classes::buildClassesGraph(string filePath) {
  g_ = Graph();
  vector<vector<string>> allClasses = csvToVector(filePath);

  for (auto& v : allClasses) {
    g_.insertVertex(v[0]);
  }

  for (auto& v : allClasses) {
    for (unsigned i = 1; i < v.size(); i++) {
      if (!g_.vertexExists(v[i])) {
        // insert vertex
        // set prereq connection classes[i] to classes[0]
        g_.insertVertex(v[i]);
      }
      // connect v[i] to v[0]
      g_.insertEdge(v[i], v[0]);
      g_.setEdgeLabel(v[i], v[0], "prerequisite");
    }
  }
}
Classes::Classes() { buildClassesGraph("Data/uiuc-prerequisites.csv"); }

Classes::Classes(string filePath) { buildClassesGraph(filePath); }

Classes::~Classes() {}

vector<string> Classes::bfs(string origin, unsigned depth) {
  vector<string> classesFound;

  return classesFound;
}

vector<string> Classes::shortestPath(string origin, string dest) {
  vector<string> path;

  return path;
}

void Classes::print() {}

void Classes::savePNG() {}

Graph Classes::getGraph() {
  return g_;
}
