#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Constructor
Process::Process(int pid)
{
  // Grab that value that won't change here
  _iPid = pid;
  _sUid = LinuxParser::Uid(_iPid);
  _sUser = LinuxParser::User(_iPid);
  _sCommand = LinuxParser::Command(_iPid);
  
}
// Return this process's ID
int Process::Pid() const { 
  return _iPid;
}

// Return this process's CPU utilization
float Process::CpuUtilization() const { 
  long upTime = LinuxParser::UpTime(_iPid);
  if( upTime > 0 )
  {
    return float(LinuxParser::ActiveJiffies(_iPid_)) / float(upTime);
  }
  return 0;
}

// TODO: Return the command that generated this process
string Process::Command() const {
  return _sCommand;
}

// TODO: Return this process's memory utilization
string Process::Ram() const {
  long megaBytes = stol(LinuxParser::Ram(_iPid)) / 1000;
  return to_string(megaBytes);
}

// TODO: Return the user (name) that generated this process
string Process::User() const {
  return _sUser;
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() const {
  return stol(LinuxParser::UpTime(_iPid) / sysconf(_SC_CLK_TCK));
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }