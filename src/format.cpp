#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int hours = seconds / 3600;
  seconds %= 3600;
  int minutes = seconds / 60;
  seconds %= 60;
  std::string timeFormat = std::to_string(hours) + ":" +
                           std::to_string(minutes) + ":" +
                           std::to_string(seconds);
  return timeFormat;
}