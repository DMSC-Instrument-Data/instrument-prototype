#!/bin/bash

# 1st argument is the full path to the top level source directory
# 2nd argument is the directory name of the google test source directory 
# 3rd argument is the directory name of the google bench source directory
# 4th argument is the directory name of the eigen source directory
# 5th argument is the full path to the top level build directory
# 6th argument is the directory name of the google test build directory 
# 7th argument is the directory name of the google bench build directory

SOURCE_DIR=$1
BUILD_DIR=$5
echo "Source to $SOURCE_DIR"
echo "Building to $BUILD_DIR"
GOOGLE_TEST_SOURCE_DIR=$SOURCE_DIR/$2
GOOGLE_BENCH_SOURCE_DIR=$SOURCE_DIR/$3
EIGEN_SOURCE_DIR=$SOURCE_DIR/$4
rm -rf $BUILD_DIR
git clone "https://github.com/google/googletest.git" $GOOGLE_TEST_SOURCE_DIR
git clone "https://github.com/google/benchmark.git" $GOOGLE_BENCH_SOURCE_DIR
hg clone "http://bitbucket.org/eigen/eigen" $EIGEN_SOURCE_DIR
GOOGLE_TEST_BUILD_DIR=$6
GOOGLE_BENCH_BUILD_DIR=$7
mkdir $BUILD_DIR
cd $BUILD_DIR
mkdir $GOOGLE_TEST_BUILD_DIR
cd $GOOGLE_TEST_BUILD_DIR
cmake -DCMAKE_BUILD_TYPE=Release . $GOOGLE_TEST_SOURCE_DIR
cmake --build . 
cd $BUILD_DIR
mkdir $GOOGLE_BENCH_BUILD_DIR
cd $GOOGLE_BENCH_BUILD_DIR
cmake -DCMAKE_BUILD_TYPE=Release . $GOOGLE_BENCH_SOURCE_DIR
cmake --build .
echo "GOOGLE_TEST_SOURCE_DIR $GOOGLE_TEST_SOURCE_DIR"
echo "GOOGLE_BENCH_SOURCE_DIR $GOOGLE_BENCH_SOURCE_DIR"
echo "EIGEN_SOURCE_DIR $EIGEN_SOURCE_DIR"
#rm -rf $SOURCE_DIR