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
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  std::cerr << "PIDS:" << std::endl;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      std::cerr << filename << std::endl;
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

vector<int> LinuxParser::Pids2(){
  vector<int>pids;
  //auto it = std::experimental::filesystem::directory_iterator("/proc/");
  for(const auto& entry : std::experimental::filesystem::directory_iterator(kProcDirectory))
  {
    if (std::experimental::filesystem::is_directory(entry.status()))
    {
      string filename(entry.path().string());
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
  // TODO: Check if we want in kB or percent
  string key, kbString;
  float MemTotal{0};
  float MemFree{0};
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key >> MemTotal >> kbString;

    std::getline(stream, line);
    linestream.str(line);
    linestream >> key >> MemFree >> kbString;

  }
  return MemTotal - MemFree;
}

// TODO: Read and return the system uptime
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
  // do we want to convert to days or hours?
  return upTime; 
}

// We don't need to read this file 3 times just for jiffies, read once
// and set values;
long LinuxParser::Jiffies() { 
  long totalJiffies{0};
  long jiffie;
  vector<string> jiffies = CpuUtilization();
  // We're going to consider the following:
  // Active: kUser_, kNice_, kSystem_, kIRQ_, kSoftIRQ_
  // inactive: kIdle_, kIOwait_, kSteal_
  // total: Active + inactive + kGuest_ + kGuestNice_
  for(auto i=0; i<10; i++)
  {
	jiffie = stol(jiffies[i]);
    if ( i == CPUStates::kUser_ || i == CPUStates::kNice_ || i == CPUStates::kSystem_ ||
        i == CPUStates::kIRQ_ || i == CPUStates::kSoftIRQ_ )
    {
      _activeJiffies += jiffie;
      totalJiffies += jiffie;
    }
    else if ( i ==  CPUStates::kIdle_ || i == CPUStates::kIOwait_ || i == CPUStates::kSteal_ )
    {
      _idleJiffies += jiffie;
      totalJiffies += jiffie;
    }
    else
    {
     totalJiffies += jiffie;
    }
  }
  return totalJiffies; 
}

// Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return _activeJiffies; }

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return _idleJiffies; }

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
        if(key == kProcessName)
        {
          return value;
        }
      }
    }
  }

  return 0; 
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
        if(key == kRunningProcessesName)
        {
          return value;
        }
      }
    }
  }

  return 0; 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
