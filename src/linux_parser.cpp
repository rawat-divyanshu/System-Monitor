#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <unistd.h>
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

// DONE : Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float memUtil;
  vector<long> memStat{};
  string line;
  string key, value;
  std::ifstream fileStream(kProcDirectory + kMeminfoFilename);
  if(fileStream.is_open()) {
    while(getline(fileStream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
        if(key == "MemTotal:" || key == "MemFree:" || key == "MemAvailable:" || key == "Buffers:") {
          memStat.push_back(std::stof(value));
        }
      }
    }
  }
  memUtil = 1.0 - (memStat[1]/(memStat[0]-memStat[3]));
  return memUtil;
}

// DONE : Read and return the system uptime
long LinuxParser::UpTime() {
  string line, upTime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime;
  }
  return std::stol(upTime);
}

// DONE : Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long upTime = LinuxParser::UpTime();
  auto sysFrequency = sysconf(_SC_CLK_TCK);
  long totalJiffies = upTime * sysFrequency;
  return totalJiffies;
}

// DONE : Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  long activeJiffies = 0;
  string jiffiesData, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()) {
    int counter = 0;
    std::getline(stream, line);
    std::istringstream lineStream(line);
    while(lineStream >> jiffiesData) {
      if(counter > 1) {
        activeJiffies += std::stol(jiffiesData);
      }
      counter++;
    }
  }
  return activeJiffies;
}

// DONE : Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long idleJiffies = 0;
  string jiffiesData, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()) {
    int counter = 0;
    std::getline(stream, line);
    std::istringstream lineStream(line);
    while(lineStream >> jiffiesData) {
      if(counter == 4 || counter == 5) {
        idleJiffies += std::stol(jiffiesData);
      }
      counter++;
    }
  }
  return idleJiffies;
}

// DONE: Read and return CPU utilization
float LinuxParser::CpuUtilization() {
  float cpuUtil;
  cpuUtil = float(ActiveJiffies() - IdleJiffies())/float(ActiveJiffies());
  return cpuUtil;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int totalProcesses = 0;
  string line, key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()) {
    while(std::getline(stream,line)) {
      std::istringstream lineStream(line);
      while(lineStream >> key >> value) {
        if(key == "processes") {
          totalProcesses = std::stoi(value);
          break;
        }
      }
    }
  }
  return totalProcesses;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int runningProcesses = 0;
  string line, key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()) {
    while(std::getline(stream,line)) {
      std::istringstream lineStream(line);
      while(lineStream >> key >> value) {
        if(key == "procs_running") {
          runningProcesses = std::stoi(value);
          break;
        }
      }
    }
  }
  return runningProcesses;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string command;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if(stream.is_open()) {
    std::getline(stream, command);
  }
  return command;
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string key, value, line, ram;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream lineStream(line);
      while(lineStream >> key >> value) {
        if(key == "VmSize:") {
          ram = std::to_string(std::stoi(value)/1024);
          break;
        }
      }
    }
  }
  return ram;
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, key, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      std::istringstream stream(line);
      while (stream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: Read and return the user associated with a process0
string LinuxParser::User(int pid) {
  string user, epwd, uid, line;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream stream(line);
      while (stream >> user >> epwd >> uid) {
        if (uid == Uid(pid)) {
          return user;
        }
      }
    }
  }
  return "";
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string pStatData, line;
  long pUpTime;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()) {
    while(getline(stream ,line)) {
      std::istringstream lineStream(line);
      int counter = 1;
      while(lineStream >> pStatData) {
        if(counter == 22) {
          pUpTime = std::stol(pStatData);
          break;
        }
      }
    }
  }
  return pUpTime;
}

// DONE : Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  long activeJiffies = 0, pidStatData;
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()) {
    std::getline(stream ,line);
    std::istringstream lineStream(line);
    int counter = 1;
    while(lineStream >> pidStatData) {
      if(counter > 13 && counter < 18){
        activeJiffies += pidStatData;
      }
      counter++;
    }
  return activeJiffies;
 }
 return activeJiffies;
}




















