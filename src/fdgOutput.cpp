#include "fdgOutput.h"

#include <math.h>

#include <fstream>
#include <iostream>
#include <mutex>

#include "structures/PNG.h"

#define SIGN(x) ((signbit(x) ? -1 : 1))

// parameters for tuning. We found these produced the best looking graph
// And the lines
#define SET_FDG_ALGORITHM_HYPERPARAMETERS                                     \
  int springRestLength = 50, repulsiveForceConstant = 500,                    \
      attractionConstant = 5, springConstant = 5, maxDisplacementSquared = 9; \
  double deltaT = 0.0003, centerConstant = 5; unsigned iterations = 1000;

std::mutex mtx;

fdgOutput::fdgOutput(
    int version, Graph graph, int sideSpace,
    std::unordered_map<std::string, double> subjectFrequencies) {
  // Run either the parallel or serial version depending on passed value
  initColorMap(subjectFrequencies);
  if (version == 0)
    defineLocationsSerial(graph, subjectFrequencies,sideSpace);
  else
    defineLocationsParallel(graph, subjectFrequencies, sideSpace);
}

// Nothing is allocated on the heap
fdgOutput::~fdgOutput() {}

// Helper function to set variables since multiple methods use it
void fdgOutput::setVariables(
    Graph graph, int scale,
    std::unordered_map<std::string, double> &subjectFrequencies,
    bool setCompletlyRandom) {
  // get the verticess and edges to not need to make this function call all the
  // time
  v = graph.getVertices();
  e = graph.getEdges();

  width = v.size() * scale;
  height = width;
  area = width * height;

  // Store the graph (use getAdjacent for radius of classes)
  g_ = graph;

  // Randomly assign positions
  if (setCompletlyRandom) {
    pos.resize(v.size(), {0, 0});
    forces.resize(v.size(), {0, 0});

    for (unsigned i = 0; i < v.size(); i++)
      pos[i] = {(std::rand() % (width / 6)) + (5 * width / 12),
                (std::rand() % (width / 6)) + (5 * width / 12)};

  } else {  // Assign positions radially based on frequencies
    double radius = width / 2;
    std::pair<double, double> center = {width / 2, width / 2};

    std::unordered_map<std::string, std::pair<double, double>> subjectAngles;
    double currAngle = 0;

    for (auto &subject : subjectFrequencies) {
      double nextAngle = currAngle + 2 * M_PI * subject.second;
      subjectAngles[subject.first] = std::make_pair(currAngle, nextAngle);
    }

    // seed the random number generator (was getting same output all the time)
    srand(time(NULL));
    int i = 0;

    for (Vertex &course : v) {
      std::string subjectName = getCourseSubject(course);
      std::pair<double, double> angleBounds = subjectAngles[subjectName];

      // get random polar coordinate within region
      double angle = fRand(angleBounds.first, angleBounds.second);
      double rad = fRand(0, radius);

      // Convert polar coordinate to rectangular and set point's position
      pos[i].first = rad * cos(angle) + center.first;
      pos[i].second = rad * sin(angle) + center.second;
      i++;
    }
  }

  return;
}

// Center all points and resize bounds to reduce wasted space in output image
void fdgOutput::recenterPts(int sideSpace) {
  double xLoc = 0, yLoc = 0, xMax = INT_MIN, xMin = INT_MAX, yMax = INT_MIN,
         yMin = INT_MAX;

  // Find the min and max x,y points
  for (unsigned i = 0; i < pos.size(); i++) {
    pos[i].first /= 10;
    pos[i].second /= 10;

    xLoc += pos[i].first;
    yLoc += pos[i].second;

    if (pos[i].first > xMax) xMax = pos[i].first;
    if (pos[i].first < xMin) xMin = pos[i].first;
    if (pos[i].second > yMax) yMax = pos[i].second;
    if (pos[i].second < yMin) yMin = pos[i].second;
  }

  // Add padding to x,y coordinates
  xMin -= sideSpace;
  xMax += sideSpace;
  yMin -= sideSpace;
  yMax += sideSpace;

  double xDiff = 0 - xMin, yDiff = 0 - yMin;

  // Reset point values based on min x and y
  for (unsigned i = 0; i < pos.size(); i++) {
    pos[i].first += xDiff;
    pos[i].second += yDiff;
  }

  // Reset class vars based on min, max x and y
  width = xMax - xMin;
  height = yMax - yMin;
  area = width * height;
}

// Serial version of finding locations to place verticies
// Iterations - recommend 1000
void fdgOutput::defineLocationsSerial(
    Graph graph, std::unordered_map<std::string, double> &subjectFrequencies, int sideSpace) {
  // set the hyperparams as a cpmpiler macro
  SET_FDG_ALGORITHM_HYPERPARAMETERS

  // Follows algorithm described on page 387 of 
  // http://cs.brown.edu/people/rtamassi/gdhandbook/chapters/force-directed.pdf
  // init vertices, edges, initial positions
  setVariables(graph, 10, subjectFrequencies, true);

  for (unsigned i = 0; i < iterations; i++) {
    if (i % 50 == 0) std::cout << "iteration: " << i << std::endl;
    // Update point positions based on the pseudocode and using params from
    // macro
    repulsionFunc(repulsiveForceConstant);
    springFunc(springConstant, springRestLength);
    centerFunc(centerConstant);
    updatePositions(deltaT, maxDisplacementSquared);
  }

  recenterPts(sideSpace);

  return;
}

// Parallel version of finding locations to place verticies
void fdgOutput::defineLocationsParallel(
    Graph graph, std::unordered_map<std::string, double> &subjectFrequencies, int sideSpace) {
  // set the hyperparams as a cpmpiler macro
  SET_FDG_ALGORITHM_HYPERPARAMETERS

  // init vertices, edges, initial positions
  setVariables(graph, 10, subjectFrequencies, true);
  // Follows algorithm described on page 387 of 
  // http://cs.brown.edu/people/rtamassi/gdhandbook/chapters/force-directed.pdf
  for (unsigned i = 0; i < iterations; i++) {
    if (i % 50 == 0) std::cout << "iteration: " << i << std::endl;

    // Update point positions based on the pseudocode and using params from macro
    // Initialize three threads to increase speed 30%
    std::thread repulsiveThread(&fdgOutput::repulsionFunc, this, repulsiveForceConstant);
    std::thread springThread(&fdgOutput::springFunc, this, springConstant, springRestLength);
    std::thread centeringThread(&fdgOutput::centerFunc, this, centerConstant);

    // launch threads
    repulsiveThread.join();
    springThread.join();
    centeringThread.join();

    // update point positions
    updatePositions(deltaT, maxDisplacementSquared);
    int counter = 0;
    if(i % 10 == 0) {
      cs225::PNG newImg = createOutputImage(subjectFrequencies);
      newImg.writeToFile("testOutputs/imgOut" + to_string(counter) + ".png");
      counter++;
    }
  }

  recenterPts(sideSpace);

  return;
}

// Helper function to update positions of verticies based on forces
void fdgOutput::updatePositions(double deltaT, int maxDisplacementSquared) {
  for (unsigned j = 0; j < v.size(); j++) {
    double deltaX = deltaT * forces[j].first,
           deltaY = deltaT * forces[j].second;
    double dispSq = deltaX * deltaX + deltaY * deltaY;

    if (dispSq > maxDisplacementSquared) {
      deltaX *= std::sqrt(maxDisplacementSquared / dispSq);
      deltaY *= std::sqrt(maxDisplacementSquared / dispSq);
    }

    pos[j].first += deltaX;
    pos[j].second += deltaY;
  }
}

// Helper function to move points more towards the center
void fdgOutput::centerFunc(double centerConstant) {
  for (unsigned j = 0; j < v.size(); j++) {
    double deltaX = pos[j].first - (width / 2),
           deltaY = pos[j].second - (width / 2);
    double dist = std::sqrt(deltaX * deltaX + deltaY * deltaY);

    if (dist != 0) {
      std::pair<double, double> f = {0, 0};
      double tempF = dist * centerConstant;
      f.first = tempF * deltaX / dist;
      f.second = tempF * deltaY / dist;

      // preevnt other functions from modifying forces vector concurrently
      mtx.lock();
      forces[j].first += f.first;
      forces[j].second += f.second;
      mtx.unlock();
    }
  }
}

// Helper function to find spring forces
void fdgOutput::springFunc(int springConstant, int springRestLength) {
  for (unsigned j = 0; j < v.size(); j++) {
    std::vector<Vertex> neighbors = g_.getAdjacent(v[j]);

    for (unsigned k = 0; k < neighbors.size(); k++) {
      int loc1 = std::find(v.begin(), v.end(), neighbors[k]) - v.begin();
      if (loc1 < 0 || loc1 > (int)v.size()) continue;
      double deltaX = pos[loc1].first - pos[j].first,
             deltaY = pos[loc1].second - pos[j].first;

      double dist = std::sqrt(deltaX * deltaX + deltaY * deltaY);

      if (dist != 0) {
        std::pair<double, double> f = {0, 0};
        double tempF = springConstant * (dist - springRestLength);
        f.first = tempF * deltaX / dist;
        f.second = tempF * deltaY / dist;

      // preevnt other functions from modifying forces vector concurrently
        mtx.lock();
        forces[j].first += f.first;
        forces[j].second += f.second;
        forces[loc1].first -= f.first;
        forces[loc1].second -= f.second;
        mtx.unlock();
      }
    }
  }

  return;
}

// Helper function to find repulsion forces
void fdgOutput::repulsionFunc(int repulsiveForceConstant) {
  for (unsigned j = 0; j < v.size(); j++) {
    for (unsigned k = j + 1; k < v.size(); k++) {
      double deltaX = pos[j].first - pos[k].first,
             deltaY = pos[j].second - pos[k].second;
      double dist = std::sqrt(deltaX * deltaX + deltaY * deltaY);
      std::pair<double, double> f = {0, 0};

      if (dist == 0) {
        f = {std::rand() % 2500, std::rand() % 2500};
      } else {
        double tempF = repulsiveForceConstant / (dist * dist);
        f.first = tempF * deltaX / dist;
        f.second = tempF * deltaY / dist;
      }

      // preevnt other functions from modifying forces vector concurrently
      mtx.lock();
      forces[j].first -= f.first;
      forces[j].second -= f.second;
      forces[k].first += f.first;
      forces[k].second += f.second;
      mtx.unlock();
    }
  }

  return;
}

// helper function to calculate if a point is within a radius
bool fdgOutput::calculateWithinRadius(int x, int y, int ctr_idx, int rad) {
  double x_pos = pos[ctr_idx].first + x;
  double y_pos = pos[ctr_idx].second + y;

  double x_ctr = pos[ctr_idx].first;
  double y_ctr = pos[ctr_idx].second;

  return pow(x_pos - x_ctr, 2) + pow(y_pos - y_ctr, 2) <= pow(rad, 2);
}

// Uses new locations to create output PNG using cs225's PNG class
cs225::PNG fdgOutput::createOutputImage(
    std::unordered_map<std::string, double> subjectFrequencies) {
  cs225::PNG out(width + 2, height + 2);


  // Draw edges
  for (unsigned i = 0; i < e.size(); i++) {
    // Find location of each source and edge in the vertices array
    auto temp1 = find(v.begin(), v.end(), e[i].source);
    auto temp2 = find(v.begin(), v.end(), e[i].dest);
    // Check to see if the vertices actually exist
    if (temp1 == v.end() || temp2 == v.end()) continue;
    std::pair<float, float> pt1 = pos[temp1 - v.begin()];
    std::pair<float, float> pt2 = pos[temp2 - v.begin()];

    // Calculate slope, y-int of the line connecting two vertices
    float slope = (pt1.second - pt2.second) / (pt1.first - pt2.first);
    float yIntercept = pt1.second - (slope * pt1.first);

    // Find department of course in the map
    cs225::HSLAPixel pixelColor(0, 0, .5);
    int end = std::max(pt1.first, pt2.first);
    int minYCoord = std::min(pt1.second, pt2.second);
    int maxYCoord = std::max(pt1.second, pt2.second);
    for (int x = std::min(pt1.first, pt2.first); x < end; x++) {
      int y = (slope * x) + yIntercept;
      // Check to see if x or y is out of bounds
      if (x < 0 || x > width + 1 || y < 0 || y > height + 1) continue;
      // out.getPixel(j, y).l = 0.6;

      // Set pixel value at point (dotted lines)
      out.getPixel(x, y) = pixelColor;
      // For steep slopes, draw a point at every (x, y + k) where k is the slope
      for (float k = 0; k < abs(slope); k += 1) {
        float incr = k * SIGN(slope);
        int newYCoord = y + ceil(incr);
        // check for overshooting or undershooting
        if (newYCoord < minYCoord || newYCoord > maxYCoord) break;
        out.getPixel(x, newYCoord) = pixelColor;
      }
    }
    // Data/uiuc-prerequisites-cs.csv

    // for (int x = pt2.first; x < pt1.first; x+= SIGN(slope)) {
    //   for (int y = pt2.second; y >= minYCoord && y <= maxYCoord; y +=
    //   ceil(slope)) {
    //     out.getPixel(x,y) = pixelColor;
    //   }

    // }
  }
  // Draw verticies
  for (unsigned i = 0; i < v.size(); i++) {
    cs225::HSLAPixel curr;
    if (cols.find(getCourseSubject(v[i])) == cols.end()) {
      // set pixel to black if not found
      curr = cs225::HSLAPixel(0, 0, 0);
      // Print vertices that were not found (error handling for color)
      // These are vertices that are not in the main dataset but are still
      // prerequisites for classes in the dataset
      // std::cout << v[i] << std::endl;
    } else {
      curr = cols.at(getCourseSubject(v[i]));
    }

    double rad = 4 * g_.getAdjacent(v[i]).size() + 2;

    for (int x = -1 * rad; x < rad; x++) {
      for (int y = -1 * rad; y < rad; y++) {
        if (calculateWithinRadius(x, y, i, rad)) {
          if ((unsigned)(pos[i].first + x) < out.width() &&
              (unsigned)(pos[i].second + y) < out.height())
            out.getPixel((int)pos[i].first + x, (int)pos[i].second + y) = curr;
        }
      }
    }
    colors.push_back({curr.h, curr.s, curr.l});
  }

  // out->getPixel(pos[i].first, pos[i].second).l = 0;

  return out;
}

// Helper function to get HSLAPixel with random color
cs225::HSLAPixel fdgOutput::getRandColor() {
  int hue = rand() % 360;
  // double f2 = (double)rand() / RAND_MAX;
  // double f3 = (double)rand() / RAND_MAX;
  cs225::HSLAPixel out(hue, 1, 0.5);

  return out;
}

// Debugging function used to print values within class vectors
void fdgOutput::printLocations() {
  std::ofstream outFile;
  outFile.open("outputImgInfo.txt");
  for (unsigned i = 0; i < v.size(); i++)
    outFile << "Vertex: " << v[i] << ", location: (" << pos[i].first << ", "
            << pos[i].second << ")"
            << ", color (HSL): (" << std::get<0>(colors[i]) << ", "
            << std::get<1>(colors[i]) << ", " << std::get<2>(colors[i])
            << ")\n";

  outFile << "width: " << width << " (size : " << area << ")\n";
  outFile << "vertex amount: " << v.size() << ", edge amount: " << e.size()
          << "\n";

  std::cout << "Wrote info to outputImgInfo.txt" << std::endl;

  return;
}

// parse out the course subject from a class
std::string fdgOutput::getCourseSubject(std::string course) {
  return course.substr(0, course.find(' '));
}

// get a random double within a range
double fdgOutput::fRand(double fMin, double fMax) {
  double f = (double)rand() / RAND_MAX;
  return fMin + f * (fMax - fMin);
}

void fdgOutput::initColorMap(std::unordered_map<std::string, double> &subjectFrequencies) {
  for (auto &it : subjectFrequencies) cols.insert({it.first, getRandColor()});
}
