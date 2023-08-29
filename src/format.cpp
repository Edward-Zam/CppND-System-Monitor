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

  std::string sHours, sMinutes, sSeconds;

  // Check for single digits and fix
  if( hours < 10 )
  {
    sHours = "0"+std::to_string(hours);
  }
  else
  {
    sHours = std::to_string(hours);
  }

  if( minutes < 10 )
  {
    sMinutes = "0"+std::to_string(minutes);
  }
  else
  {
    sMinutes = std::to_string(minutes);
  }
  
  if( seconds < 10 )
  {
    sSeconds = "0"+std::to_string(seconds);
  }
  else
  {
    sSeconds = std::to_string(seconds);
  }
  std::string timeFormat = sHours + ":" +
                           sMinutes + ":" +
                           sSeconds;
  return timeFormat;
}