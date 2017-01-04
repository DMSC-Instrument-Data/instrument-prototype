#include "SpectrumInfo.h"
#include "gtest/gtest.h"
#include "DetectorInfo.h"
#include "MockTypes.h"
#include "SourceSampleDetectorPathFactory.h"

namespace {

TEST(spectrum_info_test, test_constructor_lhr) {
  std::vector<Spectrum> spectra{{0}, {1}, {2}};
  size_t nDetectors = 3;
  auto instrument = std::make_shared<NiceMockInstrumentTree>(nDetectors);
  DetectorInfoWithNiceMockInstrument detectorInfo{
      instrument, SourceSampleDetectorPathFactory<NiceMockInstrumentTree>{}};
  SpectrumInfo<NiceMockInstrumentTree> spectrumInfo(spectra, detectorInfo);

  EXPECT_EQ(3, spectrumInfo.size());
  EXPECT_EQ(3, spectrumInfo.nDetectors());
}

TEST(spectrum_info_test, test_constructor_one_to_one) {
  size_t nDetectors = 3;
  auto instrument =
      std::make_shared<testing::NiceMock<MockFlatTree>>(nDetectors);
  DetectorInfoWithMockInstrument detectorInfo{
      instrument, SourceSampleDetectorPathFactory<MockFlatTree>{}};
  SpectrumInfo<MockFlatTree> spectrumInfo(detectorInfo);

  EXPECT_EQ(3, spectrumInfo.size());
  EXPECT_EQ(3, spectrumInfo.nDetectors());
  EXPECT_EQ(&spectrumInfo.l2s().const_ref(), &detectorInfo.l2s().const_ref())
      << "We expect L2s to be shared";
}

TEST(spectrum_info_test, test_spectra_fetch) {

  std::vector<Spectrum> spectra{{0}, {1, 2}, {3, 4, 5}};
  size_t nDetectors = 6;
  DetectorInfoWithMockInstrument tree{
      std::make_shared<testing::NiceMock<MockFlatTree>>(nDetectors),
      SourceSampleDetectorPathFactory<MockFlatTree>{}};
  SpectrumInfo<MockFlatTree> spectrumInfo(spectra, tree);

  EXPECT_EQ(3, spectrumInfo.size());
  EXPECT_EQ(6, spectrumInfo.nDetectors());

  auto spectrum = spectrumInfo.spectrum(0);
  EXPECT_EQ(spectrum, (Spectrum{0}));

  spectrum = spectrumInfo.spectrum(1);
  EXPECT_EQ(spectrum, (Spectrum{1, 2}));

  spectrum = spectrumInfo.spectrum(2);
  EXPECT_EQ(spectrum, (Spectrum{3, 4, 5}));
}

TEST(spectrum_info_test, test_l2) {

  using namespace testing;

  // Create a Detector
  size_t nDetectors = 1;
  MockDetector detector;
  // This is where I place the detector
  EXPECT_CALL(detector, getPos())
      .WillRepeatedly(testing::Return(Eigen::Vector3d{0, 0, 40}));

  // Create an Instrument around the Detector
  auto instrument =
      std::make_shared<testing::NiceMock<MockFlatTree>>(nDetectors);

  EXPECT_CALL(*instrument.get(), startPositions())
      .WillRepeatedly(Return(std::vector<Eigen::Vector3d>{{0, 0, 40}}));
  EXPECT_CALL(*instrument.get(), detectorComponentIndexes())
      .WillRepeatedly(testing::Return(std::vector<size_t>(1, 0)));

  // Create a DetectorInfo around the Instrument
  DetectorInfoWithMockInstrument detectorInfo{
      instrument, SourceSampleDetectorPathFactory<MockFlatTree>{}};

  // Create a SpectrumInfo around the DetectorInfo
  // Single detector in single spectra
  std::vector<Spectrum> spectra{{0}};
  SpectrumInfo<MockFlatTree> spectrumInfo(spectra, detectorInfo);

  // This is the point of the test. Do we calculate L2 correctly for our single
  // spectra.
  EXPECT_NEAR(40.0, spectrumInfo.l2(0), 1e-6);

  EXPECT_TRUE(Mock::VerifyAndClearExpectations(&detector))
      << "Mock Detector used incorrectly";
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(&detectorInfo))
      << "Mock DetectorInfo used incorrectly";
}

TEST(spectrum_info_test, test_l2_mapped) {

  using namespace testing;

  // Create two detectors
  size_t nDetectors = 2;

  auto instrument =
      std::make_shared<testing::NiceMock<MockFlatTree>>(nDetectors);

  EXPECT_CALL(*instrument.get(), startPositions())
      .WillRepeatedly(
          Return(std::vector<Eigen::Vector3d>{{0, 0, 40}, {0, 0, 30}}));
  EXPECT_CALL(*instrument.get(), detectorComponentIndexes())
      .WillRepeatedly(testing::Return(std::vector<size_t>{0, 1}));

  // Create a DetectorInfo around the Instrument
  DetectorInfoWithMockInstrument detectorInfo{
      instrument, SourceSampleDetectorPathFactory<MockFlatTree>{}};

  // Create a SpectrumInfo around the DetectorInfo
  // Two detectors in single spectra
  std::vector<Spectrum> spectra{{0, 1}};
  SpectrumInfo<MockFlatTree> spectrumInfo(spectra, detectorInfo);

  // This is the point of the test. Do we calculate L2 correctly for our dual
  // detector
  // spectra.
  EXPECT_EQ(35.0, spectrumInfo.l2(0)) << "(40 + 30)/2 - 0";
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(instrument.get()))
      << "Mock Instrument used incorrectly";
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(&detectorInfo))
      << "Mock DetectorInfo used incorrectly";
}

}

