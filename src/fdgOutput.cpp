#include "fdgOutput.h"

#include <math.h>

#include <fstream>
#include <iostream>
#include <mutex>

#include "structures/PNG.h"

#define SIGN(x) ((signbit(x) ? -1 : 1))
#define SET_FDG_ALGORITHM_HYPERPARAMETERS                   \
  int springRestLength = 400, repulsiveForceConstant = 100, \
      attractionConstant = 8000, springConstant = 20,       \
      maxDisplacementSquared = 100;                         \
  double deltaT = 0.0003, centerConstant = 7.5;

std::mutex mtx;

fdgOutput::fdgOutput(
    int version, Graph graph, unsigned iterations, int sideSpace,
    std::unordered_map<std::string, double> subjectFrequencies) {
  if (version == 0)
    defineLocationsSerial(graph, subjectFrequencies, iterations, sideSpace);
  else
    defineLocationsParallel(graph, subjectFrequencies, iterations, sideSpace);
}

fdgOutput::~fdgOutput() {}

// Helper function to set variables since multiple methods use it
void fdgOutput::setVariables(
    Graph graph, int scale,
    std::unordered_map<std::string, double> &subjectFrequencies,
    bool setCompletlyRandom) {
  v = graph.getVertices();
  e = graph.getEdges();

  width = v.size() * scale;
  height = width;
  area = width * height;
  g_ = graph;

  if (setCompletlyRandom) {
    pos.resize(v.size(), {0, 0});
    forces.resize(v.size(), {0, 0});

    for (unsigned i = 0; i < v.size(); i++)
      pos[i] = {(std::rand() % (width / 6)) + (5 * width / 12),
                (std::rand() % (width / 6)) + (5 * width / 12)};
  } else {
    double radius = width / 2;
    std::pair<double, double> center = {width / 2, width / 2};

    std::unordered_map<std::string, std::pair<double, double>> subjectAngles;
    double currAngle = 0;

    for (auto &subject : subjectFrequencies) {
      double nextAngle = currAngle + 2 * M_PI * subject.second;
      subjectAngles[subject.first] = std::make_pair(currAngle, nextAngle);
    }

    srand(time(NULL));
    int i = 0;

    for (Vertex &course : v) {
      std::string subjectName = getCourseSubject(course);
      std::pair<double, double> angleBounds = subjectAngles[subjectName];
      double angle = fRand(angleBounds.first, angleBounds.second);
      double rad = fRand(0, radius);

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

  xMin -= sideSpace;
  xMax += sideSpace;
  yMin -= sideSpace;
  yMax += sideSpace;

  double xDiff = 0 - xMin, yDiff = 0 - yMin;

  for (unsigned i = 0; i < pos.size(); i++) {
    pos[i].first += xDiff;
    pos[i].second += yDiff;
  }

  width = xMax - xMin;
  height = yMax - yMin;
  area = width * height;
}

// Serial version of finding locations to place verticies
// Iterations - recommend 1000
void fdgOutput::defineLocationsSerial(
    Graph graph, std::unordered_map<std::string, double> &subjectFrequencies,
    unsigned iterations, int sideSpace) {
  // set the hyperparams as a cpmpiler macro
  SET_FDG_ALGORITHM_HYPERPARAMETERS

  // init vertices, edges, initial positions

  setVariables(graph, 10, subjectFrequencies, true);

  for (unsigned i = 0; i < iterations; i++) {
    if (i % 50 == 0) std::cout << "iteration: " << i << std::endl;

    // Repulsion force
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

        forces[j].first -= f.first;
        forces[j].second -= f.second;
        forces[k].first += f.first;
        forces[k].second += f.second;
      }
    }

    // Spring force
    for (unsigned j = 0; j < v.size(); j++) {
      std::vector<Vertex> neighbors = graph.getAdjacent(v[j]);

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

          forces[j].first += f.first;
          forces[j].second += f.second;
          forces[loc1].first -= f.first;
          forces[loc1].second -= f.second;
        }
      }
    }

    // Center force
    for (unsigned j = 0; j < v.size(); j++) {
      double deltaX = pos[j].first - (width / 2),
             deltaY = pos[j].second - (width / 2);
      double dist = std::sqrt(deltaX * deltaX + deltaY * deltaY);

      if (dist != 0) {
        std::pair<double, double> f = {0, 0};
        double tempF = dist * centerConstant;
        f.first = tempF * deltaX / dist;
        f.second = tempF * deltaY / dist;

        forces[j].first += f.first;
        forces[j].second += f.second;
      }
    }

    // Update positions
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

  recenterPts(sideSpace);

  return;
}

// Parallel version of finding locations to place verticies
void fdgOutput::defineLocationsParallel(
    Graph graph, std::unordered_map<std::string, double> &subjectFrequencies,
    unsigned iterations, int sideSpace) {
  // set the hyperparams as a cpmpiler macro
  SET_FDG_ALGORITHM_HYPERPARAMETERS

  // init vertices, edges, initial positions
  setVariables(graph, 10, subjectFrequencies, true);

  for (unsigned i = 0; i < iterations; i++) {
    if (i % 50 == 0) std::cout << "iteration: " << i << std::endl;

    std::thread th1(&fdgOutput::repulsionFunc, this, repulsiveForceConstant);
    std::thread th2(&fdgOutput::springFunc, this, graph, springConstant,
                    springRestLength);
    std::thread th3(&fdgOutput::centerFunc, this, centerConstant);
    th1.join();
    th2.join();
    th3.join();

    // Update positions
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

  recenterPts(sideSpace);

  return;
}

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

      mtx.lock();
      forces[j].first += f.first;
      forces[j].second += f.second;
      mtx.unlock();
    }
  }
}

// Helper function to find spring forces for parallel version
void fdgOutput::springFunc(Graph graph, int springConstant,
                           int springRestLength) {
  for (unsigned j = 0; j < v.size(); j++) {
    std::vector<Vertex> neighbors = graph.getAdjacent(v[j]);

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

// Helper function to find repulsion forces for parallel version
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

  std::unordered_map<std::string, cs225::HSLAPixel> cols;
  for (auto it : subjectFrequencies) cols.insert({it.first, getRandColor()});

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

    // calculate the x-value of the end of the line
    int end = std::max(pt1.first, pt2.first);

    for (int x = std::min(pt1.first, pt2.first); x < end; x++) {
      int y = (slope * x) + yIntercept;
      // Check to see if x or y is out of bounds
      if (x < 0 || x > width + 1 || y < 0 || y > height + 1) continue;
      // out.getPixel(j, y).l = 0.6;

      // Set pixel value at point (dotted lines)
      // out.getPixel(x,y) = pixelColor;
      // For steep slopes, draw a point at every (x, y + k) where k is the slope
      for (int k = 0; k < (int)abs(slope); k += 2) {
        int incr = k * SIGN(slope);
        // check for overshooting or undershooting
        if (y + incr < pt1.second || y + incr > pt2.second) break;
        out.getPixel(x, y + incr) = pixelColor;
      }
    }
  }
  // Draw verticies
  for (unsigned i = 0; i < v.size(); i++) {
    cs225::HSLAPixel curr;
    if (cols.find(getCourseSubject(v[i])) == cols.end()) {
      curr = cs225::HSLAPixel(0, 0, 0);
      // Print vertices that were not found
      std::cout << v[i] << std::endl;

    } else {
      curr =
          cols.at(getCourseSubject(v[i]));  //= getRandColor();
    }

    double rad = g_.getAdjacent(v[i]).size() + 2;

    for (int x = -1 * rad; x < rad; x++) {
      for (int y = -1 * rad; y < rad; y++) {
        if (calculateWithinRadius(x, y, i, rad)) {
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
  double f2 = (double)rand() / RAND_MAX;
  double f3 = (double)rand() / RAND_MAX;
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

std::string fdgOutput::getCourseSubject(std::string course) {
  return course.substr(0, course.find(' '));
}
double fdgOutput::fRand(double fMin, double fMax) {
  double f = (double)rand() / RAND_MAX;
  return fMin + f * (fMax - fMin);
}