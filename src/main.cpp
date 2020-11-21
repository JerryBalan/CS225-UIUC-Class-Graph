#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

// Helper function to print vectors of vectors of strings
void printVect(std::vector<std::vector<std::string>> in) {
  for(int i = 0; i < in.size(); i++) {
    for(int j = 0; j < in[i].size(); j++) {
      std::cout << in[i][j];

      if(j != in[i].size() - 1)
        std::cout << ", ";
    }
    std::cout << std::endl;
  }
}

int main() {
  // Maybe use an already existing parser like this: https://github.com/ben-strasser/fast-cpp-csv-parser  ?

  // Reads csv file and saves contents to a vector of vectors of strings
  std::vector<std::vector<std::string>> fileContent;
  std::ifstream csvFile("Data/uiuc-prerequisites.csv");
  std::string currLine;
  int currLoc;

  if(!csvFile.is_open()) {
    std::cout << "File not found" << std::endl;
    return -1;
  }

  while(std::getline(csvFile, currLine)) {
    std::vector<std::string> tempVec;

    std::stringstream ss(currLine);

    while(ss.good()) {
      std::string tempStr;
      std::getline(ss, tempStr, ',');
      tempVec.push_back(tempStr);
    }

    fileContent.push_back(tempVec);
  }

  printVect(fileContent);
}
