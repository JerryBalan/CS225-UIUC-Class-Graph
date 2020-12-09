#include <vector>
#include <string>
#include "fileIO.h"
#include "fdgOutput.h"
#include "classes.h"
#include "structures/PNG.h"

int main(int argc, char * argv[]) {
//   std::vector<std::vector<std::string>> csvVect = csvToVector("Data/uiuc-prerequisites.csv");
//   printVect(csvVect);
/*
  if (argc < 2) {
    std::cout << "Please input an argument" << std::endl;
    return -1;
  }
  Classes test;

  Graph g = test.getGraph();
  if(argv[1][0] == '0') {
    std::cout << "Using serial method..." << std::endl;

    //fdgOutput newOut(g, 10, 10, 100);
    fdgOutput newOut(0, g, 100, test.getFrequencies()); // Serial
    cs225::PNG img = newOut.createOutputImage(test.getFrequencies());
    img.writeToFile("testOutput"+ string(".png"));
    newOut.printLocations();
  } else {
    std::cout << "Using parallel method..." << std::endl;

    //fdgOutput newOut(g, 10, 10, 100);
    fdgOutput newOut(1, g, 100, test.getFrequencies()); // Parallel
    cs225::PNG img = newOut.createOutputImage(test.getFrequencies());
    img.writeToFile("testOutput"+ string(".png"));
    newOut.printLocations();
  }
  */

  Classes courses; // auto init with csv

  Graph g = courses.getGraph();

  if (argc > 1) {
    std::cout << "Please respond to the following questions rather than providing arguments.\n" << std::endl;
    //return -1;
  }
  std::cout << "Please select one of the following options ('1', '2', etc.) to display in a graph:" << std::endl;
  std::cout << "1) A class and all of it's prerequisites." << std::endl;
  std::cout << "2) The (shortest) path to reach a desired class from your current class." << std::endl;
  std::cout << "3) A performance analysis of serial vs parallel force direct graphing." << std::endl;
  //std::cout << "" << std::endl;

  int displayInput = 0;
  std::cin >> displayInput;
  std::cout << "\n";

  if(displayInput == 0) { // cin fail state, also happens to be an invalid default state
    std::cout << "Incorrect input. Terminating program." << std::endl;
    return -1;
  }
  if(displayInput == 1) { // run BFS

    std::cout << "Please select a class to graph. Limited to CS/ECE and their prerequisites." << std::endl;
    std::cout << "Use the following format (with the space): 'ECE 391' or 'CS 225'" << std::endl;

    string classType;
    string classNum;
    std::cin >> classType >> classNum;
    string inputClass = classType + " " + classNum;
    std::cout << "\n";
    
    std::cout << "[" << inputClass << "]";

    if(!g.vertexExists(inputClass)) { // invalid input
      std::cout << "Invalid class input. Terminating program." << std::endl;
      return -1;
    }
    vector<Vertex> path = courses.bfs(inputClass);

    // test read
    for(size_t i = 0; i < path.size(); i++) {
      std::cout << path[i] << ",";
    }
    std::cout << std::endl;

  }
  if(displayInput == 2) { // djik/floyd-warshall
    
  }
  if(displayInput == 3) { // run fdg
    std::cout << "Please select one of the following options ('1', '2', etc.):" << std::endl;
    std::cout << "1) Serial" << std::endl;
    std::cout << "2) Parallel" << std::endl;

    int methodInput = 0;
    std::cin >> methodInput;
    std::cout << "\n";
    if(methodInput == 0) { // cin fail state, also happens to be an invalid default state
      std::cout << "Incorrect input. Terminating program." << std::endl;
      return -1;
    }
    if(methodInput == 1) { // serial
      std::cout << "Using serial method..." << std::endl;

      //fdgOutput newOut(g, 10, 10, 100);
      fdgOutput newOut(0, g, 100, courses.getFrequencies()); // Serial
      cs225::PNG img = newOut.createOutputImage(courses.getFrequencies());
      img.writeToFile("testOutput"+ string(".png"));
      newOut.printLocations();
    }
    if(methodInput == 2) { // parallel
      std::cout << "Using parallel method..." << std::endl;

      //fdgOutput newOut(g, 10, 10, 100);
      fdgOutput newOut(1, g, 100, courses.getFrequencies()); // Parallel
      cs225::PNG img = newOut.createOutputImage(courses.getFrequencies());
      img.writeToFile("testOutput"+ string(".png"));
      newOut.printLocations();
    }
  }


  
  return 0;
}
