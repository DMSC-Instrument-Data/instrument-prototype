#ifndef DETECTOR_H
#define DETECTOR_H

#include "Component.h"
#include "IntToType.h"

using DetectorIdType = IntToType<1, size_t>;

/**
 * Pure abstract detector
 */
class Detector : public Component {
public:
    virtual DetectorIdType detectorId() const = 0;
    virtual ~Detector(){}
};

#endif
