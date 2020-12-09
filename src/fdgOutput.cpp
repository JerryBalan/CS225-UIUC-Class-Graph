#include "fdgOutput.h"

#include <math.h>

#include <fstream>
#include <iostream>
#include <mutex>

#include "structures/PNG.h"

std::mutex mtx;

fdgOutput::fdgOutput(int version, Graph graph, unsigned iterations, std::unordered_map<std::string, double> subjectFrequencies) {
  if (version == 0)
    // defineLocationsSerial(graph, subjectFrequencies, 5, iterations, graph.getVertices().size());
    defineLocationsSerial(graph, subjectFrequencies);
}

fdgOutput::fdgOutput(int version, Graph graph, int scale, unsigned iterations,
                     int classAmnt, std::unordered_map<std::string, double> subjectFrequencies) {
  if (version == 0)
    // defineLocationsSerial(graph, subjectFrequencies, scale, iterations, classAmnt);
    defineLocationsSerial(graph, subjectFrequencies);
}

fdgOutput::~fdgOutput() {}



// // works somewhat good
void fdgOutput::defineLocationsSerial(Graph graph, std::unordered_map<std::string, double> subjectFrequencies) {
  v = graph.getVertices();
  e = graph.getEdges();
  pos.clear();



  width = v.size() * 5;
  area = width * width;

  pos.resize(v.size(), {width / 2, width / 2});
  disp.resize(v.size(), {0, 0});



  // bool first = false, second = false, third = false, fourth = false;

  // for(unsigned i = 0; i < v.size(); i++) {
  //   if(!first && v[i].rfind("MATH 357", 0) == 0) {
  //     std::cout << "i: " << i << ", cl: " << v[i] << std::endl;
  //     first = true;
  //   } else if(!second && v[i].rfind("PHYS 419", 0) == 0) {
  //     std::cout << "i: " << i << ", cl: " << v[i] << std::endl;
  //     second = true;
  //   }

  //   if(first && second)
  //     break;
  // }

  // 0, 1, 3, 7 - MATH 574, CS 241, ECE 556, PHYS 222
  // CS 357 - 284, ECE510 - 188, MATH 357 - 42, PHYS 419 - 453
  // std::cout << v.size(); - 574


  // find the class with most pre-reqs in each subject inputted


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

  pos[42] = {500, 2500};
  pos[453] = {500, 500};
  pos[284] = {2000, 500};
  pos[188] = {2000, 2500};

  std::vector<int> dontDo = {42, 453, 284, 188};

  // for(unsigned i = 0; i < pos.size(); i++)
  //   std::cout << "(" << pos[i].first << ", " << pos[i].second << ")" << std::endl;


  while(dontDo.size() < v.size()) {
    for(unsigned i = 0; i < v.size(); i++) {
      if(std::find(dontDo.begin(), dontDo.end(), i) == dontDo.end()) {
        int newX = 0, newY = 0;
        int vAmnt = 0;
        bool didNotConverge = false;
        for(unsigned j = 0; j < e.size(); j++) {
          // get sum for x and y
          //std::cout << "curr " << v[i] << std::endl;

          if(e[j].source == v[i]) {
            // find x and y of source
            int loc = std::find(v.begin(), v.end(), e[j].dest) - v.begin();
            std::cout << "Curr is " << v[i] << ", loc is " << loc << ", dest is " << v[loc] << ", (" << pos[loc].first << ", " << pos[loc].second << ") - ";
            if(loc < 0 || (unsigned)loc > pos.size())
              continue;
            newX += pos[loc].first;
            newY += pos[loc].second;
            vAmnt++;

            std::cout << "(" << newX << ", " << newY << ")" << std::endl;
          }

          
        }

        //int vAmnt = graph.getAdjacent(v[i]).size();
        vAmnt = std::max(1, vAmnt);
        //std::cout << "newX: " << newX << " vAmnt : " << vAmnt;
        
        newX /= vAmnt;
        newY /= vAmnt;


        // if(newX <= 0 || newX >= width || newY <= 0 || newY >= width) {
        //   newX = width / 2 + std::rand() % (width / 4);
        //   newY = width / 2 + std::rand() % (width / 4);
        //   // dontDo.push_back(i);
        //   //didNotConverge = true;
        // }


        float dist = (pos[i].first - newX) * (pos[i].first - newX);
        dist += (pos[i].second - newY) * (pos[i].second - newY);
        dist = std::sqrt(dist);

        //std::cout << "HERE" << std::endl;





        pos[i].first = newX;
        pos[i].second = newY;

        // if(dist > 500)
        //   std::cout << "newX: " << newX << ", newY: " << newY << ", dist: " << dist << ", vamnt: " << vAmnt << std::endl;
        if(dist < 500)
          dontDo.push_back(i);
      }
    }

    std::cout << "v.size() = " << v.size() << ", dontDo.size() = " << dontDo.size() << std::endl;
  }



}











// void fdgOutput::defineLocationsSerial(Graph graph, std::unordered_map<std::string, double> subjectFrequencies) {
//   v = graph.getVertices();
//   e = graph.getEdges();

//   pos.clear();



//   width = v.size() * 5;
//   area = width * width;

//   pos.resize(v.size(), {width / 2, width / 2});
//   disp.resize(v.size(), {0, 0});



//   // bool first = false, second = false, third = false, fourth = false;

//   // for(unsigned i = 0; i < v.size(); i++) {
//   //   if(!first && v[i].rfind("MATH 357", 0) == 0) {
//   //     std::cout << "i: " << i << ", cl: " << v[i] << std::endl;
//   //     first = true;
//   //   } else if(!second && v[i].rfind("PHYS 419", 0) == 0) {
//   //     std::cout << "i: " << i << ", cl: " << v[i] << std::endl;
//   //     second = true;
//   //   }

//   //   if(first && second)
//   //     break;
//   // }

//   // 0, 1, 3, 7 - MATH 574, CS 241, ECE 556, PHYS 222
//   // CS 357 - 284, ECE510 - 188, MATH 357 - 42, PHYS 419 - 453
//   // std::cout << v.size(); - 574


//   // find the class with most pre-reqs in each subject inputted


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

//   pos[42] = {500, 2500};
//   pos[453] = {500, 500};
//   pos[284] = {2000, 500};
//   pos[188] = {2000, 2500};

//   std::vector<int> dontDo = {42, 453, 284, 188};

//   // for(unsigned i = 0; i < pos.size(); i++)
//   //   std::cout << "(" << pos[i].first << ", " << pos[i].second << ")" << std::endl;


//   while(dontDo.size() != v.size()) {
//     for(unsigned i = 0; i < v.size(); i++) {
//       if(std::find(dontDo.begin(), dontDo.end(), i) == dontDo.end()) {
//         float newX = 0, newY = 0;
//         for(unsigned j = 0; j < e.size(); j++) {
//           // get sum for x and y
//           if(e[j].dest == v[i]) {
//             // find x and y of source
//             int loc = std::find(v.begin(), v.end(), e[j].source) - v.begin();
//             std::cout << "Dest is " << e[j].dest << ", loc is " << loc << ", source is " << v[loc] << ", (" << pos[loc].first << ", " << pos[loc].second << ") - "; 
//             newX += pos[loc].first;
//             newY += pos[loc].second;

//             std::cout << "(" << newX << ", " << newY << ")" << std::endl;
//           }
//         }

//         int vAmnt = graph.getAdjacent(v[i]).size();
//         vAmnt = std::max(1, vAmnt);
//         //std::cout << "newX: " << newX << " vAmnt : " << vAmnt;
//         newX /= vAmnt;
//         newY /= vAmnt;

//         //std::cout << ", updated: " << newX << std::endl;

//         // if((newX == 0 && newY == 0) || (newX == width && newY == width) || (newX == 0 && newY == width) || (newX == width && newY == 0)) {
//         //   newX = width / 2;
//         //   newY = width / 2;
//         // }

//         if(newX <= 0 || newX >= width || newY <= 0 || newY == width) {
//           newX = width / 2;
//           newY = width / 2;
//         }

        


//         // if(newX > 0 && newX < width && newY > 0 && newY < width) {
//         //   int p = 0;
//         //   p++;
//         // } else {
//         //   while(newX > width)
//         //     newX = (int)newX % width;
//         //   while(newY > width)
//         //     newY = (int)newY % width;

//         //   while(newX < 0)
//         //     newX += width;
//         //   while(newY < 0)
//         //     newY += width;
//         // }


//         float dist = (pos[i].first - newX) * (pos[i].first - newX);
//         dist += (pos[i].second - newY) * (pos[i].second - newY);
//         dist = std::sqrt(dist);





//         pos[i].first = newX;
//         pos[i].second = newY;

//         //std::cout << "newX: " << newX << ", newY: " << newY << ", dist: " << dist << std::endl;
//         if(dist < 10)
//           dontDo.push_back(i);
//       }
//     }

//     std::cout << "v.size() = " << v.size() << ", dontDo.size() = " << dontDo.size() << std::endl;
//   }



// }


// void fdgOutput::defineLocationsSerial(Graph graph) {
//   unsigned iterations = 20;

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

//   float k = std::sqrt(area / v.size());
//   float t = width / 2;

//   for(unsigned i = 1; i < iterations; i++) {
//     std::cout << "iteration " << i << std::endl;
//     // repulsion
//     for(unsigned vit = 0; vit < v.size(); vit++) {
//       disp[vit] = {0, 0};

//       for(unsigned u = 0; u < v.size(); u++) {
//         if(u != vit) {
//           float deltaX = pos[vit].first - pos[u].first;
//           float deltaY = pos[vit].second - pos[u].first;
//           disp[vit].first += (deltaX / std::abs(deltaX)) * fT(k, std::abs(deltaX));
//           disp[vit].second += (deltaY / std::abs(deltaY)) * fT(k, std::abs(deltaY));
//         }
//       }
//     }

//     // attractive
//     for(unsigned eit = 0; eit < e.size(); eit++) {
//       float loc1 = find(v.begin(), v.end(), e[eit].source) - v.begin();
//       float loc2 = find(v.begin(), v.end(), e[eit].dest) - v.begin();

//       float deltaX = disp[loc1].first - disp[loc2].first;
//       float deltaY = disp[loc1].second - disp[loc2].second;

//       disp[loc1].first -= (deltaX / std::abs(deltaX)) * fA(k, std::abs(deltaX));
//       disp[loc1].second -= (deltaY / std::abs(deltaY)) * fA(k, std::abs(deltaY));

//       disp[loc2].first += (deltaX / std::abs(deltaX)) * fA(k, std::abs(deltaX));
//       disp[loc2].second += (deltaY / std::abs(deltaY)) * fA(k, std::abs(deltaY));
//     }

//     // set points
//     for(unsigned vit = 0; vit < v.size(); vit++) {
//       pos[vit].first += (disp[vit].first / std::abs(disp[vit].first)) * std::min(disp[vit].first, t);
//       pos[vit].second += (disp[vit].second / std::abs(disp[vit].second)) * std::min(disp[vit].second, t);

//       pos[vit].first = std::min((float)width, std::max((float)0, pos[vit].first));
//       pos[vit].second = std::min((float)width, std::max((float)0, pos[vit].second));
//     }

//     t *= 0.85;
//   }

// }

float fdgOutput::fT(float k, float x) {
  return k * k / x;
}

float fdgOutput::fA(float k, float x) {
  return x * x / k;
}

// Uses new locations to create output PNG using cs225's PNG class
cs225::PNG fdgOutput::createOutputImage(std::unordered_map<std::string, double> subjectFrequencies) {
  cs225::PNG out(width + 2, width + 2);

  // Draw verticies
  for (unsigned i = 0; i < v.size(); i++) {
    // Square of size 3x3 instead of single pixel
    cs225::HSLAPixel curr(0, 0, 0);
    // colors.push_back({curr.h, curr.s, curr.l});
    // for (int j = -1; j < 2; j++) {
    //   for (int k = -1; k < 2; k++) {
    //     int x = std::max(0, (int)pos[i].first + j);
    //     int y = std::max(0, (int)pos[i].second + k);
    //     out.getPixel(x, y) = curr;
    //   }
    // }
    int x = pos[i].first, y = pos[i].second;
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

    //out.getPixel(pos[i].first, pos[i].second).l = 0;
  }

  std::unordered_map<std::string, cs225::HSLAPixel> cols;
//   for(auto it : subjectFrequencies)
//       cols.insert({it.first, getRandColor()});
  cols["MATH"] = cs225::HSLAPixel(0, 1, .5); // red
  cols["PHYS"] = cs225::HSLAPixel(72, 1, .5); // yellow
  cols["CS"] = cs225::HSLAPixel(144, 1, .5); // green
  cols["ECE"] = cs225::HSLAPixel(216, 1, .5); // blue

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
            << std::endl;

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