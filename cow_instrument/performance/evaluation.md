##Evaluation##

We are attempting to measure and understand the read-performance of the new prototype (a.k.a **mapless Instrument**) compared to the existing Instrument implementation in Mantid. The point of this testing is focused on the Instrument, so aspects relating to spectrum-detector mapping, and workspace owned parameter maps have not been considered. This is purely an evaluation of the Instrument configuration.

The same configuration was used in all tests. A fabricated instrument consisting of 6-panels, each containing 100*100 detectors. Panels where grouped onto one of two trolleys, which where themselves attached to the top-level instrument. Both Instrument v1, and Instrument v2 (mapless instrument) used the same setup.


##existing_no_map##

Mantid implementation with a parameterless instrument. See `instrument_access_performance` branch of the mantid repository. Instrumentation only over the test method `InstrumentTestPerformance::test_access_not_parameterized`

**cycles = 124x10^6**

Profiling reveals that the current implementation of the un-parameterized is inefficient largely due to time spent rotating components w.r.t the parent prior to extracting the position. 56% of the time is spent in `Quat::rotate`. As the instrument is organised with 3-levels in the component hierarchy, we are seeing that 60000 detector position calls requires 180000 rotations required. 2% of the total time is also spent in V3D::+operator() due to the lazy computation of absolute positions from relative ones required.

##existing_with_map##

Mantid implementation with a parameter mapped instrument. See `instrument_access_performance` branch of the mantid repository. Instrumentation only over the test method `InstrumentTestPerformance::test_access_parameterized`

**cycles = 266x10^6**

Profiling reveals that the the current implementation of the parameterised instrument is handling the creating of parameterized components via `ParComponentFactory::create` very inefficiently, which 46% of the time spent performing the internal dynamic cast to determine what to create. With the additional `malloc` associated with fabrication of new heap parameterized, components, the parameterized instrument is **2.1x slower** than its un-parameterized equivalent on the same tests. The parameterized instrument with the position cache, was designed to make access faster, but this appears not to be the case.

##mapless_instrument##

Prototype of an absolute positioned mapless instrument. Prototype is [here](https://github.com/DMSC-Instrument-Data/instrument-prototype/tree/master/cow_instrument). Example has been instrumented as above and can be found [here](https://github.com/DMSC-Instrument-Data/instrument-prototype/blob/master/cow_instrument/testing/Example.cpp). 

**cycles = 9x10^6**

Profiling reveals the expected results. The read implementation is now much simpler, with absolute positions stored directly against the component. This is why we see a **12x faster** w.r.t the un-parameterized existing Instrument implementation and **30x faster**  w.r.t the parameterized existing Instrument implementation.

##Notes##

Timings where almost identical when the examples where built with clang (Apple LLVM version 6.1.0 ) on osx and gcc (gcc version 4.8.2) on linux.

Profiling was achieved with a combination of *callgrind*, and *kcachegrind*











