#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

/**
 * @param TokenNum Token Number starts from 1
 */ 
long getLongValueFromProcessStatFile(string fileName, int keyTokenNum)
{
  std::ifstream stream(fileName);
  //Get token number 22 from the file
  long token = -1;
  if (stream.is_open()) {
    string line;
    std::getline(stream, line);
    std::istringstream linestream(line);
    string dummy;
    linestream >> dummy >> dummy >> dummy;//consume the first 3 tokens
    int tokenNum = 4;
    while (tokenNum++ != keyTokenNum + 1) {
      linestream >> token;
    }
  }
  stream.close();
  return token;
}

template <typename T>
T parseFileToGetValueOfKey(std::ifstream& stream, std::string key)
{
  string line, akey;
  T value;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> akey >> value;
      if (key == akey ) {
        return value;
      }
    }
  }
  return value;
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
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() 
{
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) 
  {
    string line;
    std::getline(stream, line);

    //Memory total
    string dummy;
    float memTotal;
    std::istringstream linestream(line);
    linestream >> dummy >> memTotal;

    //Memory free
    float memFree;
    std::getline(stream, line);
    linestream.str(line.c_str());
    linestream >> dummy >> memFree;

    return memFree / memTotal;
  }
  return -1;
}

// Read and return the system uptime
long LinuxParser::UpTime() 
{ 
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) 
  {
    string line;
    std::getline(stream, line);

    //Up time
    long uptime;
    std::istringstream linestream(line);
    linestream >> uptime;

    return uptime;
  }
  return -1;
}

//  Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) 
{
  string fileName = kProcDirectory + std::to_string(pid) + kStatFilename;
  long utime = getLongValueFromProcessStatFile(fileName, 14);
  long stime = getLongValueFromProcessStatFile(fileName, 15);
  long cutime = getLongValueFromProcessStatFile(fileName, 16);
  long cstime = getLongValueFromProcessStatFile(fileName, 17);
  long starttime = getLongValueFromProcessStatFile(fileName, 22);
  return utime + stime + cutime + cstime + starttime;
}

// Read and return CPU utilization
vector<int> LinuxParser::CpuUtilization() 
{
  vector<int> jennifs;
  string line;
  string value;
  int intval;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> value;
    while (linestream >> intval) {
      jennifs.push_back(intval);
    }
  }
  return jennifs;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{
  std::ifstream stream(kProcDirectory + kStatFilename);
  int numProcesses = parseFileToGetValueOfKey<int>(stream, "processes");
  return numProcesses;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses()
{
  std::ifstream stream(kProcDirectory + kStatFilename);
  int numProcesses = parseFileToGetValueOfKey<int>(stream, "procs_running");
  return numProcesses;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) 
{
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) 
  {
    string line;
    std::getline(stream, line);
    return line;
  }
  return string();
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  long ramUsageinMega = parseFileToGetValueOfKey<float>(stream, "VmSize:") / 1024;
  return std::to_string(ramUsageinMega);  
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) 
{
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  auto uid = parseFileToGetValueOfKey<int>(stream, "Uid:");
  return std::to_string(uid);
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) 
{ 
  std::ifstream stream(kPasswordPath);
  string uid = Uid(pid);
  string userName = "not-found";
  if (stream.is_open()) {
    string line, dummy, aUserName;
    int aUid;
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':',' ');
      std::istringstream linestream(line);
      linestream >> aUserName >> dummy >> aUid;
      if (std::to_string(aUid).compare(uid) == 0 ) {
        userName = aUserName;
        break;
      }
    }
  }
  return userName;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) 
{ 
  //Get token number 22 from the file
  long uptime = getLongValueFromProcessStatFile(kProcDirectory + std::to_string(pid) + kStatFilename, 22);
  return uptime / sysconf(_SC_CLK_TCK);
}

// Implement process utilization for a process
float LinuxParser::ProcessUtilization(int pid)
{ 
  string fileName = kProcDirectory + std::to_string(pid) + kStatFilename;
  long utime = getLongValueFromProcessStatFile(fileName, 14);
  long stime = getLongValueFromProcessStatFile(fileName, 15);
  long cutime = getLongValueFromProcessStatFile(fileName, 16);
  long cstime = getLongValueFromProcessStatFile(fileName, 17);
  long starttime = getLongValueFromProcessStatFile(fileName, 22);

  long total_time = utime + stime;
  total_time = total_time + cutime + cstime;
  auto system_up_time = UpTime();
  float seconds = system_up_time - (starttime / sysconf(_SC_CLK_TCK));
  float cpu_usage = /* 100 * */ ((total_time / sysconf(_SC_CLK_TCK)) / seconds);
  return cpu_usage;
}

