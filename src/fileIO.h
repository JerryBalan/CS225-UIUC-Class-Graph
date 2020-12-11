#include <iostream>
#include <string>
#include <vector>

#pragma once

void printVect(std::vector<std::vector<std::string>> in);
std::string trim(const std::string& str);
std::vector<std::vector<std::string>> csvToVector(std::string fileName);
