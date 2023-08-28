#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <experimental/filesystem>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids(){
  vector<int>pids;
  for(const auto& entry : std::experimental::filesystem::directory_iterator(kProcDirectory))
  {
    if (std::experimental::filesystem::is_directory(entry.status()))
    {
      string filename(entry.path().filename().string());
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  // For memory utilization use: MemTotal - MemFree so only ready the first two lines
  string key, kbString;
  float MemTotal{1};
  float MemFree{0};
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key >> MemTotal >> kbString;

    std::getline(stream, line);
    std::istringstream linestream2(line);
    linestream2 >> key >> MemFree >> kbString;

  }

  return (MemTotal - MemFree) / MemTotal;
}

// Read and return the system uptime (in seconds)
long LinuxParser::UpTime() { 
  // The first value of /proc/uptime
  long upTime{0};
  long idleTime{0};
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  string line;
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime >> idleTime;
  }

  return upTime; 
}

// Return active + idle jiffies
long LinuxParser::Jiffies() { 
  return ActiveJiffies() + IdleJiffies(); 
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  long activeJiffies{0};
  string line, value;
  // Store elements of the file in a vector of strings, we'll convert the elements we want
  // into long types afterwards
  vector<string> values;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if( stream.is_open() )
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while( linestream >> value )
    {
      values.push_back(value);
    }
  }
  else
  {
    // File failed to open, return 0
    return 0;
  }
  
  // Extract the values we want (should we check that they numerical strings?)
  long utime{0}; // CPU time spent in user code
  long stime{0}; // CPU time spend in kernel code
  long cutime{0};// Waited-for children's CPU time spent in user code
  long cstime{0};// Waited-for children's CPU time spend in kernel code
  
  utime = stol(values[13]);
  stime = stol(values[14]);
  cutime = stol(values[15]);
  cstime = stol(values[16]);
  activeJiffies = utime + stime + cutime + cstime;
  
  return activeJiffies;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  std::vector<string> totalJiffies = CpuUtilization();
  if( totalJiffies.size() == 0 )
  {
    return 0;
  }
  long activeJiffies = stol(totalJiffies[CPUStates::kUser_]) + stol(totalJiffies[CPUStates::kNice_]) + 
    				  stol(totalJiffies[CPUStates::kSystem_]) + stol(totalJiffies[CPUStates::kIRQ_]) + 
    				  stol(totalJiffies[CPUStates::kSoftIRQ_]);
  
  return activeJiffies;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  std::vector<string> totalJiffies = CpuUtilization();
  if( totalJiffies.size() == 0 )
  {
    return 0;
  }
  long idleJiffies = stol(totalJiffies[CPUStates::kIdle_]) + stol(totalJiffies[CPUStates::kIOwait_]) + 
    				  stol(totalJiffies[CPUStates::kSteal_]);
  
  return idleJiffies;
}

// This is a function creates a vector of strings that contain the values of all jiffies
vector<string> LinuxParser::CpuUtilization() { 
  // Read the first line of /proc/stat (CPU Utilization)
  string line, value;
  vector<string> jiffies;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if( stream.is_open() )
  {
    // Thrown away the first value ("cpu") and look through the rest
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
    while( linestream >> value )
    {
      jiffies.push_back(value);
    }
  }
  return jiffies; 
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line, key;
  int value{0};
  if (stream.is_open())
  {
    while(std::getline(stream, line))
    {
      std::istringstream stringstream(line);
      if( stringstream >> key >> value )
      {
        if(key == sProcessName)
        {
          return value;
        }
      }
    }
  }
  return value; 
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line, key;
  int value{0};
  if (stream.is_open())
  {
    while(std::getline(stream, line))
    {
      std::istringstream stringstream(line);
      if( stringstream >> key >> value )
      {
        if(key == sRunningProcessesName)
        {
          return value;
        }
      }
    }
  }
  return value; 
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string command;
  vector<string> values;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if( stream.is_open() )
  {
    std::getline(stream, command);
  }
  return command; 
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line, key, value{"0"};
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if( stream.is_open() )
  {
    while( std::getline(stream, line) )
    {
     std::istringstream linestream(line);
      linestream >> key;
      if( key == sVmSize )
      {
        linestream >> value;
        return value;
      }
    }
  }
  return value; 
}

// Read and return the user ID associated with a process
// We'll need this to find the user attached to a process
string LinuxParser::Uid(int pid) { 
  string line, key, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if( stream.is_open() )
  {
    while( std::getline(stream, line) )
    {
     std::istringstream linestream(line);
      linestream >> key;
      if( key == sUID )
      {
       linestream >> value;
        return value;
      }
    }
  }
  return value; 
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line, user, x, uid;
  string userId = Uid(pid);
  // The UID "0" is reserved for the default root account, return "root"
  if( userId == "0" )
  {
    return "root";
  }
 
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kPasswordPath);
  if( stream.is_open() )
  {
    while( std::getline(stream, line, ':') )
    {
      std::istringstream linestream(line);
      linestream >> user >> x >> uid;
      if( uid == userId )
      {
        return user;
      }
    }
  }
  
  return user; 
}

// Read and return the uptime of a process, in seconds
long LinuxParser::UpTime(int pid) {
  string line, value;
  long upTime{0};
  // Store elements of the file in a vector of strings, we'll convert the elements we want
  // into long types afterwards
  vector<string> values;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if( stream.is_open() )
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while( linestream >> value )
    {
      values.push_back(value);
    }
  }
  else
  {
    // File failed to open, return 0
    return 0;
  }
  
  //This is where it gets a little tricky, the starttime is "the time the process started
  // after system boot" and not uptime directly. So grab the system uptime and subtract
  // starttime to get pid uptime.
  long systemUptime = UpTime(); // This is already in seconds
  long startTime = stol(values[21]) / sysconf(_SC_CLK_TCK); // see proc man page
  upTime = systemUptime - startTime;
  
  return upTime;
}
