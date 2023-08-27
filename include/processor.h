#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
 // Store previous values here
 long _prevIdle{0};
 long _prevActive{0};
 long _prevTotal{0};
};

#endif