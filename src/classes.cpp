#include "classes.h"
#include <unordered_map>
#include <queue>
#include <utility>
#include <algorithm>
#include <functional>
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
      // v[i] is a prereq to v[0]
      if (!g_.vertexExists(v[i])) {
        // insert vertex
        // set prereq connection classes[i] to classes[0]
        g_.insertVertex(v[i]);
      }
      // connect v[0] to v[i]
      g_.insertEdge(v[0], v[i]);
      g_.setEdgeLabel(v[0], v[i], "prereq");
    }
  }
}
Classes::Classes() { buildClassesGraph("Data/uiuc-prerequisites.csv"); }

Classes::Classes(string filePath) { buildClassesGraph(filePath); }

Classes::~Classes() {}

vector<string> Classes::bfs(string origin) {
  vector<string> classesFound;
  g_.BFS(Vertex(origin), classesFound);
  return classesFound;
}

vector<string> Classes::shortestPath(string origin, string dest) {
  vector<string> path;
    //std::unordered_map<string, int> path;
  //vector<int> dists;
  std::unordered_map<string, int> dists;
  int inf = INT_MAX;
  for(Vertex v : g_.getVertices()) {
    path.push_back(nullptr);
    //path[v] = nullptr;
    dists[v] = inf;
  }
  dists[origin] = 0;

  std::priority_queue<std::pair<int, string>, vector<std::pair<int, string>>, std::greater<std::pair<int, string>>> q; // pq of verticies, minheap syntax
  //build pq
  q.push(make_pair(0, origin));

  while(!q.empty()) {
    string vtxClass = q.top().second;
    path.push_back(vtxClass);
    q.pop();
    vector<string> adjVtxs = g_.getAdjacent(vtxClass);
    for(Vertex v : adjVtxs) {
      if(dists[v] == inf) {
        q.push(make_pair(dists[v], v));
        dists[v] = dists[vtxClass] + 1;
      }
      /*
      if(std::find(path.begin(), path.end(), v) == path.end()) { // v is not in path
        if(dists[vtxClass] + 1 < dists[v]) { // 1 represents each edge weight, classes equivalent so 1
          q.push(make_pair());
          dists[v] = dists[vtxClass] + 1;
          path[v] // fix
        }
      }
      */
    }
  }

  return path;
}

void Classes::print() {}

void Classes::savePNG() {}

Graph Classes::getGraph() {
  return g_;
}
