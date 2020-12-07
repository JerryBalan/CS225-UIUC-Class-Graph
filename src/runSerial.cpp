#include <vector>
#include <string>
#include "fileIO.h"
#include "fdgOutput.h"
#include "classes.h"
#include "../structures/PNG.h"

int main() {
  Classes test;

  Graph g = test.getGraph();
  fdgOutput newOut(0, g, 100); // Serial
  cs225::PNG img = newOut.createOutputImage();
  img.writeToFile("testOutput"+ string(".png"));
  newOut.printLocations();
}
