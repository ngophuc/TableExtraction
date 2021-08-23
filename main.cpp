#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include <numeric>      // std::iota
#include <algorithm>    // std::sort, std::stable_sort

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "bsdetector.h"
#include "blurredsegment.h"

using namespace cv;
using namespace std;

#include "CLI11.hpp"

#define SAVE_FILE 1

std::vector<std::pair<Pt2i, Pt2i> >
FBSDDetector(const Mat& grayImg) {
  // Copy image
  int width = grayImg.cols;
  int height = grayImg.rows;
  int **tabImage = new int *[height];
  uchar c;
  for (int y = 0; y < height; y++) {
    tabImage[y] = new int[width];
    for (int x = 0; x < width; x++) {
      c = grayImg.at<uchar>(Point(x, y));
      tabImage[y][x] = int(c);
    }
  }
  // Generate grandient map for the dectetor
  VMap *gMap = NULL;
  if (gMap != NULL) delete gMap;
  // Create the gradient map
  gMap = new VMap(width, height, tabImage, VMap::TYPE_SOBEL_5X5);
  Mat gradImg(Size(width,height),CV_8UC1);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      gradImg.at<uchar>(Point(x, y)) = gMap->magn(x,y);
    }
  }
  // Create the FBSD detector
  BSDetector detector;
  detector.setGradientMap(gMap);
  detector.setAssignedThickness(1);
  // Call Fbsd detector
  detector.resetMaxDetections ();
  detector.detectAll();
  // Retrieve the detected blurred segments
  vector<BlurredSegment *> blurredSegments = detector.getBlurredSegments();
  
  std::vector<std::pair<Pt2i, Pt2i> > seg;
  for(int it=0; it<blurredSegments.size(); it++) {
    BlurredSegment * bs = blurredSegments.at(it);
    vector<Pt2i> pts = bs->getAllPoints();
    double den = double(pts.size())/sqrt(bs->getSquarredLength());
    if (den>0.9) {
      Pt2i lp = bs->getLastLeft();
      Pt2i rp = bs->getLastRight();
      seg.push_back(std::make_pair(lp, rp));
    }
  }
  return seg;
}

bool
isVerticalSegment(Pt2i p1, Pt2i p2, int tol = 4) {
  int b = abs(p2.x() - p1.x());
  return b < tol;
}

bool
isHorizontalSegment(Pt2i p1, Pt2i p2, int tol = 4) {
  int a = abs(p1.y() - p2.y());
  return a < tol;
}

std::vector<std::pair<Pt2i, Pt2i> >
HorizontalSegRecovery(const std::vector<std::pair<Pt2i, Pt2i> >& segH,
                      int tolAlign = 3,
                      int tolDistGr = 30,
                      int tolLen = 20) {
  // Sort horizontal segments
  std::vector<std::pair<Pt2i, Pt2i> > segHs = segH;
  std::sort(std::begin(segHs), std::end(segHs),
            [](std::pair<Pt2i, Pt2i> s1, std::pair<Pt2i, Pt2i> s2)
            { return s1.first.y() < s2.first.y() || (s1.first.y() == s2.first.y() && s1.first.x() < s2.first.x()); });
  
  //Extend horizontal segments
  std::vector<std::pair<Pt2i, Pt2i> > segHsE, segVsE;
  std::vector<std::vector<int> > idSegHsE, idSegVsE;
  int it1 = 0, it2 = 0;
  Pt2i lp1, rp1, lp2, rp2, lp3, rp3;
  int l = 0;
  while (it1 < segHs.size()-1) {
    std::vector<int> idSeg;
    lp1 = segHs.at(it1).first;
    rp1 = segHs.at(it1).second;
    l = abs(rp1.x() - lp1.x());
    lp3 = lp1;
    rp3 = rp1;
    it2 = it1 + 1;
    lp2 = segHs.at(it2).first;
    rp2 = segHs.at(it2).second;
    idSeg.push_back(it1);
    while (it2 < segHs.size()-1 && abs(lp1.y() - lp2.y()) < tolAlign) {
      if(lp3.x()>lp2.x())
        lp3 = lp2;
      if(rp3.x()<rp2.x())
        rp3 = rp2;
      l += abs(rp2.x() - lp2.x()) ;
      idSeg.push_back(it2);
      it2++;
      lp2 = segHs.at(it2).first;
      rp2 = segHs.at(it2).second;
    }
    segHsE.push_back(std::make_pair(lp3, rp3));
    it1 = it2;
    idSegHsE.push_back(idSeg);
  }
  if(it1 == segHs.size()-1) {
    lp1 = segHs.at(it1).first;
    rp1 = segHs.at(it1).second;
    segHsE.push_back(std::make_pair(lp1, rp1));
    idSegHsE.push_back(vector<int> (1,it1));
  }
  
  // Regroupe horizontal segments
  std::vector<std::pair<Pt2i, Pt2i> > segHsEg;
  for(int it=0; it<idSegHsE.size(); it++) {
    std::vector<int> idSeg = idSegHsE.at(it);
    std::vector<std::pair<Pt2i, Pt2i> > segEx;
    for(int it_bis=0; it_bis<idSeg.size(); it_bis++) {
      Pt2i lp = segHs.at(idSeg.at(it_bis)).first;
      Pt2i rp = segHs.at(idSeg.at(it_bis)).second;
      segEx.push_back(make_pair(lp, rp));
    }
    std::sort(std::begin(segEx), std::end(segEx),
              [](std::pair<Pt2i, Pt2i> s1, std::pair<Pt2i, Pt2i> s2) {return s1.first.x() < s2.first.x(); });
    // Regroup near horizontal segments
    int it1=0, it2=0;
    Pt2i lp1, rp1, lp2, rp2, lp3, rp3;
    int l = 0;
    while (it1 < segEx.size()-1) {
      lp1 = segEx.at(it1).first;
      rp1 = segEx.at(it1).second;
      l = abs(rp1.x() - lp1.x());
      lp3 = lp1;
      rp3 = rp1;
      it2 = it1 + 1;
      lp2 = segEx.at(it2).first;
      rp2 = segEx.at(it2).second;
      while (it2 < segEx.size()-1 && abs(rp3.x() - lp2.x()) < tolDistGr) {
        if(lp3.x()>lp2.x())
          lp3 = lp2;
        if(rp3.x()<rp2.x())
          rp3 = rp2;
        l += abs(rp2.x() - lp2.x()) ;
        it2++;
        lp2 = segEx.at(it2).first;
        rp2 = segEx.at(it2).second;
      }
      //last segment
      if(it2 == segEx.size()-1 && abs(rp3.x() - lp2.x()) < tolDistGr) {
        if(lp3.x()>lp2.x())
          lp3 = lp2;
        if(rp3.x()<rp2.x())
          rp3 = rp2;
        l += abs(rp2.x() - lp2.x()) ;
      }
      if(l > tolLen)
        segHsEg.push_back(std::make_pair(lp3, rp3));
      it1 = it2;
    }
    if(it1 == segEx.size()-1) {
      lp1 = segEx.at(it1).first;
      rp1 = segEx.at(it1).second;
      l = abs(rp1.x() - lp1.x());
      if(l > tolLen)
        segHsEg.push_back(std::make_pair(lp1, rp1));
    }
  }
  return segHsEg;
}

std::vector<std::pair<Pt2i, Pt2i> >
VerticalSegRecovery(const std::vector<std::pair<Pt2i, Pt2i> >& segV,
                    int tolAlign = 3,
                    int tolDistGr = 30,
                    int tolLen = 20) {
  // Sort vertial segments
  std::vector<std::pair<Pt2i, Pt2i> > segVs = segV;
  
  std::sort(std::begin(segVs), std::end(segVs),
            [](std::pair<Pt2i, Pt2i> s1, std::pair<Pt2i, Pt2i> s2) //{return s1.first.x() < s2.first.x(); });
            { return s1.first.x() < s2.first.x() || (s1.first.x() == s2.first.x() && s1.first.y() < s2.first.y()); });
  //Extend vertial segments
  std::vector<std::pair<Pt2i, Pt2i> > segVsE;
  std::vector<std::vector<int> > idSegVsE;
  int it1 = 0, it2 = 0;
  Pt2i lp1, rp1, lp2, rp2, lp3, rp3;
  int l = 0;
  while (it1 < segVs.size()-1) {
    std::vector<int> idSeg;
    lp1 = segVs.at(it1).first;
    rp1 = segVs.at(it1).second;
    l = abs(rp1.y() - lp1.y());
    lp3 = lp1;
    rp3 = rp1;
    it2 = it1 + 1;
    lp2 = segVs.at(it2).first;
    rp2 = segVs.at(it2).second;
    idSeg.push_back(it1);
    while (it2 < segVs.size()-1 && abs(lp1.x() - lp2.x()) < tolAlign) {
      if(lp3.y()>lp2.y())
        lp3 = lp2;
      if(rp3.y()<rp2.y())
        rp3 = rp2;
      l += abs(rp2.y() - lp2.y());
      idSeg.push_back(it2);
      it2++;
      lp2 = segVs.at(it2).first;
      rp2 = segVs.at(it2).second;
    }
    segVsE.push_back(std::make_pair(lp3, rp3));
    it1 = it2;
    idSegVsE.push_back(idSeg);
  }
  if(it1 == segVs.size()-1) {
    lp1 = segVs.at(it1).first;
    rp1 = segVs.at(it1).second;
    segVsE.push_back(std::make_pair(lp1, rp1));
    idSegVsE.push_back(vector<int> (1, it1));
  }
  // Regroupe vertical segments
  std::vector<std::pair<Pt2i, Pt2i> > segVsEg;
  for(int it=0; it<idSegVsE.size(); it++) {
    std::vector<int> idSeg = idSegVsE.at(it);
    std::vector<std::pair<Pt2i, Pt2i> > segEx;
    for(int it_bis=0; it_bis<idSeg.size(); it_bis++) {
      Pt2i lp = segVs.at(idSeg.at(it_bis)).first;
      Pt2i rp = segVs.at(idSeg.at(it_bis)).second;
      segEx.push_back(make_pair(lp, rp));
    }
    std::sort(std::begin(segEx), std::end(segEx),
              [](std::pair<Pt2i, Pt2i> s1, std::pair<Pt2i, Pt2i> s2) {return s1.first.y() < s2.first.y(); });
    // Regroup near vertical segments
    int it1=0, it2=0;
    Pt2i lp1, rp1, lp2, rp2, lp3, rp3;
    int l = 0;
    while (it1 < segEx.size()-1) {
      lp1 = segEx.at(it1).first;
      rp1 = segEx.at(it1).second;
      l = abs(rp1.y() - lp1.y());
      lp3 = lp1;
      rp3 = rp1;
      it2 = it1 + 1;
      lp2 = segEx.at(it2).first;
      rp2 = segEx.at(it2).second;
      while (it2 < segEx.size()-1 && abs(rp3.y() - lp2.y()) < tolDistGr) {
        if(lp3.y()>lp2.y())
          lp3 = lp2;
        if(rp3.y()<rp2.y())
          rp3 = rp2;
        l += abs(rp2.y() - lp2.y()) ;
        it2++;
        lp2 = segEx.at(it2).first;
        rp2 = segEx.at(it2).second;
      }
      //last segment
      if(it2 == segEx.size()-1 && abs(rp3.y() - lp2.y()) < tolDistGr) {
        if(lp3.y()>lp2.y())
          lp3 = lp2;
        if(rp3.y()<rp2.y())
          rp3 = rp2;
        l += abs(rp2.y() - lp2.y()) ;
      }
      if(l > tolLen) {
        segVsEg.push_back(std::make_pair(lp3, rp3));
      }
      it1 = it2;
    }
    if(it1 == segEx.size()-1) {
      lp1 = segEx.at(it1).first;
      rp1 = segEx.at(it1).second;
      l = abs(rp1.y() - lp1.y());
      if(l > tolLen) {
        segVsEg.push_back(std::make_pair(lp1, rp1));
      }
    }
  }
  return segVsEg;
}


bool
isHoziontalSegTab(const Mat& grayImg,
                  Pt2i p1, Pt2i p2,
                  int w = 3,
                  double ratio = 0.75,
                  int threshPic = 200,
                  int threshVal = 100) {
  int countProfil=0;
  int x = p1.x();
  int y = p1.y() - w;
  int width = abs(p2.x() - p1.x());
  int height = 2*w;
  //Create the rectangle
  cv::Rect roi(x, y, width, height);
  //Create the cv::Mat with the ROI you need, where "image" is the cv::Mat you want to extract the ROI from
  cv::Mat image_roi = grayImg(roi);
  //Count density (hight / low intensities) of roi
  int highInt = 0, lowInt = 0;
  uchar color;
  //std::vector<std::vector<int> > vecIntensity;
  countProfil = 0;
  for (int c = 0; c < width; c++) {
    std::vector<int> intensity;
    for (int l = 0; l < height; l++) {
      color = image_roi.at<uchar>(Point(c, l));
      intensity.push_back(int(color));
    }
    //vecIntensity.push_back(intensity);
    size_t pic1=0, pic2=intensity.size()-1;
    
    if(intensity[pic1]>threshPic || intensity[pic2]>threshPic) {
      for(size_t l=pic1; l<=pic2; l++) {
        if(abs(intensity[pic1]-intensity.at(l))>threshVal || abs(intensity[pic2]-intensity.at(l))>threshVal) {
          countProfil++;
          break;
        }
      }
    }
  }
  return countProfil>ratio*width;
}

bool
isVerticalSegTab(const Mat& grayImg,
                 Pt2i p1, Pt2i p2,
                 int w = 3,
                 double ratio = 0.75,
                 int threshPic = 200,
                 int threshVal = 100) {
  int countProfil=0;
  int x = p1.x() - w;
  int y = p1.y();
  int width = 2*w;
  int height = abs(p2.y() - p1.y());
  //Create the rectangle
  cv::Rect roi(x, y, width, height);
  //Create the cv::Mat with the ROI you need, where "image" is the cv::Mat you want to extract the ROI from
  cv::Mat image_roi = grayImg(roi);
  //Count density (hight / low intensities) of roi
  int highInt = 0, lowInt = 0;
  uchar color;
  countProfil = 0;
  for (int l = 0; l < height; l++) {
    std::vector<int> intensity;
    for (int c = 0; c < width; c++) {
      color = image_roi.at<uchar>(Point(c, l));
      intensity.push_back(int(color));
    }
    size_t pic1=0, pic2=intensity.size()-1;
    if(intensity[pic1]>threshPic || intensity[pic2]>threshPic) {
      for(size_t c=pic1; c<=pic2; c++) {
        if(abs(intensity[pic1]-intensity.at(c))>threshVal || abs(intensity[pic2]-intensity.at(c))>threshVal) {
          countProfil++;
          break;
        }
      }
    }
  }
  return countProfil>ratio*height;
}

vector<pair<Pt2i, Pt2i> >
getTableCells(const std::vector<std::pair<Pt2i, Pt2i> >& segH,
              const std::vector<std::pair<Pt2i, Pt2i> >& segV,
              int ext = 3) {
  vector<pair<Pt2i, Pt2i> > boxes;
  for(size_t it=0; it<segV.size(); it++) { //for each vertical segment
    std::pair<Pt2i, Pt2i> seg1 = segV.at(it);
    Pt2i lp1(seg1.first.x(), seg1.first.y()-ext);
    Pt2i rp1(seg1.second.x(), seg1.second.y()+ext);
    //find nearst horizontal segments that intersect the two vertical segment extremes
    int idFirst = -1, idLast = -1;
    int diffFirst = 0, diffLast = 0;
    for(size_t it_bis=0; it_bis<segH.size(); it_bis++) {
      std::pair<Pt2i, Pt2i> seg2 = segH.at(it_bis);
      Pt2i lp2(seg2.first.x()-ext, seg2.first.y());
      Pt2i rp2(seg2.second.x()+ext, seg2.second.y());
      if( (lp1.y()<=lp2.y() && lp2.y()<=rp1.y()) || (lp1.y()>=lp2.y() && lp2.y()>=rp1.y()) ) { // lp1.y < lp2.y < rp1.y
        if( (lp2.x()<=lp1.x() && lp1.x()<rp2.x()) || (lp2.x()>=lp1.x() && lp1.x()>=rp2.x()) ) { // lp2.x < lp1.x < rp2.x
          Pt2i pt (lp1.x(),lp2.y()); //intersection point
          if(idFirst==-1 && idLast==-1) { //first intersection found
            idFirst = it_bis;
            diffFirst = abs(pt.y()-seg1.first.y()); //to lp1
            idLast = it_bis;
            diffLast = abs(pt.y()-seg1.second.y()); //to rp1
          }
          else { //first the nearest segments
            if(abs(pt.y()-seg1.first.y()) < diffFirst) { //to lp1
              //cout<<"diffFirst="<<diffFirst<<" vs diff="<<abs(pt.y()-seg1.first.y())<<endl;
              idFirst = it_bis;
              diffFirst = abs(pt.y()-seg1.first.y());
            }
            if(abs(pt.y()-seg1.second.y()) < diffLast) { //to rp1
              //cout<<"diffLast="<<diffFirst<<" vs diff="<<abs(pt.y()-seg1.second.y())<<endl;
              idLast = it_bis;
              diffLast = abs(pt.y()-seg1.second.y());
            }
          }
        }
      }
    }
    //create the table cell
    if(idFirst!=-1 && idLast!=-1) {
      std::pair<Pt2i, Pt2i> seg2 = segH.at(idLast);
      Pt2i c1 (seg1.first.x(), seg1.first.y()); //lp1
      Pt2i c21 (seg2.second.x(), seg2.second.y()); //rp2
        boxes.push_back(make_pair(c1, c21)); //left cell
      Pt2i c22 (seg2.first.x(), seg2.first.y()); //lp2
        boxes.push_back(make_pair(c1, c22)); //right cell
    }
  }
  return boxes;
}

vector<pair<Pt2i, Pt2i> > getTables(Size imgSize, vector<pair<Pt2i, Pt2i> > cells, int minSize = 100) {
  Mat cellImage(imgSize,CV_8UC1, Scalar(0));
  for(int it=0; it<cells.size(); it++) {
    Pt2i p1 = cells.at(it).first;
    Pt2i p2 = cells.at(it).second;
    rectangle(cellImage, Point(p1.x(), p1.y()), Point(p2.x(), p2.y()), Scalar(255),-1);
  }
  //Compute connected components
  Mat labelImage;
  Mat stats;
  Mat centroids;
  int nLabels = connectedComponentsWithStats (cellImage, labelImage, stats, centroids, 4);
  //Get bounding box
  vector<pair<Pt2i, Pt2i> > boxes;
  int minWidth=imgSize.width/minSize;
  int minHeight=imgSize.height/minSize;
  for(int i=1; i<stats.rows; i++) { //label  0 is the background
    int x = stats.at<int>(Point(0, i));
    int y = stats.at<int>(Point(1, i));
    int w = stats.at<int>(Point(2, i));
    int h = stats.at<int>(Point(3, i));
    Pt2i p1(x,y);
    Pt2i p2(x+w,y+h);
    if(w>minWidth+1 && h>minHeight+1)
      boxes.push_back(make_pair(p1, p2));
  }
  return boxes;
}


int main(int argc, char *argv[]) {
  
  // parse command line using CLI ----------------------------------------------
  CLI::App app;
  string imgFileName, resFilename{"result.png"};
  int scale = 1;
  int win = 7;
  int tolAlign = 3;
  int tolDistGr = 30;
  int tolLen = 20;
  double ratio = 0.75;
  
  app.add_option("--input,-i,1", imgFileName, "Input filename.");
  app.add_option("--output,-o,2", resFilename, "Output filename (default = result.png)", true);
  app.add_option("--scale,-s", scale, "Scale factor (default = 1)", true);
  app.add_option("--window,-w", win, "Window size of intensity analysis (default = 7) ", true);
  app.add_option("--angle,-a", tolAlign, "Angle tolerance for horizontal and vertical segments (default = 5 degree)", true);
  app.add_option("--distance,-d", tolDistGr, "Max distance to regroupe the segments (default = 30)", true);
  app.add_option("--len,-l", tolLen, "Min length of segments (default = 20)", true);
  app.add_option("--ratio,-r", ratio, "Ratio for eliminating text segments (default = 0.75)", true);
  
  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------
  
  // Load image
  Mat img = imread(imgFileName, IMREAD_COLOR);
  int width = img.cols;
  int height = img.rows;
  if (width == 0 || height == 0) {
    cerr << "Couldn't open the " << imgFileName << " image file." << endl;
    exit (EXIT_FAILURE);
  }
  
  resize(img, img, Size(scale*width,scale*height),INTER_LINEAR);
  width = scale*width;
  height = scale*height;
  Mat grayImg;
  cvtColor(img, grayImg, COLOR_BGR2GRAY);
  
  // Step 1: Line segment detection using FBSD detector
  std::vector<std::pair<Pt2i, Pt2i> > seg = FBSDDetector(grayImg);
  
  //Step 2: Horizontal and vertical segment extraction
  std::vector<std::pair<Pt2i, Pt2i> > segH, segV;
  for(int it=0; it<seg.size(); it++) {
    Pt2i lp = seg.at(it).first;
    Pt2i rp = seg.at(it).second;
    if(isHorizontalSegment(lp, rp)) {
      if(lp.x()<rp.x()) //lp = left and rp = right
        segH.push_back(std::make_pair(lp, rp));
      else
        segH.push_back(std::make_pair(rp, lp));
    }
    if(isVerticalSegment(lp, rp)) {
      if(lp.y()<rp.y()) //lp = down and rp = up
        segV.push_back(std::make_pair(lp, rp));
      else
        segV.push_back(std::make_pair(rp, lp));
    }
  }
  
  //Step 3: Line segment recovery
  std::vector<std::pair<Pt2i, Pt2i> > segHsEg = HorizontalSegRecovery(segH,tolAlign,tolDistGr,tolLen);
  std::vector<std::pair<Pt2i, Pt2i> > segVsEg = VerticalSegRecovery(segV,tolAlign,tolDistGr,tolLen);
  
  //Step 4: Suppression of segments belonging to text
  std::vector<std::pair<Pt2i, Pt2i> > segHsEgT, segVsEgT;
  //Horizontal segments
  for(int it=0; it<segHsEg.size(); it++) {
    Pt2i lp = segHsEg.at(it).first;
    Pt2i rp = segHsEg.at(it).second;
    if(isHoziontalSegTab(grayImg, lp, rp, win/2, ratio))
      segHsEgT.push_back(make_pair(lp, rp));
  }
  //Vertical segments
  for(int it=0; it<segVsEg.size(); it++) {
    Pt2i lp = segVsEg.at(it).first;
    Pt2i rp = segVsEg.at(it).second;
    if(isVerticalSegTab(grayImg, lp, rp, win/2, ratio))
      segVsEgT.push_back(make_pair(lp, rp));
  }
  
  //Step 5: Table cell extraction
  vector<pair<Pt2i, Pt2i> > cells = getTableCells(segHsEgT, segVsEgT);
  
  //Step 6: Table reconstruction
  vector<pair<Pt2i, Pt2i> > tables = getTables(grayImg.size(), cells);
  
  //Display result
  cv::Mat mask(img.size(), CV_8UC3, cv::Scalar(0, 0, 0));
  for(int it=0; it<tables.size(); it++) {
    Pt2i p1 = tables.at(it).first;
    Pt2i p2 = tables.at(it).second;
    rectangle(mask, Point(p1.x(), p1.y()), Point(p2.x(), p2.y()), Scalar(255,255,255), -1);
  }
  double alpha = 0.5;
  Mat dst;
  cv::addWeighted(img, alpha, mask, 1.0 - alpha , 0.0, dst);
  resize(dst, dst, Size(width/scale,height/scale),INTER_LINEAR);
  imwrite(resFilename, dst);
  
  return EXIT_SUCCESS;
}
