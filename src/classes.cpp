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
  int total = 0;

  for (auto& v : allClasses) {
    // Update class frequencies
    total++;
    std::string dept = v[0].substr(0, v[0].find(' '));
    if(subjectFrequencies.find(dept) == subjectFrequencies.end())
      subjectFrequencies.insert({dept, 1});
    else
      subjectFrequencies[dept]++;

    g_.insertVertex(v[0]);
  }

  // Convert number of classes to percentage amount
  for(auto &it : subjectFrequencies)
    it.second /= total;

  for (auto& v : allClasses) {
    for (unsigned i = 1; i < v.size(); i++) {
      // v[i] is a prereq to v[0]
      if (!g_.vertexExists(v[i])) {
        // insert vertex
        // set prereq connection classes[i] to classes[0]
        g_.insertVertex(v[i]);
      }
      // connect v[0] to v[i]


      bool success = g_.insertEdge(v[0], v[i]);
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
std::vector<std::string> Classes::warshall(std::string origin, std::string dest) {
  //vector<vector<int>> distMatrix;
  //vector<vector<string>> pathMatrix;
  std::unordered_map<string, std::unordered_map<string, int>> distMatrix; // last int is the distance
  std::unordered_map<string, std::unordered_map<string, Vertex>> pathMatrix; //last str is the class (vtx)
  int inf = 10000; // max int

  vector<Vertex> allClasses = g_.getVertices();
  vector<Edge> allEdges = g_.getEdges();

  // map default values of infinity
  for(Vertex outerClass : allClasses) {
    for(Vertex innerClass : allClasses) {
      distMatrix[outerClass][innerClass] = inf;
      pathMatrix[outerClass][innerClass] = "";
    }
  }
  std::cout << "finished initial" << std::endl;
  
  for(Edge e : allEdges) {
    // source = high level class
    // dest = low level
    //distMatrix[e.source][e.dest] = 1; //all edges have weight 1
    //pathMatrix[e.source][e.dest] = e.dest;
    distMatrix[e.dest][e.source] = 1; //all edges have weight 1
    pathMatrix[e.dest][e.source] = e.source;
  }
  for(Vertex singleClass : allClasses) {
    distMatrix[singleClass][singleClass] = 0;
    pathMatrix[singleClass][singleClass] = singleClass;
  }

  std::cout << "finished initial2" << std::endl;
  for(Vertex w : allClasses) {
    for(Vertex u : allClasses) {
      for(Vertex v : allClasses) {
        if(distMatrix[u][w] == inf || distMatrix[w][v] == inf) {
          continue;
        }
        if(distMatrix[u][v] > distMatrix[u][w] + distMatrix[w][v]) {
          distMatrix[u][v] = distMatrix[u][w] + distMatrix[w][v];
          pathMatrix[u][v] = pathMatrix[u][w];
        }
      }
    }
  }
  std::cout << "finished initial3" << std::endl;
  
  vector<Vertex> path;
  Vertex temp = origin;
  if(pathMatrix[origin][dest] == "") {
    
    return path;
  }
  path.push_back(origin);
  while(temp != dest) {
    temp = pathMatrix[temp][dest];
    path.push_back(temp);
  }

 

  return path;
}
std::unordered_map<std::string, double> Classes::getFrequencies() {
  return subjectFrequencies;
}

void Classes::print() {}

void Classes::savePNG() {}

Graph Classes::getGraph() {
  return g_;
}



