#ifndef DETECTOR_INFO_H
#define DETECTOR_INFO_H

#include <memory>
#include <map>
#include "IdType.h"

template
<typename InstTree>
class DetectorInfo {
public:
    DetectorInfo(std::shared_ptr<InstTree>& instrumentTree) : m_instrumentTree(instrumentTree){
        instrumentTree->fillDetectorMap(m_idToIndexMap);
    }
private:
    std::shared_ptr<InstTree> m_instrumentTree;
    std::map<DetectorIdType, size_t> m_idToIndexMap;
};

#endif
