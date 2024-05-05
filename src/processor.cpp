#include "processor.h"
#include "linux_parser.h"
#include <iostream>

float Processor::Utilization() { 
    float util = static_cast<float>(LinuxParser::ActiveJiffies()-prev_active_jiffs)/(LinuxParser::Jiffies()-prev_jiffs);
    prev_jiffs = LinuxParser::Jiffies();
    prev_active_jiffs = LinuxParser::ActiveJiffies();
    return util;
}