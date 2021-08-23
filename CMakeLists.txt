project(TableExtraction)
cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0057 NEW)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Find libpng library
find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
message(STATUS "LibPNG include dir: '${PNG_INCLUDE_DIRS}' and '${LIBPNG_LIBRARIES}'")

add_definitions(-g)

# Input + CLI11
include_directories(
    ${PROJECT_SOURCE_DIR}/BlurredSegment
    ${PROJECT_SOURCE_DIR}/ConvexHull
    ${PROJECT_SOURCE_DIR}/DirectionalScanner
    ${PROJECT_SOURCE_DIR}/ImageTools
    ${PROJECT_SOURCE_DIR}/ext 
)

# Add files
set(SOURCE_BASE_HEADER_FILES 
           ${PROJECT_SOURCE_DIR}/BlurredSegment/biptlist.h
           ${PROJECT_SOURCE_DIR}/BlurredSegment/blurredsegment.h
           ${PROJECT_SOURCE_DIR}/BlurredSegment/bsdetector.h
           ${PROJECT_SOURCE_DIR}/BlurredSegment/bsproto.h
           ${PROJECT_SOURCE_DIR}/BlurredSegment/bstracker.h
           ${PROJECT_SOURCE_DIR}/BlurredSegment/nfafilter.h
           ${PROJECT_SOURCE_DIR}/ConvexHull/antipodal.h
           ${PROJECT_SOURCE_DIR}/ConvexHull/chvertex.h
           ${PROJECT_SOURCE_DIR}/ConvexHull/convexhull.h
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/adaptivescannero1.h
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/adaptivescannero2.h
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/adaptivescannero7.h
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/adaptivescannero8.h
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/directionalscanner.h
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/directionalscannero1.h
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/directionalscannero2.h
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/directionalscannero7.h
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/directionalscannero8.h
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/scannerprovider.h
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/vhscannero1.h
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/vhscannero2.h
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/vhscannero7.h
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/vhscannero8.h
           ${PROJECT_SOURCE_DIR}/ImageTools/absrat.h
           ${PROJECT_SOURCE_DIR}/ImageTools/digitalstraightline.h
           ${PROJECT_SOURCE_DIR}/ImageTools/digitalstraightsegment.h
           ${PROJECT_SOURCE_DIR}/ImageTools/edist.h
           ${PROJECT_SOURCE_DIR}/ImageTools/pt2i.h
           ${PROJECT_SOURCE_DIR}/ImageTools/vmap.h
           ${PROJECT_SOURCE_DIR}/ImageTools/vr2i.h
           ${PROJECT_SOURCE_DIR}/ImageTools/image.hpp
)

set(SOURCE_BASE_FILES
           ${PROJECT_SOURCE_DIR}/BlurredSegment/biptlist.cpp
           ${PROJECT_SOURCE_DIR}/BlurredSegment/blurredsegment.cpp
           ${PROJECT_SOURCE_DIR}/BlurredSegment/bsdetector.cpp
           ${PROJECT_SOURCE_DIR}/BlurredSegment/bsproto.cpp
           ${PROJECT_SOURCE_DIR}/BlurredSegment/bstracker.cpp
           ${PROJECT_SOURCE_DIR}/BlurredSegment/nfafilter.cpp
           ${PROJECT_SOURCE_DIR}/ConvexHull/antipodal.cpp
           ${PROJECT_SOURCE_DIR}/ConvexHull/chvertex.cpp
           ${PROJECT_SOURCE_DIR}/ConvexHull/convexhull.cpp
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/adaptivescannero1.cpp
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/adaptivescannero2.cpp
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/adaptivescannero7.cpp
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/adaptivescannero8.cpp
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/directionalscanner.cpp
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/directionalscannero1.cpp
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/directionalscannero2.cpp
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/directionalscannero7.cpp
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/directionalscannero8.cpp
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/scannerprovider.cpp
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/vhscannero1.cpp
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/vhscannero2.cpp
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/vhscannero7.cpp
           ${PROJECT_SOURCE_DIR}/DirectionalScanner/vhscannero8.cpp
           ${PROJECT_SOURCE_DIR}/ImageTools/digitalstraightline.cpp
           ${PROJECT_SOURCE_DIR}/ImageTools/digitalstraightsegment.cpp
           ${PROJECT_SOURCE_DIR}/ImageTools/edist.cpp
           ${PROJECT_SOURCE_DIR}/ImageTools/pt2i.cpp
           ${PROJECT_SOURCE_DIR}/ImageTools/vmap.cpp
           ${PROJECT_SOURCE_DIR}/ImageTools/vr2i.cpp
)



add_executable(TableExtraction main.cpp ${SOURCE_BASE_FILES} ${SOURCE_BASE_HEADER_FILES} ${PROJECT_SOURCE_DIR}/ext/CLI11.hpp)
target_link_libraries (TableExtraction ${OpenCV_LIBS})
