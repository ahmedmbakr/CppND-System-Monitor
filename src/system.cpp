#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"
#include "cassert"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Return the system's CPU
Processor& System::Cpu() 
{ 
    auto cpuUtilizations = LinuxParser::CpuUtilization();
    //assert(cpuUtilizations.size() == 10);
    this->cpu_.SetCpuUtilizations(cpuUtilizations);
    this->cpu_.Utilization();

    return cpu_; 
}

// Return a container composed of the system's processes
vector<Process>& System::Processes() {
    this->processes_.clear();
    auto pids = LinuxParser::Pids();
    for(auto pid : pids)
    {
        Process process{};
        process.pid_ = pid;
        process.command_ = LinuxParser::Command(pid);
        process.ram_ = LinuxParser:: Ram(pid);
        process.user_ = LinuxParser::User(pid);
        process.upTime_ = LinuxParser::UpTime(pid);
        process.cpuUtilization_ = LinuxParser::ProcessUtilization(pid);
        this->processes_.push_back(process);
    }
    std::sort(this->processes_.begin(), this->processes_.end());
    return this->processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() 
{
    return LinuxParser::Kernel();  
}

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }
