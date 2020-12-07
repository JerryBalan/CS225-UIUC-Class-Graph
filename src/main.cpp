#include <vector>
#include <string>
#include "fileIO.h"
#include "fdgOutput.h"
#include "classes.h"
#include "../structures/PNG.h"

int main(int argc, char * argv[]) {
//   std::vector<std::vector<std::string>> csvVect = csvToVector("Data/uiuc-prerequisites.csv");
//   printVect(csvVect);
  if(argv[1][0] == '0') {
    std::cout << "Using serial method..." << std::endl;

    Classes test;

    Graph g = test.getGraph();

    //fdgOutput newOut(g, 10, 10, 100);
    fdgOutput newOut(0, g, 100); // Serial
    cs225::PNG img = newOut.createOutputImage();
    img.writeToFile("testOutput"+ string(".png"));
    newOut.printLocations();
  } else {
    std::cout << "Using parallel method..." << std::endl;

    Classes test;

    Graph g = test.getGraph();

    //fdgOutput newOut(g, 10, 10, 100);
    fdgOutput newOut(1, g, 100); // Parallel
    cs225::PNG img = newOut.createOutputImage();
    img.writeToFile("testOutput"+ string(".png"));
    newOut.printLocations();
  }
}
