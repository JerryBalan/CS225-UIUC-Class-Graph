#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>

#include "../classes.h"
#include "../fdgOutput.h"
#include "../fileIO.h"
#include "../structures/PNG.h"
#include "../structures/catch/catch.hpp"

using namespace cs225;
using namespace std;

TEST_CASE("BFS-CS225", "[weight=10][part1]") {
  Classes test;
  Graph g = test.getGraph();
  std::vector<Vertex> output,
      expectedOutput{"CS 125",   "CS 173",   "ECE 120",  "ECE 220",
                     "MATH 112", "MATH 213", "MATH 220", "MATH 221"};
  g.BFS("CS 225", output);

  std::sort(output.begin(), output.end());
  std::sort(expectedOutput.begin(), expectedOutput.end());


  REQUIRE(output.size() == expectedOutput.size());
  REQUIRE(output == expectedOutput);
}

TEST_CASE("BFS-ECE391", "[weight=10][part1]") {
  Classes test;
  Graph g = test.getGraph();
  std::vector<Vertex> output,
      expectedOutput{"CS 125",  "CS 173",   "CS 225",   "CS 233",   "ECE 120",
                     "ECE 220", "MATH 112", "MATH 213", "MATH 220", "MATH 221"};
  g.BFS("ECE 391", output);

  std::sort(output.begin(), output.end());
  std::sort(expectedOutput.begin(), expectedOutput.end());

  REQUIRE(output.size() == expectedOutput.size());
  REQUIRE(output == expectedOutput);
}

TEST_CASE("BFS-ECE420", "[weight=10][part1]") {
  Classes test;
  Graph g = test.getGraph();
  std::vector<Vertex> output,
      expectedOutput{"ECE 110",  "ECE 210",  "ECE 310",  "MATH 220",
                     "MATH 221", "MATH 231", "MATH 241", "MATH 285",
                     "MATH 286", "PHYS 211", "PHYS 212"};
  g.BFS("ECE 420", output);

  std::sort(output.begin(), output.end());
  std::sort(expectedOutput.begin(), expectedOutput.end());

  REQUIRE(output.size() == expectedOutput.size());
  REQUIRE(output == expectedOutput);
}

TEST_CASE("Floyd-Warshall-1", "[weight=10][part2]") {
  // Classes test("Data/uiuc-prerequisites-cs.csv");
  Classes test("Data/uiuc-prerequisites-cs-ece.csv");
  std::string source = "ECE 120";
  std::string dest = "ECE 391";
  std::vector<std::string> output = test.warshall(source, dest);
  std::vector<std::string> actual = {"ECE 120", "ECE 220", "ECE 391"};
  // for (size_t i = 0; i < output.size(); i++) {
  //   std::cout << output[i] << std::endl;
  // }
  // std::cout << output.size() << std::endl;
  REQUIRE(output.size() == 3);
  REQUIRE(output == actual);
}

TEST_CASE("Floyd-Warshall-2", "[weight=10][part2]") {
  // Classes test("Data/uiuc-prerequisites-cs.csv");
  Classes test("Data/uiuc-prerequisites-cs-ece-math-phys-subset.csv");
  std::string source = "MATH 112";
  std::string dest = "CS 586";
  std::vector<std::string> output = test.warshall(source, dest);
  std::vector<std::string> actual = {"MATH 112", "CS 125", "CS 225", "CS 374",
                                     "CS 586"};
  REQUIRE(output.size() == actual.size());
  REQUIRE(output == actual);
}

TEST_CASE(
    "Floyd-Warshall-No-Path",
    "[weight=10][part2]") {  // Classes test("Data/uiuc-prerequisites-ece.csv");
  Classes test("Data/uiuc-prerequisites-cs-ece-math-phys-subset.csv");
  std::string source = "ECE 391";
  std::string dest = "ECE 120";

  std::vector<std::string> output = test.warshall(source, dest);
  REQUIRE(output.size() == 0);
}

TEST_CASE("Shortest-path-1", "[weight=10][part3]") {
  // Classes test("Data/uiuc-prerequisites-cs.csv");
  Classes test("Data/uiuc-prerequisites-cs-ece.csv");
  std::string source = "ECE 120";
  std::string dest = "ECE 391";
  std::vector<std::string> output = test.shortestPath(source, dest);
  std::vector<std::string> actual = {"ECE 120", "ECE 220", "ECE 391"};
  // for (size_t i = 0; i < output.size(); i++) {
  //   std::cout << output[i] << std::endl;
  // }
  // std::cout << output.size() << std::endl;
  REQUIRE(output.size() == 3);
  REQUIRE(output == actual);
}

TEST_CASE("Shortest-path-2", "[weight=10][part3]") {
  // Classes test("Data/uiuc-prerequisites-cs.csv");
  Classes test("Data/uiuc-prerequisites-cs-ece-math-phys-subset.csv");
  std::string source = "MATH 112";
  std::string dest = "CS 586";
  std::vector<std::string> output = test.shortestPath(source, dest);
  std::vector<std::string> actual = {"MATH 112", "CS 125", "CS 225", "CS 374",
                                     "CS 586"};
  REQUIRE(output.size() == actual.size());
  REQUIRE(output == actual);
}

TEST_CASE("Shortest-path: does not exist", "[weight=10][part3]") {
    Classes test("Data/uiuc-prerequisites-cs-ece-math-phys-subset.csv");
  std::string source = "ECE 391";
  std::string dest = "ECE 120";

  std::vector<std::string> output = test.shortestPath(source, dest);
  REQUIRE(output.size() == 0);
}

TEST_CASE("Force-directed test image exists (parallel)", "[weight=10][part4]") {
    Classes test("Data/uiuc-prerequisites-cs-ece-math-phys-subset.csv");
    test.createOutputImg(1, "tests/testImageParallel.png");
    cs225::PNG img;
    REQUIRE (img.readFromFile("tests/testImageParallel.png"));

}

TEST_CASE("Force-directed test image exists (serial)", "[weight=10][part4]") {
    Classes test("Data/uiuc-prerequisites-cs-ece-math-phys-subset.csv");
    test.createOutputImg(0, "tests/testImageSerial.png");
    cs225::PNG img;
    
    REQUIRE (img.readFromFile("tests/testImageSerial.png"));
}