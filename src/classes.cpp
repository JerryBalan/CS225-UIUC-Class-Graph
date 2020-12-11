#include "classes.h"
#include <unordered_map>
#include <queue>
#include <utility>
#include <algorithm>
#include <functional>
#include <map>
#include <set>

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

void Classes::buildClassesGraphSubset(string filePath, vector<string> subsetOfClasses) {
  g_ = Graph();
  vector<vector<string>> allClasses = csvToVector(filePath);
  vector<vector<string>> subsetClassesPrereq;
  int total = 0;
  // parse for subset
  for(string str : subsetOfClasses) {
    for(vector<string> classAndPrereqs : allClasses) {
      if(classAndPrereqs[0] == str) {
        subsetClassesPrereq.push_back(classAndPrereqs);
        continue;
      }
    }
  }



  for (auto& v : subsetClassesPrereq) {
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

  for (auto& v : subsetClassesPrereq) {
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

Classes::Classes(string filePath, vector<string> subsetOfClasses) {
  buildClassesGraphSubset(filePath, subsetOfClasses);
}


Classes::~Classes() {}

vector<string> Classes::bfs(string origin) {
  vector<string> classesFound;
  g_.BFS(Vertex(origin), classesFound);
  return classesFound;
}

vector<string> Classes::shortestPath(string origin, string dest) {
  vector<string> path;
  int inf = 10000;
  std::set<Vertex> vSet;
  vector<Vertex> allVtx = g_.getVertices();
  unordered_map<Vertex, int> dist;
  unordered_map<Vertex, Vertex> prev;

  for(Vertex v : allVtx) {
    dist[v] = inf;
    prev[v] = "";
    vSet.insert(v);
  }
  dist[origin] = 0;

  while(!vSet.empty()) {
    Vertex minVtx;
    int min_dist = inf + 1;
    for(Vertex v : vSet) {
      if(dist[v] < min_dist) {
        minVtx = v;
        min_dist = dist[v];
      }
    }

    vSet.erase(minVtx);

    vector<Vertex> adjVtxs = g_.getAdjacent(minVtx);
    for(Vertex v : adjVtxs) {
      if(vSet.count(v) > 0) {
        int newDist = dist[v] + 1;
        if(newDist < dist[v]) {
          dist[v] = newDist;
          prev[v] = minVtx;
        }
      }
      //std::cout << newDist << std::endl;
    }
    
  }
  // done with main algo, traverse
  
  Vertex vtx = dest;
  std::cout << prev[vtx] << std::endl;
  if(prev[vtx] != "" || origin == dest) {
    while(vtx != "") {
      path.push_back(vtx);
      vtx = prev[vtx];
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



