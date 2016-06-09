#include "SpectrumInfo.h"
#include "gtest/gtest.h"
#include "DetectorInfo.h"
#include "MockTypes.h"


TEST(spectrum_info_test, test_constructor_lhr){
    std::vector<Spectrum> spectra{{1},{2},{3}};
    size_t nDetectors = 3;
    DetectorInfoWithMockInstrument tree{
        std::make_shared<testing::NiceMock<MockInstrumentTree>>(nDetectors)};
    SpectrumInfo<MockInstrumentTree> spectrumInfo(spectra, tree);

    EXPECT_EQ(3,spectrumInfo.size());
    EXPECT_EQ(3,spectrumInfo.nDetectors());
}

TEST(spectrum_info_test, test_spectra_fetch){

    std::vector<Spectrum> spectra{{1},{2,3},{4,5,6}};
    size_t nDetectors = 6;
    DetectorInfoWithMockInstrument tree{
        std::make_shared<testing::NiceMock<MockInstrumentTree>>(nDetectors)};
    SpectrumInfo<MockInstrumentTree> spectrumInfo(spectra, tree);

    EXPECT_EQ(3,spectrumInfo.size());
    EXPECT_EQ(6,spectrumInfo.nDetectors());

    auto spectrum = spectrumInfo.spectra(0);
    EXPECT_EQ(spectrum, (Spectrum{1}));

    spectrum = spectrumInfo.spectra(1);
    EXPECT_EQ(spectrum, (Spectrum{2,3}));

    spectrum = spectrumInfo.spectra(2);
    EXPECT_EQ(spectrum, (Spectrum{4,5,6}));
}


