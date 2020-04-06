#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <vector>

class Processor {
 public:
    float Utilization();  // TODO: See src/processor.cpp

    void SetCpuUtilizations(std::vector<int>& cpuUtilizations);

  // TODO: Declare any necessary private members
 private:
    int usertime_;
    int nicetime_;
    int systemtime_;
    int idletime_;
    int iowaittime_;
    int irqtime_;
    int softirqtime_;
    int stealtime_;
    int guesttime_;
    int guestnicetime_;
};

#endif