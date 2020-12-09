#include <vector>
#include <string>
#include "fileIO.h"
#include "fdgOutput.h"
#include "classes.h"
#include "structures/PNG.h"

int main(int argc, char * argv[]) {
  // std::vector<std::vector<std::string>> csvVect = csvToVector("Data/uiuc-prerequisites-less.csv");
  // printVect(csvVect);

  std::cout << "Using serial method..." << std::endl;

  Classes test;

  Graph g = test.getGraph();

  //fdgOutput newOut(g, 10, 10, 100);
  fdgOutput newOut(0, g, 5, test.getFrequencies()); // Serial
  cs225::PNG img = newOut.createOutputImage(test.getFrequencies());
  img.writeToFile("testOutput"+ string(".png"));
  newOut.printLocations();
  
  return 0;
}
