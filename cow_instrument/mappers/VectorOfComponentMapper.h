#ifndef _VECTOROFCOMPONENTMAPPER_H
#define _VECTOROFCOMPONENTMAPPER_H

#include "PolymorphicSerializer.h"
#include "ComponentMapperFactory.h"
#include "VectorOfMapper.h"

using VectorOfComponentMapper =
    VectorOfMapper<PolymorphicSerializer<ComponentMapperFactory>>;

#endif
