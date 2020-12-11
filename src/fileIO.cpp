#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include "fileIO.h"

#include <algorithm>
#include <functional>
#include <array>
#include <iostream>


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

std::string trim(const std::string& str) {
  size_t first = str.find_first_not_of(' ');
  if (std::string::npos == first)
      return str;
  size_t last = str.find_last_not_of(' ');
  return str.substr(first, (last - first + 1));
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

  int lineCounter = -1;
  while(std::getline(csvFile, currLine)) {
    lineCounter++;
    if (!lineCounter) {
      continue;
    }
    std::vector<std::string> tempVec;

    std::stringstream ss(currLine);
    int curr = 0;
    while(ss.good()) {

      std::string tempStr;
      std::getline(ss, tempStr, ',');

      // Newlines can die
      tempStr.erase(std::remove(tempStr.begin(), tempStr.end(), '\r'), tempStr.end());

      if(curr != 1)
        tempVec.push_back(trim(tempStr));
      curr++;
    }

    // Only CS/ECE courses
    fileContent.push_back(tempVec);
  }

  return fileContent;
}
