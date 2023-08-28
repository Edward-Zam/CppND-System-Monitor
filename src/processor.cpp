#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
// We're going to calculate aggregate CPU utilization based on Vangelis' response in
// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
float Processor::Utilization() {
  // get new values
  long newTotal = LinuxParser::Jiffies();
  //long newActive = LinuxParser::ActiveJiffies();
  long newIdle = LinuxParser::IdleJiffies();

  // Calculate Deltas
  float deltaTotal = float(newTotal) - float(_prevTotal);
  float deltaIdle = float(newIdle) - float(_prevIdle);

  float utilization = (deltaTotal - deltaIdle) / deltaTotal;

  // Save new olds
  _prevIdle = newIdle;
  _prevTotal = newTotal;

  return utilization;
}