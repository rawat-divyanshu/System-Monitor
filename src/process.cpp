#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pId) : pId_(pId) {
}

// DONE: Return this process's ID
int Process::Pid() {
    return pId_;
}

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() {
    long sUpTime = LinuxParser::UpTime();
    long pUpTime = LinuxParser::UpTime(pId_);
    long totalTime = LinuxParser::ActiveJiffies(pId_);

    float elapTime = float(sUpTime) - (float(pUpTime)/sysconf(_SC_CLK_TCK));
    cpuUtil_ = 100*(float(totalTime)/sysconf(_SC_CLK_TCK))/elapTime;
    return cpuUtil_;
}

// DONE: Return the command that generated this process
string Process::Command() {
    return LinuxParser::Command(pId_);
}

// DONE: Return this process's memory utilization
string Process::Ram() {
    return LinuxParser::Ram(pId_);
}

// DONE: Return the user (name) that generated this process
string Process::User() {
    return LinuxParser::User(pId_);
}

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() {
    return LinuxParser::UpTime(pId_);
}

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
    return this->cpuUtil_ < a.cpuUtil_;
}