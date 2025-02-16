#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// Constructor
Process::Process(int pid) {
  // Grab that values that won't change here
  _iPid = pid;
  _sUid = LinuxParser::Uid(_iPid);
  _sUser = LinuxParser::User(_iPid);
  _sCommand = LinuxParser::Command(_iPid);
}
// Return this process's ID
int Process::Pid() const { return _iPid; }

// Return this process's CPU utilization
float Process::CpuUtilization() {
  // Get new values
  long newUpTime = LinuxParser::UpTime(_iPid);
  long newActiveTime = LinuxParser::ActiveJiffies(_iPid) / sysconf(_SC_CLK_TCK);

  // Calculate Deltas
  float deltaTotal = float(newUpTime) - float(_iPrevUpTime);
  float deltaActive = float(newActiveTime) - float(_iPrevActive);

  float utilization{0};
  if (deltaTotal > 0) {
    utilization = deltaActive / deltaTotal;
  }

  // Save new olds
  _iPrevActive = newActiveTime;
  _iPrevUpTime = newActiveTime;

  return utilization;
}

// Return the command that generated this process
string Process::Command() const { return _sCommand; }

// Return this process's memory utilization
string Process::Ram() const {
  long megaBytes = stol(LinuxParser::Ram(_iPid)) / 1000;
  return to_string(megaBytes);
}

// Return the user (name) that generated this process
string Process::User() const { return _sUser; }

// Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(_iPid); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  // We can use CPU Utilization or RAM to sort
  return stol(Ram()) < stol(a.Ram());
  // return CpuUtilization() < a.CpuUtilization();
}