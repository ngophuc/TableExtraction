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

bool isVerticalSegment(Pt2i p1, Pt2i p2, int tol = 4) {
  //int a = p1.y() - p2.y();
  int b = abs(p2.x() - p1.x());
  //int c = p1.x()*p2.y() - p2.x()*p1.y();
  return b < tol;
}

bool isHorizontalSegment(Pt2i p1, Pt2i p2, int tol = 4) {
  int a = abs(p1.y() - p2.y());
  //int b = p2.x() - p1.x();
  //int c = p1.x()*p2.y() - p2.x()*p1.y();
  return a < tol;
}

template <typename T>
vector<size_t> sort_indexes(const vector<T> &v) {

  // initialize original index locations
  vector<size_t> idx(v.size());
  iota(idx.begin(), idx.end(), 0);

  // sort indexes based on comparing values in v
  // using std::stable_sort instead of std::sort
  // to avoid unnecessary index re-orderings
  // when v contains elements of equal values
  //stable_sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});
  stable_sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) {return v[i1] > v[i2];});
  return idx;
}
/*
bool isHoziontalSegTab(Mat gradImg, Pt2i p1, Pt2i p2, int w = 5, int threshPic = 200, int threshVal = 180, double ratio = 0.9) {
  int countProfil=0;
  int x = p1.x();
  int y = p1.y() - w;
  int width = abs(p2.x() - p1.x());
  int height = 2*w;
  //Create the rectangle
  cv::Rect roi(x, y, width, height);
  //Create the cv::Mat with the ROI you need, where "image" is the cv::Mat you want to extract the ROI from
  cv::Mat image_roi = gradImg(roi);
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
    vector<size_t> indexIntensity = sort_indexes(intensity);
    size_t pic1, pic2;
    if(indexIntensity[0]<indexIntensity[1]) {
      pic1 = indexIntensity[0];
      pic2 = indexIntensity[1];
    }
    else {
      pic2 = indexIntensity[0];
      pic1 = indexIntensity[1];
    }
    //cout<<"pic1="<<pic1<<" and pic2="<<pic2<<", intensity[pic1]="<<int(intensity[pic1])<<" and intensity[pic2]="<<int(intensity[pic2])<<endl;
    if(intensity[pic1]>threshPic && intensity[pic2]>threshPic) {
      for(size_t l=pic1; l<=pic2; l++) {
        if(intensity.at(l)<threshVal)
          countProfil++;
      }
    }
  }
  return countProfil>ratio*width;
}
*/
bool isHoziontalSegTab(Mat grayImg, Pt2i p1, Pt2i p2, int w = 3, int threshPic = 200, int threshVal = 100, double ratio = 0.75) {
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
    //vector<size_t> indexIntensity = sort_indexes(intensity);
    size_t pic1=0, pic2=intensity.size()-1;
    /*
    if(intensity[pic1]>threshPic && intensity[pic2]>threshPic) {
      for(size_t l=pic1; l<=pic2; l++) {
        if(intensity.at(l)<threshVal)
          countProfil++;
      }
    }
    */
    if(intensity[pic1]>threshPic || intensity[pic2]>threshPic) {
      //for(size_t l=pic1+w/2; l<=pic2-w/2; l++) {
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
/*
bool isVerticalSegTab(Mat gradImg, Pt2i p1, Pt2i p2, int w = 5, int threshPic = 200, int threshVal = 180, double ratio = 0.9) {
  int countProfil=0;
  int countPic=0;
  int x = p1.x() - w;
  int y = p1.y();
  int width = 2*w;
  int height = abs(p2.y() - p1.y());
  //Create the rectangle
  cv::Rect roi(x, y, width, height);
  //Create the cv::Mat with the ROI you need, where "image" is the cv::Mat you want to extract the ROI from
  cv::Mat image_roi = gradImg(roi);
  //Count density (hight / low intensities) of roi
  int highInt = 0, lowInt = 0;
  uchar color;
  //std::vector<std::vector<int> > vecIntensity;
  countProfil = 0;
  for (int l = 0; l < height; l++) {
    std::vector<int> intensity;
    for (int c = 0; c < width; c++) {
      color = image_roi.at<uchar>(Point(c, l));
      intensity.push_back(int(color));
    }
    //vecIntensity.push_back(intensity);
    vector<size_t> indexIntensity = sort_indexes(intensity);
    while(countPic<intensity.size() && intensity[indexIntensity[countPic]]>threshPic)
      countPic++;
    //cout<<"countPic="<<countPic<<" vs w="<<w<<endl;
    if(countPic==0)
      return false;
    
    size_t pic1, pic2;
    if(indexIntensity[0]<indexIntensity[1]) {
      pic1 = indexIntensity[0];
      pic2 = indexIntensity[1];
    }
    else {
      pic2 = indexIntensity[0];
      pic1 = indexIntensity[1];
    }
    //cout<<"pic1="<<pic1<<" and pic2="<<pic2<<", intensity[pic1]="<<int(intensity[pic1])<<" and intensity[pic2]="<<int(intensity[pic2])<<endl;
    if(intensity[pic1]>threshPic && intensity[pic2]>threshPic) {
      for(size_t c=pic1; c<=pic2; c++) {
        if(intensity.at(c)<threshVal)
          countProfil++;
      }
    }
  }
  return countProfil>ratio*height;
}
*/

bool isVerticalSegTab(Mat grayImg, Pt2i p1, Pt2i p2, int w = 3, int threshPic = 200, int threshVal = 100, double ratio = 0.75) {
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
  //std::vector<std::vector<int> > vecIntensity;
  countProfil = 0;
  for (int l = 0; l < height; l++) {
    std::vector<int> intensity;
    for (int c = 0; c < width; c++) {
      color = image_roi.at<uchar>(Point(c, l));
      intensity.push_back(int(color));
    }
    //vecIntensity.push_back(intensity);
    //vector<size_t> indexIntensity = sort_indexes(intensity);
    size_t pic1=0, pic2=intensity.size()-1;
    /*
    if(intensity[pic1]>threshPic && intensity[pic2]>threshPic) {
      for(size_t c=pic1; c<=pic2; c++) {
        if(intensity.at(c)<threshVal)
          countProfil++;
      }
    }
    */
    if(intensity[pic1]>threshPic || intensity[pic2]>threshPic) {
      //for(size_t c=pic1+w/2; c<=pic2-w/2; c++) {
      for(size_t c=pic1; c<=pic2; c++) {
        if(abs(intensity[pic1]-intensity.at(c))>threshVal || abs(intensity[pic2]-intensity.at(c))>threshVal) {
          countProfil++;
          break;
        }
      }
    }
  }
  /*
  Mat tImg = grayImg.clone();
  line(tImg, Point(p1.x(), p1.y()), Point(p2.x(), p2.y()), Scalar(120),5);
  imshow("image", tImg);
  cout<<"countProfil="<<countProfil<<" and ratio*height="<<ratio*height<<endl;
  waitKey();
  */
  return countProfil>ratio*height;
}

vector<vector<Pt2i> > getHorizontalIntersection(std::vector<std::pair<Pt2i, Pt2i> > segH, std::vector<std::pair<Pt2i, Pt2i> > segV, int ext = 3) {
  vector<vector<Pt2i> > ptsIntH;
  for(size_t it=0; it<segH.size(); it++) {
    vector<Pt2i> ptsInt;
    std::pair<Pt2i, Pt2i> seg1 = segH.at(it);
    Pt2i lp1(seg1.first.x()-ext, seg1.first.y());
    Pt2i rp1(seg1.second.x()+ext, seg1.second.y());
    for(size_t it_bis=0; it_bis<segV.size(); it_bis++) {
      std::pair<Pt2i, Pt2i> seg2 = segV.at(it_bis);
      Pt2i lp2(seg2.first.x(), seg2.first.y()-ext);
      Pt2i rp2(seg2.second.x(), seg2.second.y()+ext);
      if( (lp2.y()<=lp1.y() && lp1.y()<=rp2.y()) || (lp2.y()>=lp1.y() && lp1.y()>=rp2.y()) ) { // lp2.y < lp1.y < rp2.y
        if( (lp1.x()<=lp2.x() && lp2.x()<rp1.x()) || (lp1.x()>=lp2.x() && lp2.x()>=rp1.x()) ) { // lp1.x < lp2.x < rp1.x
          Pt2i pt (lp2.x(),lp1.y());
          ptsInt.push_back(pt);
        }
      }
    }
    ptsIntH.push_back(ptsInt);
  }
  return ptsIntH;
}

vector<vector<Pt2i> > getVerticalIntersection(std::vector<std::pair<Pt2i, Pt2i> > segH, std::vector<std::pair<Pt2i, Pt2i> > segV, int ext = 3) {
  vector<vector<Pt2i> > ptsIntV;
  for(size_t it=0; it<segV.size(); it++) {
    vector<Pt2i> ptsInt;
    std::pair<Pt2i, Pt2i> seg1 = segV.at(it);
    Pt2i lp1(seg1.first.x(), seg1.first.y()-ext);
    Pt2i rp1(seg1.second.x(), seg1.second.y()+ext);
    for(size_t it_bis=0; it_bis<segH.size(); it_bis++) {
      std::pair<Pt2i, Pt2i> seg2 = segH.at(it_bis);
      Pt2i lp2(seg2.first.x()-ext, seg2.first.y());
      Pt2i rp2(seg2.second.x()+ext, seg2.second.y());
      if( (lp1.y()<=lp2.y() && lp2.y()<=rp1.y()) || (lp1.y()>=lp2.y() && lp2.y()>=rp1.y()) ) { // lp1.y < lp2.y < rp1.y
        if( (lp2.x()<=lp1.x() && lp1.x()<rp2.x()) || (lp2.x()>=lp1.x() && lp1.x()>=rp2.x()) ) { // lp2.x < lp1.x < rp2.x
          Pt2i pt (lp1.x(),lp2.y());
          ptsInt.push_back(pt);
        }
      }
    }
    ptsIntV.push_back(ptsInt);
  }
  return ptsIntV;
}

vector<pair<Pt2i, Pt2i> > getTableCells(std::vector<std::pair<Pt2i, Pt2i> > segH, std::vector<std::pair<Pt2i, Pt2i> > segV, int ext = 3) {
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
      //if(abs(c1.x()-c21.x())>2*ext && abs(c1.y()-c21.y())>2*ext) //Verify thickness of the cell
        boxes.push_back(make_pair(c1, c21)); //left cell
      Pt2i c22 (seg2.first.x(), seg2.first.y()); //lp2
      //if(abs(c1.x()-c22.x())>2*ext && abs(c1.y()-c22.y())>2*ext) //Verify thickness of the cell
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
    //std::cout << "minWidth=" << minWidth << " minHeight=" << minHeight << " w=" << w << " h=" << h << std::endl;
    Pt2i p1(x,y);
    Pt2i p2(x+w,y+h);
    //if ((stats.at<int>(i, CC_STAT_AREA)) > minSize)
    if(w>minWidth+1 && h>minHeight+1)
      boxes.push_back(make_pair(p1, p2));
  }
  return boxes;
}

int main(int argc, char *argv[]) {
  string dir = "/Users/hngo/Documents/Codes/Code_Dev/TableExtraction/Results/";
  string dirImg = "gt_eu_img/";
  string dirRes = "gt_eu_res/";
  
  // parse command line using CLI ----------------------------------------------
  CLI::App app;
  int db {0}; //0: Eu, 1: Us
  string filename {"eu-001_page0"};//eu-001_page0 eu-002_page0 eu-003_page0 eu-022_page1 eu-023_page2
  app.add_option("--input,-i,1", filename, "Input filename (without extention).");
  app.add_option("--database,-d,2", db, "Select de database (0: Eu, 1: Us).", true);
  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------
  
  if(db==1) {
    dirImg = "gt_us_img/";
    dirRes = "gt_us_res/";
  }
  
  string imgFileName = dir+dirImg+filename+".jpg";//".jpg";
  string resFilename = dir+dirRes+filename +".png";
  
  int tolAlign = 3;
  int tolDistGr = 10*tolAlign;
  int tolDistNear = 5;
  int tolLen = 4*tolDistNear;
  int scale = 2; 
  
  string outFilename;
  //Step 1: Load image
  Mat img = imread(imgFileName, IMREAD_COLOR); //IMREAD_GRAYSCALE
  int width = img.cols;
  int height = img.rows;
  resize(img, img, Size(scale*width,scale*height),INTER_LINEAR);
  width = scale*width;
  height = scale*height;
  Mat grayImg;
  cvtColor(img, grayImg, COLOR_BGR2GRAY);
  if(SAVE_FILE) {
    outFilename = filename + "_gray.png";
    imwrite(outFilename, grayImg);
  }
  
  if (width == 0 || height == 0) {
    cerr << "Couldn't open the " << imgFileName << " image file." << endl;
    exit (EXIT_FAILURE);
  }
  // Copy image
  int **tabImage = new int *[height];
  uchar c;
  for (int y = 0; y < height; y++) {
    tabImage[y] = new int[width];
    for (int x = 0; x < width; x++) {
      c = grayImg.at<uchar>(Point(x, y));
      tabImage[y][x] = int(c);
    }
  }
  //Step 2: Gen grandient map for the dectetor
  // Check the gMap pointer
  VMap *gMap = NULL;
  if (gMap != NULL)
    delete gMap;
  // Create the gradient map
  gMap = new VMap(width, height, tabImage, VMap::TYPE_SOBEL_5X5);
  Mat gradImg(Size(width,height),CV_8UC1);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      gradImg.at<uchar>(Point(x, y)) = gMap->magn(x,y);
    }
  }
  if(SAVE_FILE) {
    outFilename = filename + "_grad.png";
    imwrite(outFilename, gradImg);
  }
  
  std::cout << "FBSD ..." << std::endl;
  // Create the FBSD detector
  BSDetector detector;
  detector.setGradientMap(gMap);
  detector.setAssignedThickness(1);
  // Fbsd detector
  detector.resetMaxDetections ();
  detector.detectAll();
  // Retrieve the detected blurred segments
  vector<BlurredSegment *> blurredSegments = detector.getBlurredSegments();
  std::cout << "FBSD OK : " << blurredSegments.size() << " blurred segments" << std::endl;
  
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
  //write first detection result into file
  if(SAVE_FILE) {
    ofstream myfile;
    string filetxt = filename+".txt";
    myfile.open (filetxt);
    for(int it=0; it<seg.size(); it++) {
      Pt2i lp = seg.at(it).first;
      Pt2i rp = seg.at(it).second;
      myfile << lp.x()<<" "<<lp.y()<<" "<<rp.x()<<" "<<rp.y()<<std::endl;
    }
    myfile.close();
  }
  /*
  for(int it=0; it<seg.size(); it++) {
    Pt2i lp = seg.at(it).first;
    Pt2i rp = seg.at(it).second;
    line(img, Point(lp.x(),lp.y()), Point(rp.x(),rp.y()), Scalar(0,255,0));
  }
  resize(img, img, Size(resize*width,resize*height),INTER_LINEAR);
  imshow("image", img);
  waitKey(0);
  exit(0);
  */
  
  //Step 3: filtering vertial or horizontal segments
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
  
  //Step 4: extending and sorting vertial or horizontal segments
  std::vector<std::pair<Pt2i, Pt2i> > segHs = segH, segVs = segV;
  std::sort(std::begin(segHs), std::end(segHs),
            [](std::pair<Pt2i, Pt2i> s1, std::pair<Pt2i, Pt2i> s2) //{return s1.first.y() < s2.first.y(); });
            { return s1.first.y() < s2.first.y() || (s1.first.y() == s2.first.y() && s1.first.x() < s2.first.x()); });
  
  std::sort(std::begin(segVs), std::end(segVs),
            [](std::pair<Pt2i, Pt2i> s1, std::pair<Pt2i, Pt2i> s2) //{return s1.first.x() < s2.first.x(); });
            { return s1.first.x() < s2.first.x() || (s1.first.x() == s2.first.x() && s1.first.y() < s2.first.y()); });
  
  std::vector<std::pair<Pt2i, Pt2i> > segHsE, segVsE;
  std::vector<std::vector<int> > idSegHsE, idSegVsE;
  int it1 = 0, it2 = 0;
  Pt2i lp1, rp1, lp2, rp2, lp3, rp3;
  int l = 0;
  //Horizontal segments
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
    //if(double(l)/abs(rp3.x() - lp3.x()) > 0.5 && l > 10 && abs(lp3.y() - rp3.y())<3) //density and length of extended segment
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
  
  //Vertical segments
  it1 = 0;
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
    //if(double(l)/abs(rp3.y() - lp3.y()) > 0.5 && l > 10 && abs(lp3.x() - rp3.x())<3) //density and length of extended segment
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
  
  //Step 5: regroupe vertical and horizontal segments
  std::vector<std::pair<Pt2i, Pt2i> > segHsEg, segVsEg;
  //Horizontal segments
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
    //regroup near horizontal segments
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
      if(l > tolLen)// && double(l)/abs(rp3.x() - lp3.x()) > 0.5) //length and density of extended segment
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
  
  //Vertical segments
  for(int it=0; it<idSegVsE.size(); it++) {
    std::vector<int> idSeg = idSegVsE.at(it);
    std::vector<std::pair<Pt2i, Pt2i> > segEx;
    for(int it_bis=0; it_bis<idSeg.size(); it_bis++) {
      Pt2i lp = segVs.at(idSeg.at(it_bis)).first;
      Pt2i rp = segVs.at(idSeg.at(it_bis)).second;
      segEx.push_back(make_pair(lp, rp));
    }
    //sort segEx by y
    std::sort(std::begin(segEx), std::end(segEx),
              [](std::pair<Pt2i, Pt2i> s1, std::pair<Pt2i, Pt2i> s2) {return s1.first.y() < s2.first.y(); });
    //regroup near segments
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
      if(l > tolLen) { // && double(l)/abs(rp3.y() - lp3.y()) > 0.5) //length and density of extended segment
        segVsEg.push_back(std::make_pair(lp3, rp3));
      }
      it1 = it2;
    }
    if(it1 == segEx.size()-1) {
      lp1 = segEx.at(it1).first;
      rp1 = segEx.at(it1).second;
      l = abs(rp1.y() - lp1.y());
      //cout<<"it1="<<it1<<" and l="<<l<<endl;
      if(l > tolLen) {
        segVsEg.push_back(std::make_pair(lp1, rp1));
      }
    }
  }
  
  //Step 6: filtering segments belonging to tables but not texte
  std::vector<std::pair<Pt2i, Pt2i> > segHsEgT, segVsEgT;
  //Horizontal segment
  for(int it=0; it<segHsEg.size(); it++) {
    Pt2i lp = segHsEg.at(it).first;
    Pt2i rp = segHsEg.at(it).second;
    if(isHoziontalSegTab(grayImg/*gradImg*/, lp, rp))
      segHsEgT.push_back(make_pair(lp, rp));
  }
  //Vertical segment
  for(int it=0; it<segVsEg.size(); it++) {
    Pt2i lp = segVsEg.at(it).first;
    Pt2i rp = segVsEg.at(it).second;
    if(isVerticalSegTab(grayImg/*gradImg*/, lp, rp))
      segVsEgT.push_back(make_pair(lp, rp));
  }
  
  //Step 7: get intersecting points alongs horizontal and vertical segments => NO NEED !
  vector<vector<Pt2i> > ptsH = getHorizontalIntersection(segHsEgT, segVsEgT);
  vector<vector<Pt2i> > ptsV = getVerticalIntersection(segHsEgT, segVsEgT);
  
  //Step 8: table reconstruction
  vector<pair<Pt2i, Pt2i> > cells = getTableCells(segHsEgT, segVsEgT);
  /*
  //Step 9: lengthening to image size the vertical and horizontal segments
  std::vector<std::pair<Pt2i, Pt2i> > segHL, segVL;
  for(int it=0; it<segHsEgT.size(); it++) {
    Pt2i lp = segHsEgT.at(it).first;
    lp.set(0, lp.y());
    Pt2i rp = segHsEgT.at(it).second;
    rp.set(width, lp.y());
    segHL.push_back(std::make_pair(lp, rp));
  }
  
  for(int it=0; it<segVsEgT.size(); it++) {
    Pt2i lp = segVsEgT.at(it).first;
    lp.set(lp.x(),0);
    Pt2i rp = segVsEgT.at(it).second;
    rp.set(lp.x(), height);
    segVL.push_back(std::make_pair(lp, rp));
  }
  */
  
  //Step 9: Table reconstruction
  vector<pair<Pt2i, Pt2i> > tables = getTables(grayImg.size(), cells);
  
  //Step 10: Display result
  Mat imgCopy = grayImg.clone();
  cvtColor(imgCopy, imgCopy, COLOR_GRAY2BGR);
  //Mat imgCopy = img.clone();
  for(int it=0; it<seg.size(); it++) {
    Pt2i lp = seg.at(it).first;
    Pt2i rp = seg.at(it).second;
    line(imgCopy, Point(lp.x(),lp.y()), Point(rp.x(),rp.y()), Scalar(0,255,0));
  }
  if(SAVE_FILE) {
    outFilename = filename + "_Seg.png";
    imwrite(outFilename, imgCopy);
  }
  
  imgCopy = grayImg.clone();
  cvtColor(imgCopy, imgCopy, COLOR_GRAY2BGR);
  //imgCopy = grayImg.clone();
  for(int it=0; it<idSegHsE.size(); it++) {//segHs.size()
    vector<int> idSeg = idSegHsE.at(it);
    for(int it_bis=0; it_bis<idSeg.size(); it_bis++) {
      Pt2i lp = segHs.at(idSeg.at(it_bis)).first;//segHs.at(it).first;
      Pt2i rp = segHs.at(idSeg.at(it_bis)).second;//segHs.at(it).second;
      line(imgCopy, Point(lp.x(),lp.y()), Point(rp.x(),rp.y()), Scalar(255,0,0));
    }
  }
  
  for(int it=0; it<idSegVsE.size(); it++) {//segVs.size()
    vector<int> idSeg = idSegVsE.at(it);
    for(int it_bis=0; it_bis<idSeg.size(); it_bis++) {
      Pt2i lp = segVs.at(idSeg.at(it_bis)).first;//segVs.at(it).first;
      Pt2i rp = segVs.at(idSeg.at(it_bis)).second;//segVs.at(it).second;
      line(imgCopy, Point(lp.x(),lp.y()), Point(rp.x(),rp.y()), Scalar(0,0,255));
    }
  }
  if(SAVE_FILE) {
    outFilename = filename + "_HV.png";
    imwrite(outFilename, imgCopy);
  }
  //imshow("image", imgCopy);
  //waitKey(0);
  
  imgCopy = grayImg.clone();
  cvtColor(imgCopy, imgCopy, COLOR_GRAY2BGR);
  //imgCopy = img.clone();
  for(int it=0; it<segHsE.size(); it++) {
    Pt2i lp = segHsE.at(it).first;
    Pt2i rp = segHsE.at(it).second;
    line(imgCopy, Point(lp.x(),lp.y()), Point(rp.x(),rp.y()), Scalar(255,0,0));
  }
  
  for(int it=0; it<segVsE.size(); it++) {
    Pt2i lp = segVsE.at(it).first;
    Pt2i rp = segVsE.at(it).second;
    line(imgCopy, Point(lp.x(),lp.y()), Point(rp.x(),rp.y()), Scalar(0,0,255));
  }
  if(SAVE_FILE) {
    outFilename = filename + "_HVE.png";
    imwrite(outFilename, imgCopy);
  }
  //imshow("image", imgCopy);
  //waitKey(0);
  
  imgCopy = grayImg.clone();
  cvtColor(imgCopy, imgCopy, COLOR_GRAY2BGR);
  //imgCopy = img.clone();
  for(int it=0; it<segHsEg.size(); it++) {
    Pt2i lp = segHsEg.at(it).first;
    Pt2i rp = segHsEg.at(it).second;
    line(imgCopy, Point(lp.x(),lp.y()), Point(rp.x(),rp.y()), Scalar(255,0,0));
  }
  for(int it=0; it<segVsEg.size(); it++) {
    Pt2i lp = segVsEg.at(it).first;
    Pt2i rp = segVsEg.at(it).second;
    line(imgCopy, Point(lp.x(),lp.y()), Point(rp.x(),rp.y()), Scalar(0,0,255));
  }
  if(SAVE_FILE) {
    outFilename = filename + "_HVEg.png";
    imwrite(outFilename, imgCopy);
  }
  //imshow("image", imgCopy);
  //waitKey(0);
  
  imgCopy = grayImg.clone();
  cvtColor(imgCopy, imgCopy, COLOR_GRAY2BGR);
  //imgCopy = img.clone();
  for(int it=0; it<segHsEgT.size(); it++) {
    Pt2i lp = segHsEgT.at(it).first;
    Pt2i rp = segHsEgT.at(it).second;
    line(imgCopy, Point(lp.x(),lp.y()), Point(rp.x(),rp.y()), Scalar(255,0,0));
  }
  for(int it=0; it<segVsEgT.size(); it++) {
    Pt2i lp = segVsEgT.at(it).first;
    Pt2i rp = segVsEgT.at(it).second;
    line(imgCopy, Point(lp.x(),lp.y()), Point(rp.x(),rp.y()), Scalar(0,0,255));
  }
  if(SAVE_FILE) {
    outFilename = filename + "_HVT.png";
    imwrite(outFilename, imgCopy);
  }
  //imshow("image", imgCopy);
  //waitKey(0);
  
  imgCopy = grayImg.clone();
  cvtColor(imgCopy, imgCopy, COLOR_GRAY2BGR);
  //imgCopy = img.clone();
  for(int it=0; it<ptsH.size(); it++) {
    for(int it_bis=0; it_bis<ptsH.at(it).size(); it_bis++) {
      Pt2i p = ptsH.at(it).at(it_bis);
      circle(imgCopy, Point(p.x(), p.y()), 5, Scalar(0,255,0), -1);
    }
    if(ptsH.at(it).size()>0) {
      Pt2i lp = segHsEgT.at(it).first;
      Pt2i rp = segHsEgT.at(it).second;
      line(imgCopy, Point(lp.x(),lp.y()), Point(rp.x(),rp.y()), Scalar(255,0,0));
    }
  }
  for(int it=0; it<ptsV.size(); it++) {
    for(int it_bis=0; it_bis<ptsV.at(it).size(); it_bis++) {
      Pt2i p = ptsV.at(it).at(it_bis);
      circle(imgCopy, Point(p.x(), p.y()), 5, Scalar(0,255,0), -1);
    }
    if(ptsV.at(it).size()>0) {
      Pt2i lp = segVsEgT.at(it).first;
      Pt2i rp = segVsEgT.at(it).second;
      line(imgCopy, Point(lp.x(),lp.y()), Point(rp.x(),rp.y()), Scalar(0,0,255));
    }
  }
  if(SAVE_FILE) {
    outFilename = filename + "_HVInt.png";
    imwrite(outFilename, imgCopy);
  }
  //imshow("image", imgCopy);
  //waitKey(0);
  
  imgCopy = grayImg.clone();
  cvtColor(imgCopy, imgCopy, COLOR_GRAY2BGR);
  //imgCopy = img.clone();
  for(int it=0; it<cells.size(); it++) {
    Pt2i p1 = cells.at(it).first;
    Pt2i p2 = cells.at(it).second;
    //rectangle(imgCopy, Point(p1.x(), p1.y()), Point(p2.x(), p2.y()), Scalar(120,120,120),-1);
    rectangle(imgCopy, Point(p1.x(), p1.y()), Point(p2.x(), p2.y()), Scalar(0,0,0),-1);
  }
  if(SAVE_FILE) {
    outFilename = filename + "_HVCell.png";
    imwrite(outFilename, imgCopy);
  }
  //imshow("image", imgCopy);
  //waitKey(0);
  
  imgCopy = grayImg.clone();
  cvtColor(imgCopy, imgCopy, COLOR_GRAY2BGR);
  //imgCopy = img.clone();
  for(int it=0; it<tables.size(); it++) {
    Pt2i p1 = tables.at(it).first;
    Pt2i p2 = tables.at(it).second;
    rectangle(imgCopy, Point(p1.x(), p1.y()), Point(p2.x(), p2.y()), Scalar(0,0,0),-1);
  }
  if(SAVE_FILE) {
    outFilename = filename + "_HVTable.png";
    imwrite(outFilename, imgCopy);
  }
  //imshow("image", imgCopy);
  //waitKey(0);
  
  /*
  imgCopy = img.clone();
  for(int it=0; it<segHL.size(); it++) {
    Pt2i lp = segHL.at(it).first;
    Pt2i rp = segHL.at(it).second;
    //if(it<segHL.size()-1 && abs(segHL.at(it).first.y()-segHL.at(it+1).first.y()) > tolDistNear)
    //   line(image, Point(lp.x(),lp.y()), Point(rp.x(),rp.y()), Scalar(255,0,0));
    //if(it==segHL.size()-1 && abs(segHL.at(it).first.y()-segHL.at(it-1).first.y()) > tolDistNear)
      line(imgCopy, Point(lp.x(),lp.y()), Point(rp.x(),rp.y()), Scalar(255,0,0));
  }
  for(int it=0; it<segVL.size(); it++) {
    Pt2i lp = segVL.at(it).first;
    Pt2i rp = segVL.at(it).second;
    //if(it<segVL.size()-1 && abs(segVL.at(it).first.x()-segVL.at(it+1).first.x()) > tolDistNear)
    //  line(image, Point(lp.x(),lp.y()), Point(rp.x(),rp.y()), Scalar(0,0,255));
    //if(it==segVL.size()-1 && abs(segVL.at(it).first.x()-segVL.at(it-1).first.x()) > tolDistNear)
      line(imgCopy, Point(lp.x(),lp.y()), Point(rp.x(),rp.y()), Scalar(0,0,255));
  }
   */
  
  //namedWindow("image", WINDOW_AUTOSIZE);
  //resize(img, img, Size(resize*width,resize*height),INTER_LINEAR);
  Mat resImg(Size(width,height),CV_8UC1, cv::Scalar(0));
  for(int it=0; it<cells.size(); it++) {
    Pt2i p1 = cells.at(it).first;
    Pt2i p2 = cells.at(it).second;
    rectangle(resImg, Point(p1.x(), p1.y()), Point(p2.x(), p2.y()), Scalar(255), -1);
  }
  resize(resImg, resImg, Size(width/scale,height/scale),INTER_LINEAR);
  threshold(resImg, resImg, 125, 255, THRESH_BINARY);
  imwrite(resFilename, resImg);
  //imshow("image", resImg);
  //waitKey(0);
  
  return EXIT_SUCCESS;
}
