#ifndef COMMAND_H
#define COMMAND_H

class Component;

class Command{
  public:
    virtual void execute(Component& component) const = 0;
    virtual ~Command() {}
};

#endif
