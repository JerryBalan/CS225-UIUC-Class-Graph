#include <vector>
#include <string>
#include "fileIO.h"
#include "fdgOutput.h"
#include "classes.h"
#include "structures/PNG.h"

int main(int argc, char * argv[]) {
  // run for graph.py
  if (argc > 1) {
    std::cout << "This mode  is meant for graph.py. If you want to use the main program, please run again without any arguments." << std::endl;
    Classes courses("Data/uiuc-prerequisites-cs-ece-math-phys-subset.csv");
    courses.createOutputImg(argv[1][0] - '0', "fdgOutput.png");
    return 0;
  }

  //printVect(csvToVector("Data/uiuc-prerequisites-cs-ece-math-phys-subset.csv"));

  std::cout << "Please input the location/name of the input CSV file (leave blank for default): ";
  std::string fileName;
  getline(std::cin, fileName);
  // default file
  if(fileName.empty())
    fileName = "Data/uiuc-prerequisites-cs-ece-math-phys-subset.csv";

  std::cout << fileName << std::endl;
  Classes courses(fileName);
  Graph g = courses.getGraph();
    
  // prompt for imput
  std::cout << "Please select one of the following options ('1', '2', etc.) to display in a graph:" << std::endl;
  std::cout << "1) A class and all of its prerequisites (BFS)" << std::endl;
  std::cout << "2) The (shortest) path to reach a desired class from your current class (A*)" << std::endl;
  std::cout << "3) A performance analysis of serial vs parallel force direct graphing (Fruchterman-Reingold algorithm)" << std::endl;
  //std::cout << "" << std::endl;

  // parse user input]
  std::string dInStr;
  getline(std::cin, dInStr);
  int displayInput = std::stoi(dInStr);
  std::cout << "\n";

  if(displayInput == 0) { // cin fail state, also happens to be an invalid default state
    std::cout << "Incorrect input. Terminating program." << std::endl;
    return -1;
  }

  if(displayInput == 1) { // run BFS
    std::cout << "Please select a class to graph. Limited to CS/ECE and their prerequisites." << std::endl;
    std::cout << "Use the following format (with the space): 'ECE 391' or 'CS 225'" << std::endl;

    std::string inputClass;
    getline(std::cin, inputClass);
    std::cout << "\n";
    
    //std::cout << "Results for [" << inputClass << "]" << std::endl;;

    if(!g.vertexExists(inputClass)) { // invalid input
      std::cout << "Invalid class input. Terminating program." << std::endl;
      return -1;
    }


    // code used for testing specific classes
    // std::vector<Vertex> adj = g.getAdjacent("ECE 391");
    // std::cout << "adj: ";
    // for(unsigned i = 0; i < adj.size(); i++)
    //   std::cout << adj[i] << std::endl;
    // std::cout << "end adj";

    // std::cout << std::endl;


    // for(unsigned i = 0; i < adj.size(); i++) {
    //   std::cout << "adj for " << adj[i] << std::endl;
    //   std::vector<Vertex> newAdj = g.getAdjacent(adj[i]);

    //   for(unsigned j = 0; j < newAdj.size(); j++)
    //     std::cout << newAdj[j] << std::endl;
    //   std::cout << "end adj";
    // }

    // g.initSnapshot("1");
    // g.snapshot();
    //g.print();



    //vector<Vertex> path = courses.bfs(inputClass);

    //calling BFS on inputted classes after making sure it exists
    std::vector<Vertex> path;
    g.BFS(inputClass, path);
    std::sort(path.begin(), path.end());
    std::cout << "size: " << path.size() << std::endl; //how many prerequesites there are (+ class itself)

    // test read
    for(unsigned i = 0; i < path.size(); i++)
      std::cout << path[i] << ", ";
    std::cout << std::endl;

    // build a graph with just those
    Classes subsetCourses(fileName, path);
    // Graph gSubset = subsetCourses.getGraph();
    subsetCourses.createOutputImg(1, "subsetGraph.png");
  }
  
  if(displayInput == 2) { // djik/floyd-warshall
    std::cout << "Please select a DESTINATION class to graph. Limited to CS/ECE and their prerequisites." << std::endl;
    std::cout << "Use the following format (with the space): 'ECE 391' or 'CS 225'" << std::endl;

    string classTypeDest;
    string classNumDest;
    std::cin >> classTypeDest >> classNumDest;
    string destClass = classTypeDest + " " + classNumDest;
    std::cout << "\n";
    
    //std::cout << "[" << destClass << "]" << std::endl;

    if(!g.vertexExists(destClass)) { // invalid input
      std::cout << "Invalid class input. Terminating program." << std::endl;
      return -1;
    }

    std::cout << "Please select an ORIGIN class to graph. Limited to CS/ECE and their prerequisites." << std::endl;
    std::cout << "Use the following format (with the space): 'ECE 391' or 'CS 225'" << std::endl;

    // get input classes
    string classTypeOG;
    string classNumOG;
    std::cin >> classTypeOG >> classNumOG;
    string ogClass = classTypeOG + " " + classNumOG;
    std::cout << "\n";
    
    //std::cout << "[" << ogClass << "]" << std::endl;

    if(!g.vertexExists(ogClass)) { // invalid input
      std::cout << "Invalid class input. Terminating program." << std::endl;
      return -1;
    }

    // run algo
    vector<Vertex> pathShort = courses.shortestPath(ogClass, destClass);
    //vector<Vertex> pathShort = courses.warshall(ogClass, destClass);
    std::cout << "Done!" << std::endl;
    // test read
    
    // print classes
    for(size_t i = 0; i < pathShort.size(); i++) {
      std::cout << pathShort[i] << std::endl;
    }
    std::cout << pathShort.size() << std::endl;
  }
  
  if(displayInput == 3) { // run fdg
    std::cout << "Please select one of the following options ('1', '2', etc.):" << std::endl;
    std::cout << "1) Serial" << std::endl;
    std::cout << "2) Parallel" << std::endl;

    std::string mInStr;
    getline(std::cin, mInStr);
    int methodInput = std::stoi(mInStr);
    std::cout << "\n";
    if(methodInput != 1 && methodInput != 2) { // cin fail state, also happens to be an invalid default state
      std::cout << "Incorrect input. Terminating program." << std::endl;
      return -1;
    }

    courses.createOutputImg(methodInput - 1, "fdgOutput.png");
  }
  
  return 0;
}
