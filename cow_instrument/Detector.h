#ifndef DETECTOR_H
#define DETECTOR_H

#include "Component.h"
#include "IdType.h"



/**
 * Pure abstract detector
 */
class Detector : public Component {
public:
    virtual DetectorIdType detectorId() const = 0;
    virtual ~Detector(){}
};

#endif
