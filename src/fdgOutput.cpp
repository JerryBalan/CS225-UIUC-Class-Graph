#include "fdgOutput.h"

#include <math.h>

#include <fstream>
#include <iostream>
#include <mutex>
#include <cassert>

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

  // double radius = width / 2;
  // std::pair<double, double> center = {width / 2, width / 2};

  // std::unordered_map<std::string, std::pair<double, double>> subjectAngles;
  // double currAngle = 0;
  // for (auto& subject : subjectFrequencies) {
  //   double nextAngle = currAngle + 2 * M_PI * subject.second;
  //   subjectAngles[subject.first] = std::make_pair(currAngle, nextAngle);
  // }
  // srand(time(NULL));
  // int i = 0;
  // for (Vertex& course : v) {
  //   std::string subjectName = getCourseSubject(course);
  //   std::pair<double, double> angleBounds = subjectAngles[subjectName];
  //   double angle = fRand(angleBounds.first, angleBounds.second);
  //   double rad = fRand(0, radius);

  //   pos[i].first = rad * cos(angle) + center.first;
  //   pos[i].second = rad * sin(angle) + center.second;
  //   i++;
  // }

  // rand location
  for(unsigned i = 0; i < v.size(); i++) {
    pos[i].first = std::rand() % width;
    pos[i].second = std::rand() % width;
  }


  return;
}

// Serial version of finding locations to place verticies
// void fdgOutput::defineLocationsSerial(
//     Graph graph, std::unordered_map<std::string, double> &subjectFrequencies,
//     int scale, unsigned iterations, int classAmnt) {
//   setVariables(graph, classAmnt, scale, subjectFrequencies);
//   float t = area, K = std::sqrt(area / v.size());

//   if (iterations < 0 || iterations > v.size()) iterations = v.size();

// //   double radius = width / 2;
// //   std::pair<double, double> center = {width / 2, width / 2};

// //   std::unordered_map<std::string, std::pair<double, double>> subjectAngles;
// //   double currAngle = 0;
// //   for (auto& subject : subjectFrequencies) {
// //     double nextAngle = currAngle + 2 * M_PI * subject.second;
// //     subjectAngles[subject.first] = std::make_pair(currAngle, nextAngle);
// //   }
// //   srand(time(NULL));
// //   int i = 0;
// //   for (Vertex& course : v) {
// //     std::string subjectName = getCourseSubject(course);
// //     std::pair<double, double> angleBounds = subjectAngles[subjectName];
// //     double angle = fRand(angleBounds.first, angleBounds.second);
// //     double rad = fRand(0, radius);

// //     pos[i].first = rad * cos(angle) + center.first;
// //     pos[i].second = rad * sin(angle) + center.second;
// //     i++;
// //   }

//   for (unsigned i = 0; i < iterations; i++) {
//     std::cout << "iteration " << i << std::endl;
//     // Repulsion forces
//     for (unsigned j = 0; j < v.size(); j++) {
//       disp[j] = {0, 0};

//       for (unsigned k = 0; k < v.size(); k++) {
//         float d = pos[j].first - pos[k].first;
//         float d2 = pos[j].second - pos[k].second;
//         float dist = std::max((float)0.001, std::sqrt(d * d + d2 * d2));
//         float aForce = K * K / dist / v.size() / 100.0;
//         disp[j].first += d / dist * aForce;
//         disp[j].second += d2 / dist * aForce;
//       }
//     }

//     // Attractive forces
//     for (unsigned j = 0; j < e.size(); j++) {
//       auto temp1 = find(v.begin(), v.end(), e[j].source);
//       auto temp2 = find(v.begin(), v.end(), e[j].dest);
//       if (temp1 == v.end() || temp2 == v.end()) continue;
//       float loc1 = temp1 - v.begin();
//       float loc2 = temp2 - v.begin();

//       float x = pos[i].first - pos[loc1].first;
//       float y = pos[i].second - pos[loc2].second;

//       float dist = std::max((float)0.001, std::sqrt(x * x + y * y));
//       float aForce = dist * dist / K / v.size();

//       disp[i].first -= x / dist * aForce;
//       disp[i].second += y / dist * aForce;
//     }

//     // Change position values based on displacement
//     for (unsigned j = 0; j < v.size(); j++) {
//       float d = std::sqrt(disp[j].first * disp[j].first +
//                           disp[j].second * disp[j].second);
//       pos[j].first += d > t ? disp[j].first / d * t : disp[j].first;
//       pos[j].second += d > t ? disp[j].second / d * t : disp[j].second;

//       pos[j].first = std::min((float)width, std::max((float)0, pos[j].first));
//       pos[j].second = std::min((float)width, std::max((float)0, pos[j].second));

//       // if ((pos[j].first == 0 || pos[j].second == 0) ||
//       //     (pos[j].first == width || pos[j].second == width)) {
//       //   pos[i].first = std::rand() % width;
//       //   pos[i].second = std::rand() % width;
//       // }
//     }

//     t *= 0.99;
//   }

//   return;
// }


// best working
// void fdgOutput::defineLocationsSerial(Graph graph, std::unordered_map<std::string, double> &subjectFrequencies, int scale, unsigned iterations, int classAmnt) {
//   iterations = 100;

//   v = graph.getVertices();
//   e = graph.getEdges();
//   width = v.size() * 5;
//   area = width * width;

//   pos.resize(v.size(), {0, 0});
//   disp.resize(v.size(), {0, 0});


//   // rand location
//   for(unsigned i = 0; i < v.size(); i++) {
//     pos[i].first = std::rand() % width;
//     pos[i].second = std::rand() % width;
//   }

//   double k = std::sqrt(area / v.size());
//   double t = width / 2;

//   for(unsigned i = 1; i < iterations; i++) {
//     if(i % 100 == 0)
//       std::cout << "iteration " << i << std::endl;
//     // repulsion
//     for(unsigned vit = 0; vit < v.size(); vit++) {
//       disp[vit] = {0, 0};

//       for(unsigned u = 0; u < v.size(); u++) {
//         if(u != vit) {
//           double deltaX = pos[vit].first - pos[u].first;
//           double deltaY = pos[vit].second - pos[u].second;


//           if(deltaX == 0 && deltaY == 0) {
//             pos[vit] = {std::rand() % width, std::rand() % width};
//             deltaX = pos[vit].first - pos[u].first;
//             deltaY = pos[vit].second - pos[u].second;
//           }


//           double dist = std::sqrt(deltaX * deltaX + deltaY * deltaY);
//           //std::cout << "deltaX: " << deltaX << ", deltaY: " << deltaY << ", dist: " << dist << std::endl;
//           //std::cout << " fT(k, dist): " << fT(k, dist) << ", deltaX: " << deltaX << ", deltaY: " << deltaY << ", dist: " << dist << std::endl;

//           // disp[vit].first += (deltaX / std::abs(deltaX + 0.001)) * fT(k, std::abs(deltaX + 0.001));
//           // disp[vit].second += (deltaY / std::abs(deltaY + 0.001)) * fT(k, std::abs(deltaY + 0.001));

//           disp[vit].first += (deltaX / dist) * fT(k, dist);
//           disp[vit].second += (deltaY / dist) * fT(k, dist);

//           assert(!isnan(disp[vit].first) && !isnan(disp[vit].second));

//           //std::cout << "disp[vit].first: " << disp[vit].first << ", disp[vit].second: " << disp[vit].second << std::endl;
//         }
//       }
//     }


//     // attractive
//     for(unsigned eit = 0; eit < e.size(); eit++) {
//       int loc1 = find(v.begin(), v.end(), e[eit].source) - v.begin();
//       int loc2 = find(v.begin(), v.end(), e[eit].dest) - v.begin();

//       //std::cout << "loc1: " << loc1 << ", loc2: " << loc2 << ", disp.size(): " << disp.size() << std::endl;

//       double deltaX = pos[loc1].first - pos[loc2].first;
//       double deltaY = pos[loc1].second - pos[loc2].second;

//       if(deltaX == 0 && deltaY == 0) {
//         pos[loc1] = {std::rand() % width, std::rand() % width};
//         deltaX = pos[loc1].first - pos[loc2].first;
//         deltaY = pos[loc1].second - pos[loc2].second;
//       }




//       double dist = std::sqrt(deltaX * deltaX + deltaY * deltaY);

//       // if(dist == 0)
//       //   continue;

//       //std::cout << "disp[loc1].first: " << disp[loc1].first << ", disp[loc1].second: " << disp[loc1].second << ", disp[loc2].first: " << disp[loc2].first << ", disp[loc2].second: " << disp[loc2].second << std::endl;

//       // double subValX = (deltaX / std::abs(deltaX + 0.001)) * fA(k, std::abs(deltaX + 0.001)), subValY = (deltaY / std::abs(deltaY + 0.001)) * fA(k, std::abs(deltaY + 0.001));
//       //assert(!isnan(subValX));
//       //std::cout << "deltaX: " << deltaX << ", fA: " << fA(k, std::abs(deltaX)) << ", deltaY: " << deltaY << ", fA: " << fA(k, std::abs(deltaY)) << std::endl;

//       disp[loc1].first -= (deltaX / dist) * fA(k, dist);
//       disp[loc1].second -= (deltaY / dist) * fA(k, dist);

//       disp[loc2].first += (deltaX / dist) * fA(k, dist);
//       disp[loc2].second += (deltaY / dist) * fA(k, dist);

//       //std::cout << "DeltaX: " << deltaX << ", deltaY: " << deltaY << ", disp[loc1].first: " << disp[loc1].first << ", disp[loc1].second: " << disp[loc1].second << std::endl;
//     }

//     // set points
//     for(unsigned vit = 0; vit < v.size(); vit++) {
//       double dist = std::sqrt((pos[vit].first * pos[vit].first) + (pos[vit].second * pos[vit].second));  
//       pos[vit].first += (disp[vit].first / dist) * std::min(disp[vit].first, t);
//       pos[vit].second += (disp[vit].second / dist) * std::min(disp[vit].second, t);

//       pos[vit].first = std::min((double)width, std::max((double)0, pos[vit].first));
//       pos[vit].second = std::min((double)width, std::max((double)0, pos[vit].second));
//     }

//     t *= 0.98;
//   }

//   // for(unsigned i = 0; i < pos.size(); i++) {
//   //   std::cout << "(" << pos[i].first << ", " << pos[i].second << ")" << std::endl;
//   // }

// }

// double fdgOutput::fT(double k, double x) {
//   return (k * k) / x;
// }

// double fdgOutput::fA(double k, double x) {
//   return (x * x) / k;
// }


void fdgOutput::defineLocationsSerial(Graph graph, std::unordered_map<std::string, double> &subjectFrequencies, int scale, unsigned iterations, int classAmnt) {
  iterations = 1500;
  int springRestLength = 400, repulsiveForceConstant = 1000, attractionConstant = 8000, springConstant = 50, maxDisplacementSquared = 400;
  double deltaT = 0.0003, centerConstant = 0.6;

  v = graph.getVertices();
  e = graph.getEdges();
  width = v.size() * 10;
  area = width * width;

  pos.resize(v.size(), {0, 0});
  forces.resize(v.size(), {0, 0});

  for(unsigned i = 0; i < v.size(); i++)
    pos[i] = {(std::rand() % (width / 6) ) + (5 * width / 12), (std::rand() % (width / 6) ) + (5 * width / 12)};

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

    // for(unsigned j = 0; j < v.size(); j++) {
    //   std::cout << "forcesX: " << forces[j].first << ", forcesY: " << forces[j].second << std::endl;
    // }


    // Attraction forces
    // for(unsigned j = 0; j < v.size(); j++) {
    //   std::vector<Vertex> neighbors = graph.getAdjacent(v[j]);

    //   for(unsigned k = 0; k < neighbors.size(); k++) {
    //     if(j < k) {
    //       int loc1 = std::find(v.begin(), v.end(), neighbors[k]) - v.begin();
    //       double deltaX = pos[loc1].first - pos[j].first, deltaY = pos[loc1].second - pos[j].first;

    //       double dist = std::sqrt(deltaX * deltaX + deltaY * deltaY);

    //       if(dist != 0) {
    //         std::pair<double, double> f = {0, 0};
    //         double tempF = attractionConstant * dist;
    //         f.first = tempF * deltaX / dist;
    //         f.second = tempF * deltaY / dist;

    //         forces[j].first += f.first;
    //         forces[j].second += f.second;
    //         forces[loc1].first -= f.first;
    //         forces[loc1].second -= f.second;
    //       }
    //     }
    //   }

    // }

    // Spring force
    for(unsigned j = 0; j < v.size(); j++) {
      std::vector<Vertex> neighbors = graph.getAdjacent(v[j]);

      for(unsigned k = 0; k < neighbors.size(); k++) {
        //if(j < k) {
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
        //}
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

  for(unsigned i = 0; i < pos.size(); i++) {
    pos[i].first /= 10;
    pos[i].second /= 10;
  }


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

      pos[j].first = std::min((double)width, std::max((double)0, pos[j].first));
      pos[j].second = std::min((double)width, std::max((double)0, pos[j].second));

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
    // for (int j = -1; j < 2; j++) {
    //   for (int k = -1; k < 2; k++) {
    //     int x = std::max(0, (int)pos[i].first + j);
    //     int y = std::max(0, (int)pos[i].second + k);
    //     out.getPixel(x, y) = curr;
    //   }
    // }

    //out.getPixel(pos[i].first, pos[i].second).l = 0;
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
