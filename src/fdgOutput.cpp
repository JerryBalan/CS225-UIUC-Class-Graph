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
void fdgOutput::setVariables(Graph graph, int classAmnt, int scale, std::unordered_map<std::string, double> &subjectFrequencies) {
  v = graph.getVertices();
  e = graph.getEdges();
  v.resize(classAmnt);

  disp.clear();
  pos.clear();
  disp.resize(v.size(), {0, 0});
  pos.resize(v.size(), {0, 0});

  width = v.size() * scale;
  area = width * width;

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

  return;
}

// Serial version of finding locations to place verticies
void fdgOutput::defineLocationsSerial(
    Graph graph, std::unordered_map<std::string, double> &subjectFrequencies,
    int scale, unsigned iterations, int classAmnt) {
  setVariables(graph, classAmnt, scale, subjectFrequencies);
  float t = area, K = std::sqrt(area / v.size());

  if (iterations < 0 || iterations > v.size()) iterations = v.size();

//   double radius = width / 2;
//   std::pair<double, double> center = {width / 2, width / 2};

//   std::unordered_map<std::string, std::pair<double, double>> subjectAngles;
//   double currAngle = 0;
//   for (auto& subject : subjectFrequencies) {
//     double nextAngle = currAngle + 2 * M_PI * subject.second;
//     subjectAngles[subject.first] = std::make_pair(currAngle, nextAngle);
//   }
//   srand(time(NULL));
//   int i = 0;
//   for (Vertex& course : v) {
//     std::string subjectName = getCourseSubject(course);
//     std::pair<double, double> angleBounds = subjectAngles[subjectName];
//     double angle = fRand(angleBounds.first, angleBounds.second);
//     double rad = fRand(0, radius);

//     pos[i].first = rad * cos(angle) + center.first;
//     pos[i].second = rad * sin(angle) + center.second;
//     i++;
//   }

  for (unsigned i = 0; i < iterations; i++) {
    // Repulsion forces
    for (unsigned j = 0; j < v.size(); j++) {
      disp[j] = {0, 0};

      for (unsigned k = 0; k < v.size(); k++) {
        float d = pos[j].first - pos[k].first;
        float d2 = pos[j].second - pos[k].second;
        float dist = std::max((float)0.001, std::sqrt(d * d + d2 * d2));
        float aForce = K * K / dist / v.size() / 100.0;
        disp[j].first += d / dist * aForce;
        disp[j].second += d2 / dist * aForce;
      }
    }

    // Attractive forces
    for (unsigned j = 0; j < e.size(); j++) {
      auto temp1 = find(v.begin(), v.end(), e[j].source);
      auto temp2 = find(v.begin(), v.end(), e[j].dest);
      if (temp1 == v.end() || temp2 == v.end()) continue;
      float loc1 = temp1 - v.begin();
      float loc2 = temp2 - v.begin();

      float x = pos[i].first - pos[loc1].first;
      float y = pos[i].second - pos[loc2].second;

      float dist = std::max((float)0.001, std::sqrt(x * x + y * y));
      float aForce = dist * dist / K / v.size();

      disp[i].first -= x / dist * aForce;
      disp[i].second += y / dist * aForce;
    }

    // Change position values based on displacement
    for (unsigned j = 0; j < v.size(); j++) {
      float d = std::sqrt(disp[j].first * disp[j].first +
                          disp[j].second * disp[j].second);
      pos[j].first += d > t ? disp[j].first / d * t : disp[j].first;
      pos[j].second += d > t ? disp[j].second / d * t : disp[j].second;

      pos[j].first = std::min((float)width, std::max((float)0, pos[j].first));
      pos[j].second = std::min((float)width, std::max((float)0, pos[j].second));

      if ((pos[j].first == 0 || pos[j].second == 0) ||
          (pos[j].first == width || pos[j].second == width)) {
        pos[i].first = std::rand() % width;
        pos[i].second = std::rand() % width;
      }
    }

    t *= 0.99;
  }

  return;
}

// Parallel version of finding locations to place verticies
void fdgOutput::defineLocationsParallel(Graph graph, std::unordered_map<std::string, double> &subjectFrequencies,
                                int scale, unsigned iterations, int classAmnt) {
  setVariables(graph, classAmnt, scale, subjectFrequencies);
  float t = area, K = std::sqrt(area / v.size());

  if (iterations < 0 || iterations > v.size()) iterations = v.size();

  for (unsigned i = 0; i < iterations; i++) {
    // Run threads on attractive and repulsion functions
    std::thread th1(&fdgOutput::attractiveFunc, this, i);
    std::thread th2(&fdgOutput::repulsionFunc, this, i);
    th1.join();
    th2.join();

    // Change position values based on displacement
    for (unsigned j = 0; j < v.size(); j++) {
      float d = std::sqrt(disp[j].first * disp[j].first +
                          disp[j].second * disp[j].second);
      pos[j].first += d > t ? disp[j].first / d * t : disp[j].first;
      pos[j].second += d > t ? disp[j].second / d * t : disp[j].second;

      pos[j].first = std::min((float)width, std::max((float)0, pos[j].first));
      pos[j].second = std::min((float)width, std::max((float)0, pos[j].second));

      if ((pos[j].first == 0 || pos[j].second == 0) ||
          (pos[j].first == width || pos[j].second == width)) {
        pos[i].first = std::rand() % width;
        pos[i].second = std::rand() % width;
      }
    }

    t *= 0.99;
  }

  return;
}

// Helper function to find attractive forces for parallel version
void fdgOutput::attractiveFunc(int i) {
  // Attractive forces
  float K = std::sqrt(area / v.size());
  for (unsigned j = 0; j < e.size(); j++) {
    auto temp1 = find(v.begin(), v.end(), e[j].source);
    auto temp2 = find(v.begin(), v.end(), e[j].dest);
    if (temp1 == v.end() || temp2 == v.end()) continue;
    float loc1 = temp1 - v.begin();
    float loc2 = temp2 - v.begin();

    float x = pos[i].first - pos[loc1].first;
    float y = pos[i].second - pos[loc2].second;

    float dist = std::max((float)0.001, std::sqrt(x * x + y * y));
    float aForce = dist * dist / K / v.size();
    mtx.lock();
    disp[i].first -= x / dist * aForce;
    disp[i].second += y / dist * aForce;
    mtx.unlock();
  }
}

// Helper function to find repulsion forces for parallel version
void fdgOutput::repulsionFunc(int i) {
  // Repulsion forces
  float K = std::sqrt(area / v.size());
  for (unsigned j = 0; j < v.size(); j++) {
    disp[j] = {0, 0};

    for (unsigned k = 0; k < v.size(); k++) {
      float d = pos[j].first - pos[k].first;
      float d2 = pos[j].second - pos[k].second;
      float dist = std::max((float)0.001, std::sqrt(d * d + d2 * d2));
      float aForce = K * K / dist / v.size() / 100.0;
      mtx.lock();
      disp[j].first += d / dist * aForce;
      disp[j].second += d2 / dist * aForce;
      mtx.unlock();
    }
  }
}

// Uses new locations to create output PNG using cs225's PNG class
cs225::PNG fdgOutput::createOutputImage(std::unordered_map<std::string, double> subjectFrequencies) {
  cs225::PNG out(width + 2, width + 2);

  // Draw verticies
  for (unsigned i = 0; i < v.size(); i++) {
    // Square of size 3x3 instead of single pixel
    cs225::HSLAPixel curr = getRandColor();
    colors.push_back({curr.h, curr.s, curr.l});
    for (int j = -1; j < 2; j++) {
      for (int k = -1; k < 2; k++) {
        int x = std::max(0, (int)pos[i].first + j);
        int y = std::max(0, (int)pos[i].second + k);
        out.getPixel(x, y) = curr;
      }
    }

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