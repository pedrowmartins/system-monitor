#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() const { 
    return pid; 
}

float Process::GetCpuUtilization() const{
    return cpu_util;
}

float Process::CpuUtilization() {
    long uptime;
    string line, substring, upt, idt;
    std::ifstream stream_(LinuxParser::kProcDirectory + LinuxParser::kUptimeFilename);
    if (stream_.is_open()){
        std::getline(stream_,line);
        std::istringstream linestream_(line);
        linestream_ >> upt >> idt;
        try{
            uptime = std::stol(upt);
        }catch (std::invalid_argument &e){
            uptime = static_cast<long>(0);
        }
        
    }
    long active_jiffies = LinuxParser::ActiveJiffies(pid);
    long starttime;
    long hertz = sysconf(_SC_CLK_TCK);
    int i = 0;
    std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kStatFilename);
    if (stream.is_open()){
        std::getline(stream,line);
        std::istringstream linestream(line);
        while (linestream >> substring){
            i++;
            if (i==22){
                try{
                    starttime = std::stol(substring);
                }catch (std::invalid_argument &e){
                    starttime = static_cast<long>(0);
                }
                
            }
        }
    }
    cpu_util = (static_cast<float>(active_jiffies)/hertz - prev_active_secs)/(uptime - prev_secs - static_cast<float>(starttime)/hertz);
    prev_active_secs = static_cast<float>(active_jiffies)/hertz;
    prev_secs = uptime-static_cast<float>(starttime)/hertz;
    return cpu_util; 
}

string Process::Command() { 
    return LinuxParser::Command(pid); 
}

string Process::Ram() { 
    return LinuxParser::Ram(pid); 
}

string Process::User() { 
    return LinuxParser::User(pid); 
}

long int Process::UpTime() { 
    return LinuxParser::UpTime(pid); 
}

bool Process::operator>(Process const& a) const { 
    return (cpu_util > a.GetCpuUtilization()); 
}