#!/bin/bash

# 1st argument is the full path to the top level build directory
# 2nd argument is the directory name of the google test build directory 
# 3rd argument is the directory name of the google bench build directory
BUILD_DIR=$1
echo "Building to $BUILD_DIR"
SOURCE_DIR="$(mktemp -d)"
GOOGLE_TEST_SOURCE_DIR=$SOURCE_DIR/googletest
GOOGLE_BENCH_SOURCE_DIR=$SOURCE_DIR/googlebench
rm -rf $BUILD_DIR
git clone "https://github.com/google/googletest.git" $GOOGLE_TEST_SOURCE_DIR
git clone "https://github.com/google/benchmark.git" $GOOGLE_BENCH_SOURCE_DIR
GOOGLE_TEST_BUILD_DIR=$BUILD_DIR/$2
GOOGLE_BENCH_BUILD_DIR=$BUILD_DIR/$3
mkdir $BUILD_DIR
mkdir $GOOGLE_TEST_BUILD_DIR
cd $GOOGLE_TEST_BUILD_DIR
cmake -DCMAKE_BUILD_TYPE=Release . $GOOGLE_TEST_SOURCE_DIR
make -j8
mkdir $GOOGLE_BENCH_BUILD_DIR
cd $GOOGLE_BENCH_BUILD_DIR
cmake -DCMAKE_BUILD_TYPE=Release . $GOOGLE_BENCH_SOURCE_DIR
make -j8
rm -rf $SOURCE_DIR