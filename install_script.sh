#!/bin/bash
BUILD_DIR=$HOME/build1
SOURCE_DIR="$(mktemp -d)"
GOOGLE_TEST_SOURCE_DIR=$SOURCE_DIR/googletest
GOOGLE_BENCH_SOURCE_DIR=$SOURCE_DIR/googlebench
rm -rf $BUILD_DIR
git clone "https://github.com/google/googletest.git" $GOOGLE_TEST_SOURCE_DIR
git clone "https://github.com/google/benchmark.git" $GOOGLE_BENCH_SOURCE_DIR
GOOGLE_TEST_BUILD_DIR=$BUILD_DIR/googletest
GOOGLE_BENCH_BUILD_DIR=$BUILD_DIR/googlebench
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
