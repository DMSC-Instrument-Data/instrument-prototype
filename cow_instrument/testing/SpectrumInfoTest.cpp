#include "SpectrumInfo.h"
#include "gtest/gtest.h"
#include "DetectorInfo.h"
#include "MockTypes.h"

TEST(spectrum_info_test, test_constructor_lhr) {
  std::vector<Spectrum> spectra{{0}, {1}, {2}};
  size_t nDetectors = 3;
  auto instrument = std::make_shared<testing::NiceMock<MockInstrumentTree>>(nDetectors);
  DetectorInfoWithMockInstrument detectorInfo{instrument};
  SpectrumInfo<MockInstrumentTree> spectrumInfo(spectra, detectorInfo);

  EXPECT_EQ(3, spectrumInfo.size());
  EXPECT_EQ(3, spectrumInfo.nDetectors());
}

TEST(spectrum_info_test, test_constructor_one_to_one) {
  size_t nDetectors = 3;
  auto instrument =
      std::make_shared<testing::NiceMock<MockInstrumentTree>>(nDetectors);
  DetectorInfoWithMockInstrument detectorInfo{instrument};
  SpectrumInfo<MockInstrumentTree> spectrumInfo(detectorInfo);

  EXPECT_EQ(3, spectrumInfo.size());
  EXPECT_EQ(3, spectrumInfo.nDetectors());
  EXPECT_EQ(&spectrumInfo.l2s().const_ref(), &detectorInfo.l2s().const_ref())
      << "We expect L2s to be shared";
}

TEST(spectrum_info_test, test_spectra_fetch) {

  std::vector<Spectrum> spectra{{0}, {1, 2}, {3, 4, 5}};
  size_t nDetectors = 6;
  DetectorInfoWithMockInstrument tree{
      std::make_shared<testing::NiceMock<MockInstrumentTree>>(nDetectors)};
  SpectrumInfo<MockInstrumentTree> spectrumInfo(spectra, tree);

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
      .WillRepeatedly(testing::Return(V3D{0, 0, 40}));

  // Create an Instrument around the Detector
  auto instrument =
      std::make_shared<testing::NiceMock<MockInstrumentTree>>(nDetectors);
  EXPECT_CALL(*instrument.get(), getDetector(_))
      .WillRepeatedly(ReturnRef(detector));

  EXPECT_CALL(*instrument.get(), samplePos()).WillOnce(Return(V3D{0, 0, 20}));
  MockDetector mockDetector;

  // Create a DetectorInfo around the Instrument
  DetectorInfoWithMockInstrument detectorInfo{instrument};

  // Create a SpectrumInfo around the DetectorInfo
  // Single detector in single spectra
  std::vector<Spectrum> spectra{{0}};
  SpectrumInfo<MockInstrumentTree> spectrumInfo(spectra, detectorInfo);

  // This is the point of the test. Do we calculate L2 correctly for our single
  // spectra.
  EXPECT_EQ(20.0, spectrumInfo.l2(0));

  EXPECT_TRUE(Mock::VerifyAndClearExpectations(&detector))
      << "Mock Detector used incorrectly";
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(&detectorInfo))
      << "Mock DetectorInfo used incorrectly";
}

TEST(spectrum_info_test, test_l2_mapped) {

  using namespace testing;

  // Create two detectors
  size_t nDetectors = 2;
  MockDetector detectorA, detectorB;
  // This is where I place one of the the detectors
  EXPECT_CALL(detectorA, getPos())
      .WillRepeatedly(testing::Return(V3D{0, 0, 40}));
  // This is where I place the other of the the detectors
  EXPECT_CALL(detectorB, getPos())
      .WillRepeatedly(testing::Return(V3D{0, 0, 30}));

  auto instrument =
      std::make_shared<testing::NiceMock<MockInstrumentTree>>(nDetectors);
  EXPECT_CALL(*instrument.get(), getDetector(_))
      .WillOnce(ReturnRef(detectorA))
      .WillOnce(ReturnRef(detectorB));
  EXPECT_CALL(*instrument.get(), samplePos()).WillOnce(Return(V3D{0, 0, 20}));

  // Create a DetectorInfo around the Instrument
  DetectorInfoWithMockInstrument detectorInfo{instrument};

  // Create a SpectrumInfo around the DetectorInfo
  // Two detectors in single spectra
  std::vector<Spectrum> spectra{{0, 1}};
  SpectrumInfo<MockInstrumentTree> spectrumInfo(spectra, detectorInfo);

  // This is the point of the test. Do we calculate L2 correctly for our dual
  // detector
  // spectra.
  EXPECT_EQ(15.0, spectrumInfo.l2(0)) << "(40 + 30)/2 - 20";
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(&detectorA))
      << "Mock Detector used incorrectly";
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(&detectorB))
      << "Mock Detector used incorrectly";
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(&detectorInfo))
      << "Mock DetectorInfo used incorrectly";
}

TEST(spectrum_info_test, test_modify) {

  using namespace testing;
  auto *pMockInstrumentTree = new NiceMockInstrumentTree{};

  // We expect that the modify method of the existing instrument tree gets
  // called
  EXPECT_CALL(*pMockInstrumentTree, modifyProxy(testing::_, testing::_))
      .Times(1);

  SpectrumInfo<MockInstrumentTree> spectrumInfo(DetectorInfoWithMockInstrument{
      std::shared_ptr<MockInstrumentTree>{pMockInstrumentTree}});

  MockCommand command;
  spectrumInfo.modify(0, command);

  // test modify called on instrument.
  EXPECT_TRUE(testing::Mock::VerifyAndClear(pMockInstrumentTree));
}
