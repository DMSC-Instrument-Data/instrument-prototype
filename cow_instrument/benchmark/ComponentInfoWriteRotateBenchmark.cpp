#include "StandardInstrument.h"
#include <benchmark/benchmark_api.h>

#include <stdexcept>
namespace {

class ComponentInfoWriteRotateFixture : public StandardInstrumentFixture {

public:
  void rotateOnComponent(size_t componentIndex, bool read,
                         benchmark::State &state) {

    Eigen::Vector3d axis{0, 0, 1};
    auto angle = M_PI / 2;
    Eigen::Vector3d center{0, 0, 0};

    while (state.KeepRunning()) {
      // Then modify that node
      m_componentInfo.rotate(componentIndex, axis, angle, center);

      // If we want to compare reads too.
      if (read) {
        Eigen::Vector3d pos;
        size_t nComponents =
            m_componentInfo.const_instrumentTree().componentSize();
        for (size_t i = 0; i < nComponents; ++i) {
          benchmark::DoNotOptimize(pos += m_componentInfo.position(i));
        }
      }
    }
    // For statistics. Mark the number of itertions
    state.SetItemsProcessed(state.iterations() * 1);
  }
};

BENCHMARK_F(ComponentInfoWriteRotateFixture,
            BM_rotate_root)(benchmark::State &state) {
  this->rotateOnComponent(0, false /*no read metric*/, state);
}

BENCHMARK_F(ComponentInfoWriteRotateFixture,
            BM_rotate_one_trolley)(benchmark::State &state) {
  this->rotateOnComponent(1, false /*no read metric*/, state);
}

BENCHMARK_F(ComponentInfoWriteRotateFixture,
            BM_rotate_one_bank)(benchmark::State &state) {
  this->rotateOnComponent(2, false /*no read metric*/, state);
}

BENCHMARK_F(ComponentInfoWriteRotateFixture,
            BM_rotate_one_bank_with_pos_read)(benchmark::State &state) {
  this->rotateOnComponent(2, true /*with read metric*/, state);
}

// Just gets us the execution policy
class RotationBenchmark : public StandardBenchmark<RotationBenchmark> {};

BENCHMARK_F(RotationBenchmark,
            BM_rotation_one_bank_math)(benchmark::State &state) {
  // Check the rotation math independenly of other implementation details.

  // One bank has 100 * 100 detectors
  std::vector<Eigen::Vector3d> positions(10000);
  std::vector<Eigen::Quaterniond> rotations(10000,
                                            Eigen::Quaterniond::Identity());
  auto theta = M_PI;
  auto axis = Eigen::Vector3d{1, 0, 0};
  auto center = Eigen::Vector3d{0, 0, 0};

  while (state.KeepRunning()) {
    using namespace Eigen;
    Affine3d A = Translation3d(center) * AngleAxisd(theta, axis) *
                 Translation3d(-center);
    Quaterniond rotationPart(A.rotation());
    for (auto &pos : positions) {
      pos = A * pos;
    }
    for (auto &rot : rotations) {
      rot = rotationPart * rot;
    }
  }
  // For statistics. Mark the number of itertions. In this case we want to
  // compare against a single bank.
  state.SetItemsProcessed(state.iterations() * 1);
}

} // namespace
