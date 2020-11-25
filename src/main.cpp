#include <vector>
#include <string>
#include "fileIO.h"


int main() {
  std::vector<std::vector<std::string>> csvVect = csvToVector("Data/uiuc-prerequisites.csv");
  printVect(csvVect);
}
