#include "../structures/catch/catch.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stack>
#include "../fileIO.h"
#include "../fdgOutput.h"
#include "../classes.h"
#include "../structures/PNG.h"

using namespace cs225;
using namespace std;

TEST_CASE("BFS-CS225", "[weight=10][part1]")
{
	Classes test;
	Graph g = test.getGraph();
	std::vector<Vertex> output, expectedOutput{"CS 125", "CS 173", "CS 225", "ECE 120", "ECE 220", "MATH 112", "MATH 213", "MATH 220", "MATH 221"};
	g.BFS("CS 225", output);

	std::sort(output.begin(), output.end());
	std::sort(expectedOutput.begin(), expectedOutput.end());

	// for(unsigned i = 0; i < output.size(); i++) {
	// 	std::cout << output[i];

	// 	if(i != output.size() - 1)
	// 		std::cout << ", ";
	// }
	// std::cout << std::endl;

	REQUIRE(output.size() == expectedOutput.size());
	REQUIRE(output == expectedOutput);
}

TEST_CASE("BFS-ECE391", "[weight=10][part1]")
{
	Classes test;
	Graph g = test.getGraph();
	std::vector<Vertex> output, expectedOutput{"CS 125", "CS 173", "CS 225", "CS 233", "ECE 120", "ECE 220", "ECE 391", "MATH 112", "MATH 213", "MATH 220", "MATH 221"};
	g.BFS("ECE 391", output);

	std::sort(output.begin(), output.end());
	std::sort(expectedOutput.begin(), expectedOutput.end());

	REQUIRE(output.size() == expectedOutput.size());
	REQUIRE(output == expectedOutput);
}

TEST_CASE("BFS-ECE420", "[weight=10][part1]")
{
	Classes test;
	Graph g = test.getGraph();
	std::vector<Vertex> output, expectedOutput{"ECE 110", "ECE 210", "ECE 310", "ECE 420", "MATH 220", "MATH 221", "MATH 231", "MATH 241", "MATH 285", "MATH 286", "PHYS 211", "PHYS 212"};
	g.BFS("ECE 420", output);

	std::sort(output.begin(), output.end());
	std::sort(expectedOutput.begin(), expectedOutput.end());

	REQUIRE(output.size() == expectedOutput.size());
	REQUIRE(output == expectedOutput);
}

TEST_CASE("A-Star-1", "[weight=10][part1]")
{
	REQUIRE(1 == 2);
}

TEST_CASE("A-Star-2", "[weight=10][part1]")
{
	REQUIRE(1 == 2);
}

TEST_CASE("A-Star-3", "[weight=10][part1]")
{
	REQUIRE(1 == 2);
}

TEST_CASE("Output-Img-1", "[weight=10][part1]")
{
	REQUIRE(1 == 2);
}

TEST_CASE("Output-Img-2", "[weight=10][part1]")
{
	REQUIRE(1 == 2);
}