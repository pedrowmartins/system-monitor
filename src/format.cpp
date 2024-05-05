#include <string>
#include <cmath>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int hours = floor(seconds/3600);
    int mins = floor((seconds - hours*3600)/60);
    int secs = seconds - hours*3600 - mins*60; 
    return Format::TimeToString(hours) + ":" + Format::TimeToString(mins) + ":" + Format::TimeToString(secs); 
}

string Format::TimeToString(int number){
    return number<10 ? "0"+std::to_string(number) : std::to_string(number);
}