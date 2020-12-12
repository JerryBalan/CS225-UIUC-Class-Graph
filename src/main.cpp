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

  // ask user for input
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
  std::cout << "2) The (shortest) path to reach a desired class from your current class" << std::endl;
  std::cout << "3) A performance analysis of serial vs parallel force direct graphing (Fruchterman-Reingold algorithm)" << std::endl;

  // parse user input
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

    if(!g.vertexExists(inputClass)) { // invalid input
      std::cout << "Invalid class input. Terminating program." << std::endl;
      return -1;
    }

    //calling BFS on inputted classes after making sure it exists
    std::vector<Vertex> path;
    g.BFS(inputClass, path);
    std::sort(path.begin(), path.end());
    std::cout << "size: " << path.size() << std::endl; //how many prerequesites there are (+ class itself)

    // test read
    std::cout << "Prerequisites: " << path[0];
    for(unsigned i = 1; i < path.size(); i++)
      std::cout << ", " << path[i];
    std::cout << std::endl;
    
    path.push_back(inputClass);
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

    std::cout << "Please select one of the following options ('1', '2', etc.):" << std::endl;
    std::cout << "1) Djikstra" << std::endl;
    std::cout << "2) Floyd-Warshall (this will be time consuming, ~5 min on default filtered dataset)" << std::endl;

    int methodInput;
    std::cin >> methodInput;
    std::cout << "\n";



    // run algo
    vector<Vertex> pathShort;
    if(methodInput == 1) { //djik
      pathShort = courses.shortestPath(ogClass, destClass);
    } else if(methodInput == 2) { //warsh
      pathShort = courses.warshall(ogClass, destClass);
    } else {
      std::cout << "Incorrect input. Terminating program." << std::endl;
      return -1;
    }

    std::cout << "Done!" << std::endl;
    
    // print classes
    std::cout << "size: " << pathShort.size() << std::endl; //how many prerequesites there are (+ class itself)
    std::cout << "Path: " << pathShort[0];
    for(unsigned i = 1; i < pathShort.size(); i++)
      std::cout << ", " << pathShort[i];
    std::cout << std::endl;


    Classes subsetCourses(fileName, pathShort);
    // Graph gSubset = subsetCourses.getGraph();
    subsetCourses.createOutputImg(1, "subsetGraph.png");
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
    // pass image fdg to helper function of class (courses)
    courses.createOutputImg(methodInput - 1, "fdgOutput.png");
  }
  
  return 0;
}
