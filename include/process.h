#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid): pid(pid) {}
  int Pid() const;                               // TODO: See src/process.cpp
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization();                  // TODO: See src/process.cpp
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator>(Process const& a) const;  // TODO: See src/process.cpp
  float GetCpuUtilization() const;

 private:
  //process pid
  int pid;
  //previous cpuutilization calculation active time
  long prev_active_secs = 0;
  //previous cpuutilization calculation time since startime
  long prev_secs = 0;
  //cpu utilization
  float cpu_util = 0;
};

#endif