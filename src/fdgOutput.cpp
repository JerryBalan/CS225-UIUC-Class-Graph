#include "fdgOutput.h"

#include <math.h>

#include <fstream>
#include <iostream>
#include <mutex>

#include "structures/PNG.h"

std::mutex mtx;

fdgOutput::fdgOutput(int version, Graph graph, unsigned iterations, std::unordered_map<std::string, double> subjectFrequencies) {
  if (version == 0)
    defineLocationsSerial(graph, subjectFrequencies, 5, iterations, graph.getVertices().size());
  else
    defineLocationsParallel(graph, subjectFrequencies, 5, iterations, graph.getVertices().size());
}

fdgOutput::fdgOutput(int version, Graph graph, int scale, unsigned iterations,
                     int classAmnt, std::unordered_map<std::string, double> subjectFrequencies) {
  if (version == 0)
    defineLocationsSerial(graph, subjectFrequencies, scale, iterations, classAmnt);
  else
    defineLocationsParallel(graph, subjectFrequencies, scale, iterations, classAmnt);
}

fdgOutput::~fdgOutput() {}

// Helper function to set variables since multiple methods use it
void fdgOutput::setVariables(Graph graph, int scale, std::unordered_map<std::string, double> &subjectFrequencies, bool setCompletlyRandom) {
  v = graph.getVertices();
  e = graph.getEdges();

  width = v.size() * scale;
  area = width * width;

  if(setCompletlyRandom) {
    pos.resize(v.size(), {0, 0});
    forces.resize(v.size(), {0, 0});
    
    for(unsigned i = 0; i < v.size(); i++)
      pos[i] = {(std::rand() % (width / 6) ) + (5 * width / 12), (std::rand() % (width / 6) ) + (5 * width / 12)};
  } else {
    double radius = width / 2;
    std::pair<double, double> center = {width / 2, width / 2};

    std::unordered_map<std::string, std::pair<double, double>> subjectAngles;
    double currAngle = 0;

    for (auto& subject : subjectFrequencies) {
      double nextAngle = currAngle + 2 * M_PI * subject.second;
      subjectAngles[subject.first] = std::make_pair(currAngle, nextAngle);
    }

    srand(time(NULL));
    int i = 0;

    for (Vertex& course : v) {
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

// Serial version of finding locations to place verticies
// Iterations - recommend 1000
void fdgOutput::defineLocationsSerial(Graph graph, std::unordered_map<std::string, double> &subjectFrequencies, int scale, unsigned iterations, int classAmnt) {
  int springRestLength = 400, repulsiveForceConstant = 1000, attractionConstant = 8000, springConstant = 50, maxDisplacementSquared = 400;
  double deltaT = 0.0003, centerConstant = 5;

  setVariables(graph, 10, subjectFrequencies, true);

  for(unsigned i = 0; i < iterations; i++) {
    if(i % 25 == 0)
      std::cout << "iteration: " << i << std::endl;
    
    // Repulsion force
    for(unsigned j = 0; j < v.size(); j++) {
      for(unsigned k = j + 1; k < v.size(); k++) {
        double deltaX = pos[j].first - pos[k].first, deltaY = pos[j].second - pos[k].second;
        double dist = std::sqrt(deltaX * deltaX + deltaY * deltaY);
        std::pair<double, double> f = {0, 0};

        if(dist == 0) {
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
    for(unsigned j = 0; j < v.size(); j++) {
      std::vector<Vertex> neighbors = graph.getAdjacent(v[j]);

      for(unsigned k = 0; k < neighbors.size(); k++) {
        int loc1 = std::find(v.begin(), v.end(), neighbors[k]) - v.begin();
        if(loc1 < 0 || loc1 > (int)v.size())
          continue;
        double deltaX = pos[loc1].first - pos[j].first, deltaY = pos[loc1].second - pos[j].first;

        double dist = std::sqrt(deltaX * deltaX + deltaY * deltaY);

        if(dist != 0) {
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
    for(unsigned j = 0; j < v.size(); j++) {
      double deltaX = pos[j].first - (width / 2), deltaY = pos[j].second - (width / 2);
      double dist = std::sqrt(deltaX * deltaX + deltaY * deltaY);

      if(dist != 0) {
        std::pair<double, double> f = {0, 0};
        double tempF = dist * centerConstant;
        f.first = tempF * deltaX / dist;
        f.second = tempF * deltaY / dist;

        forces[j].first += f.first;
        forces[j].second += f.second;
      }
    }

    // Update positions
    for(unsigned j = 0; j < v.size(); j++) {
      double deltaX = deltaT * forces[j].first, deltaY = deltaT * forces[j].second;
      double dispSq = deltaX * deltaX + deltaY * deltaY;

      if(dispSq > maxDisplacementSquared) {
        deltaX *= std::sqrt(maxDisplacementSquared / dispSq);
        deltaY *= std::sqrt(maxDisplacementSquared / dispSq);
      }

      pos[j].first += deltaX;
      pos[j].second += deltaY;
    }
  }

  // Center all points
  float xLoc = 0, yLoc = 0;
  for(unsigned i = 0; i < pos.size(); i++) {
    pos[i].first /= 10;
    pos[i].second /= 10;

    xLoc += pos[i].first;
    yLoc += pos[i].second;
  }

  xLoc /= pos.size();
  yLoc /= pos.size();
  xLoc = (width / 2) - xLoc;
  yLoc = (width / 2) - yLoc;

  for(unsigned i = 0; i < pos.size(); i++) {
    pos[i].first += xLoc;
    pos[i].second += yLoc;
  }

  return;
}

// Parallel version of finding locations to place verticies
void fdgOutput::defineLocationsParallel(Graph graph, std::unordered_map<std::string, double> &subjectFrequencies,
                                int scale, unsigned iterations, int classAmnt) {
  int springRestLength = 400, repulsiveForceConstant = 1000, attractionConstant = 8000, springConstant = 50, maxDisplacementSquared = 400;
  double deltaT = 0.0003, centerConstant = 5;

  setVariables(graph, 10, subjectFrequencies, true);

  for(unsigned i = 0; i < iterations; i++) {
    if(i % 25 == 0)
      std::cout << "iteration: " << i << std::endl;

    std::thread th1(&fdgOutput::repulsionFunc, this, repulsiveForceConstant);
    std::thread th2(&fdgOutput::springFunc, this, graph, springConstant, springRestLength);
    std::thread th3(&fdgOutput::centerFunc, this, centerConstant);
    th1.join();
    th2.join();
    th3.join();

    // Update positions
    for(unsigned j = 0; j < v.size(); j++) {
      double deltaX = deltaT * forces[j].first, deltaY = deltaT * forces[j].second;
      double dispSq = deltaX * deltaX + deltaY * deltaY;

      if(dispSq > maxDisplacementSquared) {
        deltaX *= std::sqrt(maxDisplacementSquared / dispSq);
        deltaY *= std::sqrt(maxDisplacementSquared / dispSq);
      }

      pos[j].first += deltaX;
      pos[j].second += deltaY;
    }
  }

  // Center all points
  float xLoc = 0, yLoc = 0;
  for(unsigned i = 0; i < pos.size(); i++) {
    pos[i].first /= 10;
    pos[i].second /= 10;

    xLoc += pos[i].first;
    yLoc += pos[i].second;
  }

  xLoc /= pos.size();
  yLoc /= pos.size();
  xLoc = (width / 2) - xLoc;
  yLoc = (width / 2) - yLoc;

  for(unsigned i = 0; i < pos.size(); i++) {
    pos[i].first += xLoc;
    pos[i].second += yLoc;
  }

  return;
}

void fdgOutput::centerFunc(double centerConstant) {
    for(unsigned j = 0; j < v.size(); j++) {
    double deltaX = pos[j].first - (width / 2), deltaY = pos[j].second - (width / 2);
    double dist = std::sqrt(deltaX * deltaX + deltaY * deltaY);

    if(dist != 0) {
      std::pair<double, double> f = {0, 0};
      double tempF = dist * centerConstant;
      f.first = tempF * deltaX / dist;
      f.second = tempF * deltaY / dist;

      forces[j].first += f.first;
      forces[j].second += f.second;
    }
  }
}

// Helper function to find spring forces for parallel version
void fdgOutput::springFunc(Graph graph, int springConstant, int springRestLength) {
  for(unsigned j = 0; j < v.size(); j++) {
    std::vector<Vertex> neighbors = graph.getAdjacent(v[j]);

    for(unsigned k = 0; k < neighbors.size(); k++) {
      int loc1 = std::find(v.begin(), v.end(), neighbors[k]) - v.begin();
      if(loc1 < 0 || loc1 > (int)v.size())
        continue;
      double deltaX = pos[loc1].first - pos[j].first, deltaY = pos[loc1].second - pos[j].first;

      double dist = std::sqrt(deltaX * deltaX + deltaY * deltaY);

      if(dist != 0) {
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
  for(unsigned j = 0; j < v.size(); j++) {
    for(unsigned k = j + 1; k < v.size(); k++) {
      double deltaX = pos[j].first - pos[k].first, deltaY = pos[j].second - pos[k].second;
      double dist = std::sqrt(deltaX * deltaX + deltaY * deltaY);
      std::pair<double, double> f = {0, 0};

      if(dist == 0) {
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

// Uses new locations to create output PNG using cs225's PNG class
cs225::PNG fdgOutput::createOutputImage(std::unordered_map<std::string, double> subjectFrequencies) {
  cs225::PNG out(width + 2, width + 2);

  // Draw verticies
  for (unsigned i = 0; i < v.size(); i++) {
    // Square of size 3x3 instead of single pixel
    // cs225::HSLAPixel curr = getRandColor();
    // colors.push_back({curr.h, curr.s, curr.l});
    // for (int j = -1; j < 2; j++) {
    //   for (int k = -1; k < 2; k++) {
    //     int x = std::max(0, (int)pos[i].first + j);
    //     int y = std::max(0, (int)pos[i].second + k);
    //     out.getPixel(x, y) = curr;
    //   }
    // }

    cs225::HSLAPixel curr(0, 0, 0); //= getRandColor();
    int x= pos[i].first, y = pos[i].second;
    out.getPixel(x, y) = curr;
    out.getPixel(x+1,y)= curr;
    out.getPixel(x+2,y)= curr;
    out.getPixel(x+3,y)= curr;
    out.getPixel(x-1,y)= curr;
    out.getPixel(x-2,y)= curr;
    out.getPixel(x,y+1)= curr;
    out.getPixel(x,y+2)= curr;
    out.getPixel(x,y+3)= curr;
    out.getPixel(x,y-1)= curr;
    out.getPixel(x,y-2)= curr;
    colors.push_back({curr.h, curr.s, curr.l});

    // out->getPixel(pos[i].first, pos[i].second).l = 0;
  }

  std::unordered_map<std::string, cs225::HSLAPixel> cols;
//   for(auto it : subjectFrequencies)
//       cols.insert({it.first, getRandColor()});
  cols["CS"] = cs225::HSLAPixel(0, 1, .5); // red
  cols["ECE"] = cs225::HSLAPixel(72, 1, .5); // yellow
  cols["PHYS"] = cs225::HSLAPixel(144, 1, .5); // green
  cols["MATH"] = cs225::HSLAPixel(216, 1, .5); // blue

  // Draw edges
  for (unsigned i = 0; i < e.size(); i++) {
    auto temp1 = find(v.begin(), v.end(), e[i].source);
    auto temp2 = find(v.begin(), v.end(), e[i].dest);
    if (temp1 == v.end() || temp2 == v.end()) continue;
    std::pair<float, float> pt1 = pos[temp1 - v.begin()];
    std::pair<float, float> pt2 = pos[temp2 - v.begin()];

    float slope = (pt1.second - pt2.second) / (pt1.first - pt2.first);
    float yIntercept = pt1.second - (slope * pt1.first);

    int end = std::max(pt1.first, pt2.first);
    for (int j = std::min(pt1.first, pt2.first); j < end; j++) {
      int y = (slope * j) + yIntercept;
      if (j < 0 || j > width + 2 || y < 0 || y > width + 2) continue;
      //out.getPixel(j, y).l = 0.6;
      out.getPixel(j, y) = cols.at(v[temp1 - v.begin()].substr(0, v[temp1 - v.begin()].find(' ')));
    }
  }

  return out;
}

// Helper function to get HSLAPixel with random color
cs225::HSLAPixel fdgOutput::getRandColor() {
  double f = (double)rand() / RAND_MAX;
  double f2 = (double)rand() / RAND_MAX;
  double f3 = (double)rand() / RAND_MAX;
  cs225::HSLAPixel out(f, f2, f3);

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