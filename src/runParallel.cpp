#include <vector>
#include <string>
#include "fileIO.h"
#include "fdgOutput.h"
#include "classes.h"
#include "structures/PNG.h"

int main() {
  std::cout << "Using multithreading..." << std::endl;
  
  Classes test;

  Graph g = test.getGraph();
  fdgOutput newOut(1, g, 100); // Parallel
  cs225::PNG img = newOut.createOutputImage();
  img.writeToFile("testOutput"+ string(".png"));
  newOut.printLocations();
}
