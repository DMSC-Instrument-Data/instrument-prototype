[![Build Status](https://travis-ci.org/DMSC-Instrument-Data/instrument-prototype.svg?branch=master)](https://travis-ci.org/DMSC-Instrument-Data/instrument-prototype)

##Repository Purpose##

To create a prototype and minimal-viable-product for the next generation of virtual instruments. This repository specifically looks at novel approaches to virtual instrument representations for high-throughput data reduction from neutron scattering experiments. A major requirement is the ability to rapidly extract key detector information required for data reduction. See [requirements](https://github.com/mantidproject/documents/blob/master/Design/Instrument-2.0/requirements-v2.md) for more context.

This repository is designed to host several prototypes aiming for the same purposes.

##What we expect the prototypes to do##

A distilled version of the requirements that the prototypes should address:

* Changing some part of the instrument geometry should not result in a deep copy.
* Allow each component to have a "shape" where each the shape can be shared across possibly 100,000's pixels but 
  interrogating the location of the shape produces its "real" position
* Store key/value pairs (limited allowed types is fine) against a component. We need to handle cases where a parameter value is set across either a composite component or the whole instrument, e.g. He3 pressure in a tube. The lookup in this cases would still be through the leaf component so the client is unaware that it is shared but we don't create potentially a great many entries that are the same. The lookup for this still needs to be reasonably quick, e.g. accessing `efixed` for a given component in `Indirect` mode during `ConvertUnits`.
* Fast access to frequently-queried geometry quantities such as `l1`, `l2`, `theta`, `phi` (essentially what would be in a `.dat` file)

##Building the prototype##

CMake is required to configure the prototype.

[GoogleTest](https://github.com/google/googletest) is required to build the prototype, and must be downloaded and built separately:

```
$ git clone https://github.com/google/googletest.git
$ cd googletest
$ cmake .
$ cmake --build
```

[GoogleBench](https://github.com/google/benchmark) is required, and should be built in a similar way to googletest:

```
$ git clone https://github.com/google/benchmark.git
$ cd benchmark
$ cmake .
$ cmake --build
```

[Eigen](http://eigen.tuxfamily.org) is required, but is header only, so no build is necessary.

```
$ hg clone http://bitbucket.org/eigen/eigen
```

There are several variables that may need to be set when configuring the prototype: `GTEST_SOURCE_DIR`, `GTEST_BUILD_DIR`, `GBENCH_BUILD_DIR`, `GBENCH_SOURCE_DIR`, `EIGEN_ADDITIONAL_SEARCH_PATHS`.

`GTEST_SOURCE_DIR` should be the path to the googletest topmost directory.
`GTEST_BUILD_DIR` should be the path to the googletest build directory (will be the same as `GTEST_SOURCE_DIR` in case of in-source builds). `GBENCH_SOURCE_DIR` should be the path to the googlebenchmark topmost directory containing the toplevel CMakeLists.txt. `GBENCH_BUILD_DIR` should be the path to the googlebench build directory, containing CMakeCache.txt. `EIGEN_ADDITIONAL_SEARCH_PATHS` should be the path to the topmost eigen source directory. The respective projects are located [here for googletest](https://github.com/google/googletest), [here for google benchmark](https://github.com/google/benchmark), [here for eigen](http://eigen.tuxfamily.org).  

GBench and GTest Variables mentioned above have defaults that assume googletest was built in-source (as shown above) and is adjacent to instrument-prototype. For example:

```
- Dev
|-- googletest
|-- googlebench
|-- instrument-prototype
```

If this is not how your directories are set up, or if you built googletest or googlebench as an out of source build, you will need to provide those variables to CMake when configuring the prototype. This can be done via the GUI or the commandline, e.g.:

```
$ git clone https://github.com/DMSC-Instrument-Data/instrument-prototype.git
$ cd instrument-prototype
$ mkdir build
$ cd build
$ cmake -DGTEST_SOURCE_DIR="{path_to_gtest_source}" -DGTEST_BUILD_DIR="{path_to_gtest_build}" -DGBENCH_SOURCE_DIR={path_to_gbench_source} -DGBENCH_BUILD_DIR="{path_to_gbench_build}" -DEIGEN_ADDITIONAL_SEARCH_PATHS="{path_to_eigen_source}"
$ make
```

CMake will issue an error if the directories are incorrect (if it cannot find the include/library files required).

You may also inspect the [.travis.yml](.travis.yml) file associated with this prototype to see individual steps for the build.

# Homebrew #

If you are using homebrew on OSX
```
brew install gcc
brew install boost --with-mpi --without-single --c++11
brew install mpi
```
prior to running cmake.


# Trusty #
```
For required trusty pacakges see *before_install* in [.travis.yml](.travis.yml)
```

