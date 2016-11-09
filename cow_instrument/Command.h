#ifndef COMMAND_H
#define COMMAND_H

#include "cow_ptr.h"
#include "Component.h"

class Command{
  public:
    virtual bool execute(CowPtr<Component> &component) const = 0;
    virtual bool isMetaDataCommand() const = 0;
    virtual ~Command() {}
};

/**
 * TODO. Deprecate Command with this.
 */
class Command2 {
public:
  virtual void execute(size_t componentIndex) const = 0;
  virtual bool isMetaDataCommand() const = 0;
  virtual ~Command2() {}
};

#endif
