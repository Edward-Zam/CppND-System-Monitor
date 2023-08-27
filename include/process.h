#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
This class will handle conversions to more user-friedly values
*/
class Process {
 public:
  Process(int pid);
  int Pid() const;
  std::string User();                      
  std::string Command();                   
  float CpuUtilization() const;                  
  std::string Ram();                       
  long int UpTime();                       
  bool operator<(Process const& a) const;  

  // TODO: Declare any necessary private members
 private:
  int _iPid;
  long int _iUptime;
  float _fUtilization;
  std::string _sUid;
  std::string _sUser;
  std::string _memory;
  std::string _sCommand;
};

#endif