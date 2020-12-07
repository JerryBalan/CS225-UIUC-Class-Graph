#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include "fileIO.h"


// Helper function to print vectors of vectors of strings
void printVect(std::vector<std::vector<std::string>> in) {
  for(unsigned i = 0; i < in.size(); i++) {
    for(unsigned j = 0; j < in[i].size(); j++) {
      std::cout << in[i][j];

      if(j != in[i].size() - 1)
        std::cout << ", ";
    }
    std::cout << std::endl;
  }
}
// hassan = god
std::vector<std::vector<std::string>> csvToVector(std::string fileName) {
  // Maybe use an already existing parser like this: https://github.com/ben-strasser/fast-cpp-csv-parser  ?

  // Reads csv file and saves contents to a vector of vectors of strings
  std::vector<std::vector<std::string>> fileContent;
  std::ifstream csvFile(fileName);
  std::string currLine;
  
  // Early return if file doesn't exist
  if(!csvFile.is_open()) {
    std::cout << "File not found" << std::endl;
    return fileContent;
  }

  while(std::getline(csvFile, currLine)) {
    std::vector<std::string> tempVec;

    std::stringstream ss(currLine);
    int curr = 0;
    while(ss.good()) {
      std::string tempStr;
      std::getline(ss, tempStr, ',');

      // Don't add number of pre-reqs to vector
      if(curr != 1)
        tempVec.push_back(tempStr);
      curr++;
    }

    // Only CS/ECE courses
    if(tempVec[0].rfind("MATH", 0) == 0 || tempVec[0].rfind("PHYS", 0) == 0 || (tempVec[0].rfind("CS", 0) == 0 && tempVec[0].rfind("CSE", 0) != 0) || tempVec[0].rfind("ECE", 0) == 0)
      fileContent.push_back(tempVec);
  }

  return fileContent;
}
