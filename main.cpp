#include <iostream>
#include "JobSystem.h"
#include <chrono>

void testFunc(JobArgs jobArgs) {
    assert(jobArgs.ptr);
    auto t0 = std::chrono::high_resolution_clock::now();
    decltype(t0) t1;
    double ms = *reinterpret_cast<double*>(jobArgs.ptr);
    std::chrono::duration<double, std::milli> dur;
    while (dur.count() < ms) {
        t1 = std::chrono::high_resolution_clock::now();
        dur = t1 - t0;
    }
}

int main()
{
    JobManager<100> jobManager;
    JobCounter jobCounter;
    jobManager.initialize(jobManager.getMaxPossibleThreads(), &jobCounter);
    Job job;
    double ms = 100.f;
    job.jobArgs.ptr = &ms;
    job.jobArgs.len = 1;
    job.func = testFunc;
    auto t0 = std::chrono::high_resolution_clock::now();
    job.func(job.jobArgs);
    job.func(job.jobArgs);
    job.func(job.jobArgs);
    job.func(job.jobArgs);
    auto t1 = std::chrono::high_resolution_clock::now();
    std::cout << "Serial: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << std::endl;
    t0 = std::chrono::high_resolution_clock::now();
    jobManager.jobQueue.push_job(job, &jobCounter);
    jobManager.jobQueue.push_job(job, &jobCounter);
    jobCounter.wait();
    jobManager.jobQueue.push_job(job, &jobCounter);
    jobManager.jobQueue.push_job(job, &jobCounter);
    jobCounter.wait();
    t1 = std::chrono::high_resolution_clock::now();
    std::cout << "Parallel: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << std::endl;
}
