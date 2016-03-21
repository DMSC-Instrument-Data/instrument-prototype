#ifndef DETECTOR_H
#define DETECTOR_H

#include "Component.h"

/**
 * Pure abstract detector
 */
class Detector : public Component {
public:
    virtual size_t id() const = 0;
    virtual ~Detector(){}
};

#endif
