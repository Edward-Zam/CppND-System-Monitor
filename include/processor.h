#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

  // Declare any necessary private members
 private:
 // Store previous values here
 long _prevIdle{0};
 long _prevTotal{0};
};

#endif