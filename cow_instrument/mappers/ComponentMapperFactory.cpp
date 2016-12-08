#include "ComponentMapperFactory.h"
#include "ComponentVisitor.h"
#include "CompositeComponentMapper.h"
#include "DetectorComponentMapper.h"
#include "NullComponentMapper.h"
#include "PointSampleMapper.h"
#include "PointSourceMapper.h"
#include <vector>
#include <memory>

std::vector<std::shared_ptr<ComponentVisitor>>
ComponentMapperFactory::createMappers() {

  return std::vector<std::shared_ptr<ComponentVisitor>>{
      std::make_shared<CompositeComponentMapper>(),
      std::make_shared<DetectorComponentMapper>(),
      std::make_shared<NullComponentMapper>(),
      std::make_shared<PointSampleMapper>(),
      std::make_shared<PointSourceMapper>()};
}
