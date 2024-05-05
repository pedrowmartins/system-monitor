#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

//Auxilary function to get value from a key in a file
string getValueFromFileKey(string const &filedirectory, string const &key_filter){
  string line, key, value;
  std::ifstream stream(filedirectory);
  if (stream.is_open()){
    while (std::getline(stream, line)){
      //If the key is present in the line
      if (line.find(key_filter) != string::npos){
        std::istringstream linestream(line);
        //Take the value from the line
        linestream >> key >> value;
        return value;
      }
    }
  }
  return value;
}

//Auxiliary function to get value from a file with an index
string getValueFromFileIndex(string const &filedirectory, int const &index){
  string line, value;
  int i = 0;
  std::ifstream stream(filedirectory);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value){
      i++;
      if (i==index){
        return value;
      }
    }
  }
  return string();
}

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
  string os, version, kernel;
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
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() { 
  string m_total = getValueFromFileKey(kProcDirectory + kMeminfoFilename, "MemTotal");
  string m_free = getValueFromFileKey(kProcDirectory + kMeminfoFilename, "MemFree");
  try{
    return static_cast<float>(std::stol(m_total) - std::stol(m_free))/(std::stol(m_total)); 
  }catch (std::invalid_argument &e){
    return static_cast<float>(0);
  }
}

long LinuxParser::UpTime() { 
  string uptime, line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);
    linestream >> uptime;
    try{
      return std::stol(uptime);
    }catch (std::invalid_argument &e){
      return static_cast<long>(0);
    }
    
  }
  return static_cast<long>(0);
}

long LinuxParser::Jiffies() { 
  string line, substring;
  long jiffies[10];
  int i = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);
    //passes through all the elements in linestream object
    while (linestream >> substring){
      if (i>0){
        try{
          jiffies[i-1] = std::stol(substring);
        }catch (std::invalid_argument &e){
          jiffies[i-1] = static_cast<long>(0);
        }
        
      }
      i++;
    }
  }
  return LinuxParser::LongArraySum(jiffies,10); 
}

long LinuxParser::LongArraySum(long array[], int size){
  long int sum = 0;
  for (int i = 0; i < size; i++){
    sum += array[i];
  }
  return sum;
}

long LinuxParser::ActiveJiffies(int pid) {
  long active_jiffies[4];
  int const indexes[4] = {14,15,16,17};
  for (int i=0; i<4; i++){
    try{
      active_jiffies[i] = std::stol(getValueFromFileIndex(kProcDirectory + std::to_string(pid) + kStatFilename,indexes[i]));
    }catch (std::invalid_argument &e){
      active_jiffies[i] = static_cast<long>(0);
    }
    
  }

  return LinuxParser::LongArraySum(active_jiffies,4); 
}

long LinuxParser::ActiveJiffies() { 
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies(); 
}

long LinuxParser::IdleJiffies() { 
  long idle[2];
  int const indexes[2] = {4,5};
  for (int i = 0; i<2; i++){
    try{
      idle[i] = std::stol(getValueFromFileIndex(kProcDirectory + kStatFilename,indexes[i]));
    }catch (std::invalid_argument &e){
      idle[i] = static_cast<long>(0);
    }
    
  }
  return LinuxParser::LongArraySum(idle,2);
}

vector<string> LinuxParser::CpuUtilization() { 
  vector<int> pids = LinuxParser::Pids();
  vector<string> cpus;
  long active_jiffies;
  long const uptime = LinuxParser::Jiffies();
  long starttime;
  string line, substring;
  for (int pid : pids){
    active_jiffies = LinuxParser::ActiveJiffies(pid);
    try{
      starttime = std::stol(getValueFromFileIndex(kProcDirectory + std::to_string(pid) + kStatFilename,22));
    }catch (std::invalid_argument &e){
      starttime = static_cast<long>(0);
    }
    
    cpus.push_back(std::to_string(active_jiffies/(uptime-starttime)));
  }
  return cpus; 
}

int LinuxParser::TotalProcesses() { 
  string n_processes = getValueFromFileKey(kProcDirectory + kStatFilename, "processes");
  return std::stoi(n_processes);
}

int LinuxParser::RunningProcesses() { 
  string n_processes = getValueFromFileKey(kProcDirectory + kStatFilename,"procs_running");
  return std::stoi(n_processes);
}


string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  std::getline(stream, line);
  return line;
}


string LinuxParser::Ram(int pid) {
  //I used VmRSS instead of VmSize because it gives me the physical memory being consumed
  //instead of the virtual memory size, as per a Udacity reviewer recommendation https://man7.org/linux/man-pages/man5/proc.5.html
  string size = getValueFromFileKey(kProcDirectory + std::to_string(pid) + kStatusFilename,"VmRSS");
  
  try{
    return std::to_string(static_cast<int>(std::stol(size)*0.001));
  }catch (std::invalid_argument &e){
    return string();
  }
  
}


string LinuxParser::Uid(int pid) { 
  return getValueFromFileKey(kProcDirectory + std::to_string(pid) + kStatusFilename,"Uid");
}


string LinuxParser::User(int pid) { 
  string const uid = LinuxParser::Uid(pid);
  string line, substring;
  vector<string> line_substrings;
  bool i = false;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()){
    while (std::getline(stream,line)){
      std::istringstream linestream(line);
      while (std::getline(linestream,substring,':')){
        line_substrings.push_back(substring);
      }
      for (string substring : line_substrings){
        if (substring == uid){
          i = true;
        }
      }

      if (i){
        return line_substrings[0];
      }
      line_substrings.clear();
    }
  }
  return string();
}


long LinuxParser::UpTime(int pid) {
  try{
    return LinuxParser::UpTime() - std::stol(getValueFromFileIndex(kProcDirectory + std::to_string(pid) + kStatFilename,22))/sysconf(_SC_CLK_TCK);
  }catch (std::invalid_argument &e){
    return static_cast<long>(0);
  }
  
}
