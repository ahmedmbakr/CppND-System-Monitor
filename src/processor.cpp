#include "processor.h"
#include <cassert>

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() 
{ 
    //using this link https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux

    // Guest time is already accounted in usertime
    usertime_ = usertime_ - guesttime_;                     
    nicetime_ = nicetime_ - guestnicetime_;                 
    idletime_ = idletime_ + iowaittime_;                 // ioWait is added in the idleTime
    auto systemalltime = systemtime_ + irqtime_ + softirqtime_;
    auto virtalltime = guesttime_ + guestnicetime_;
    auto totaltime = usertime_ + nicetime_ + systemalltime + idletime_ + stealtime_ + virtalltime;


    return (totaltime- idletime_) * 1.0 / totaltime;
}

void Processor::SetCpuUtilizations(std::vector<int>& cpuUtilizations)
{  
    assert(cpuUtilizations.size() == 10);
    if(cpuUtilizations.size() == 10)
    {
    usertime_ = cpuUtilizations[0];
    nicetime_ = cpuUtilizations[1];
    systemtime_ = cpuUtilizations[2];
    idletime_ = cpuUtilizations[3];
    iowaittime_ = cpuUtilizations[4];
    irqtime_ = cpuUtilizations[5];
    softirqtime_ = cpuUtilizations[6];
    stealtime_ = cpuUtilizations[7];
    guesttime_ = cpuUtilizations[8];
    guestnicetime_ = cpuUtilizations[9];
    }
}
