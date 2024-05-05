#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;


Processor& System::Cpu() { 
    return cpu_; 
}

bool sortProcesses(Process a, Process b){
    return (a>b);
}

vector<Process>& System::Processes() { 
    vector<int> lista_pid = LinuxParser::Pids();
    if (processes_.empty()){
        for (int i : lista_pid){
            processes_.push_back(Process(i));
        }
    }else{
        bool in = false;
        int size = processes_.size();
        for (int j : lista_pid){
            for (int i = 0; i < size; i++){
                if (j == processes_[i].Pid()){
                    in = true;
                }
            }
            if (!in){
                processes_.push_back(Process(j));
            }
            in = false;
        }
    }

    std::sort(processes_.begin(),processes_.end(),sortProcesses);
    return processes_; 
}

std::string System::Kernel() { 
    return LinuxParser::Kernel(); 
}

float System::MemoryUtilization() { 
    return LinuxParser::MemoryUtilization(); 
}

std::string System::OperatingSystem() { 
    return LinuxParser::OperatingSystem(); 
}

int System::RunningProcesses() { 
    return LinuxParser::RunningProcesses(); 
}

int System::TotalProcesses() { 
    return LinuxParser::TotalProcesses(); 
}

long int System::UpTime() { 
    return LinuxParser::UpTime(); 
}