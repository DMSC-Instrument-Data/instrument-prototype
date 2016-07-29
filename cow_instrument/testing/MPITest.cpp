#include <gtest/gtest.h>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <iostream>

/*
 * This is a meta test relating to mpi and boost mpi. We should remove it.
 */

TEST(mpi_test, test_mpi_exec) {

  namespace mpi = boost::mpi;
  mpi::environment env;
  mpi::communicator world;
  EXPECT_NO_THROW(world.rank());
}
