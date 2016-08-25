#include <stdio.h>
#include <algorithm>
#include <tuple>

#include "IndexTranslator.h"
#include "gtest/gtest.h"

using namespace testing;

namespace {

TEST(partitioning_test, test_must_have_not_zero_partitions) {

  EXPECT_THROW(Partitioning p(0), std::invalid_argument)
      << "Cannot have a Partitioning with zero partiions";
}

TEST(partitioning_test, test_single_partition) {
  Partitioning p(1 /*single partition only*/);
  size_t globalSpectrumIndex = 0;
  EXPECT_EQ(0, p.partitionIndexOf(globalSpectrumIndex))
      << "Partition should always be zero";
  globalSpectrumIndex = 2;
  EXPECT_EQ(0, p.partitionIndexOf(globalSpectrumIndex))
      << "Partition should always be zero";
  ;
  globalSpectrumIndex = 11;
  EXPECT_EQ(0, p.partitionIndexOf(globalSpectrumIndex))
      << "Partition should always be zero";
  ;
}

TEST(index_translator_test, test_simple_index_translator_partitioning) {
  Partitioning singlePartition(1 /*Two partitions*/);
  std::vector<SpectrumNumber> spectrumNumbers = {1, 2, 3};
  const size_t partitionIndex = 0;
  IndexTranslator translator(singlePartition, partitionIndex, spectrumNumbers);

  auto spectrumIndexes = translator.spectrumIndices(singlePartition);
  EXPECT_EQ(spectrumIndexes.size(), 1);
  EXPECT_EQ(spectrumIndexes[0], std::vector<size_t>({0, 1, 2}));
}

TEST(index_translator_test, test_split_1_to_2) {
  Partitioning p1(1);
  IndexTranslator t(p1, 0, {1, 2, 4, 5, 6});
  Partitioning p2(2);

  auto indices = t.spectrumIndices(p2);
  EXPECT_EQ(indices.size(), 2);
  EXPECT_EQ(indices[0], std::vector<size_t>({0, 2, 4}));
  EXPECT_EQ(indices[1], std::vector<size_t>({1, 3}));

  auto globalIndices = t.globalSpectrumIndices(p2);
  EXPECT_EQ(globalIndices.size(), 2);
  EXPECT_EQ(globalIndices[0], std::vector<GlobalSpectrumIndex>({0, 2, 4}));
  EXPECT_EQ(globalIndices[1], std::vector<GlobalSpectrumIndex>({1, 3}));

  auto spectrumNumbers = t.spectrumNumbers(p2);
  EXPECT_EQ(spectrumNumbers.size(), 2);
  EXPECT_EQ(spectrumNumbers[0], std::vector<SpectrumNumber>({1, 4, 6}));
  EXPECT_EQ(spectrumNumbers[1], std::vector<SpectrumNumber>({2, 5}));
}

TEST(index_translator_test, test_split_2_to_3_partition_0) {
  Partitioning p2(2);
  IndexTranslator t(p2, 0, {1, 2, 4, 5, 6});
  Partitioning p3(3);

  // --- Indexing and resulting partitioning based on p2 ---
  // SpectrumNumber        1 2 4 5 6
  // GlobalSpectrumIndex   0 1 2 3 4
  // Partition (start)     0 1 0 1 0
  // SpectrumIndex (start) 0 0 1 1 2

  // --- Indexing and resulting partitioning based on p3 ---
  // Partition (end)       0 1 2 0 1
  // SpectrumIndex (end)   0 0 0 1 1

  auto indices = t.spectrumIndices(p3);
  EXPECT_EQ(indices.size(), 3);
  EXPECT_EQ(indices[0], std::vector<size_t>({0}));
  EXPECT_EQ(indices[1], std::vector<size_t>({2}));
  EXPECT_EQ(indices[2], std::vector<size_t>({1}));

  auto globalIndices = t.globalSpectrumIndices(p3);
  EXPECT_EQ(globalIndices.size(), 3);
  EXPECT_EQ(globalIndices[0], std::vector<GlobalSpectrumIndex>({0}));
  EXPECT_EQ(globalIndices[1], std::vector<GlobalSpectrumIndex>({4}));
  EXPECT_EQ(globalIndices[2], std::vector<GlobalSpectrumIndex>({2}));

  auto spectrumNumbers = t.spectrumNumbers(p3);
  EXPECT_EQ(spectrumNumbers.size(), 3);
  EXPECT_EQ(spectrumNumbers[0], std::vector<SpectrumNumber>({1}));
  EXPECT_EQ(spectrumNumbers[1], std::vector<SpectrumNumber>({6}));
  EXPECT_EQ(spectrumNumbers[2], std::vector<SpectrumNumber>({4}));
}

TEST(index_translator_test, test_split_2_to_3_partition_1) {
  Partitioning p2(2);
  IndexTranslator t(p2, 1, {1, 2, 4, 5, 6});
  Partitioning p3(3);

  auto indices = t.spectrumIndices(p3);
  EXPECT_EQ(indices.size(), 3);
  EXPECT_EQ(indices[0], std::vector<size_t>({1}));
  EXPECT_EQ(indices[1], std::vector<size_t>({0}));
  EXPECT_EQ(indices[2], std::vector<size_t>({}));

  auto globalIndices = t.globalSpectrumIndices(p3);
  EXPECT_EQ(globalIndices.size(), 3);
  EXPECT_EQ(globalIndices[0], std::vector<GlobalSpectrumIndex>({3}));
  EXPECT_EQ(globalIndices[1], std::vector<GlobalSpectrumIndex>({1}));
  EXPECT_EQ(globalIndices[2], std::vector<GlobalSpectrumIndex>({}));

  auto spectrumNumbers = t.spectrumNumbers(p3);
  EXPECT_EQ(spectrumNumbers.size(), 3);
  EXPECT_EQ(spectrumNumbers[0], std::vector<SpectrumNumber>({5}));
  EXPECT_EQ(spectrumNumbers[1], std::vector<SpectrumNumber>({2}));
  EXPECT_EQ(spectrumNumbers[2], std::vector<SpectrumNumber>({}));
}

TEST(index_translator_test, test_split_2_to_3_combine_results) {
  // This tests mimics what would happen in an MPI redistribution
  Partitioning p2(2);
  std::vector<IndexTranslator> t;
  t.emplace_back(
      p2, 0, std::vector<SpectrumNumber>{1, 2, 4, 5, 6, 8, 9}); // partition 0
  t.emplace_back(
      p2, 1, std::vector<SpectrumNumber>{1, 2, 4, 5, 6, 8, 9}); // partition 1
  Partitioning p3(3);

  // Fake data object, could be anything.
  using Data = size_t;

  // Outermost vector is for target partition
  // Middle vector is for input partition
  // Innermost vector is spectrum index of input
  std::vector<std::vector<
      std::vector<std::tuple<GlobalSpectrumIndex, SpectrumNumber, Data>>>>
      receiveBuffer(p3.size());

  // This for-loop mimics MPI ranks *before* redistribution
  for (const auto partition : {0, 1}) {
    // 1. Get local spectrum indices (used to extract data)
    auto indices = t[partition].spectrumIndices(p3);

    // 2. Use indices to get data from object that should be redistributed. Here
    // we simply keep indices as fake data.
    const auto &data = indices;

    // 3. Tag data with GlobalSpectrumIndex and SpectrumNumber
    auto globalIndices = t[partition].globalSpectrumIndices(p3);
    auto spectrumNumbers = t[partition].spectrumNumbers(p3);
    std::vector<
        std::vector<std::tuple<GlobalSpectrumIndex, SpectrumNumber, Data>>>
        sendBuffer(p3.size());
    // Loop over target partition
    for (size_t p = 0; p < indices.size(); ++p) {
      for (size_t i = 0; i < indices[p].size(); ++i) {
        sendBuffer[p].emplace_back(globalIndices[p][i], spectrumNumbers[p][i],
                                   data[p][i]);
      }
    }

    // 4. Send data, e.g., with MPI_Isend(). Here we fake a send by simply
    // appending the receive buffer.
    for (size_t p = 0; p < indices.size(); ++p)
      receiveBuffer[p].push_back(sendBuffer[p]);
  }

  // This for-loop mimics MPI ranks *after* redistribution
  for (const auto partition : {0, 1, 2}) {
    // 5. Merge data from all ranks
    std::vector<std::tuple<GlobalSpectrumIndex, SpectrumNumber, Data>> result;
    // Loop over initial partition
    for (const auto &partitionData : receiveBuffer[partition])
      for (const auto &item : partitionData)
        result.push_back(item);

    // 6. Sort data according to global spectrum index
    std::sort(
        result.begin(), result.end(),
        [](const std::tuple<GlobalSpectrumIndex, SpectrumNumber, Data> &t1,
           const std::tuple<GlobalSpectrumIndex, SpectrumNumber, Data> &t2)
            -> bool { return std::get<0>(t1) < std::get<0>(t2); });

    // SpectrumNumber        1 2 4 5 6 8 9
    // GlobalSpectrumIndex   0 1 2 3 4 5 6
    // Partition (start)     0 1 0 1 0 1 0
    // SpectrumIndex (start) 0 0 1 1 2 2 3
    // Partition (end)       0 1 2 0 1 2 0
    // SpectrumIndex (end)   0 0 0 1 1 1 2
    switch (partition) {
    case 0:
      EXPECT_EQ(result.size(), 3);
      EXPECT_EQ(result[0], std::make_tuple(0, 1, 0));
      EXPECT_EQ(result[1], std::make_tuple(3, 5, 1));
      EXPECT_EQ(result[2], std::make_tuple(6, 9, 3));
      break;
    case 1:
      EXPECT_EQ(result.size(), 2);
      EXPECT_EQ(result[0], std::make_tuple(1, 2, 0));
      EXPECT_EQ(result[1], std::make_tuple(4, 6, 2));
      break;
    case 2:
      EXPECT_EQ(result.size(), 2);
      EXPECT_EQ(result[0], std::make_tuple(2, 4, 1));
      EXPECT_EQ(result[1], std::make_tuple(5, 8, 2));
      break;
    }
  }
}
}
