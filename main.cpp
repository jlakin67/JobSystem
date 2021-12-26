#include <iostream>
#include "JobSystem.h"
#include <chrono>

void testFunc(void* jobArgs) {
    assert(jobArgs);
    auto t0 = std::chrono::high_resolution_clock::now();
    decltype(t0) t1;
    double ms = *reinterpret_cast<double*>(jobArgs);
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
    jobManager.initialize(jobManager.getMaxPossibleThreads());
    Job job;
    double ms = 100.0;
    job.jobArgs = &ms;
    job.func = testFunc;
    job.counter = &jobCounter;
    auto t0 = std::chrono::high_resolution_clock::now();
    job.func(job.jobArgs);
    job.func(job.jobArgs);
    job.func(job.jobArgs);
    job.func(job.jobArgs);
    job.func(job.jobArgs);
    job.func(job.jobArgs);
    auto t1 = std::chrono::high_resolution_clock::now();
    std::cout << "Serial: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << std::endl;
    t0 = std::chrono::high_resolution_clock::now();
    std::array<Job, 2> jobs{ job, job };
    jobManager.jobQueue.push_jobs(jobs.size(), jobs.data());
    jobManager.jobQueue.push_job(job);
    jobCounter.wait();
    jobManager.jobQueue.push_jobs(jobs.size(), jobs.data());
    jobManager.jobQueue.push_job(job);
    jobManager.jobQueue.push_job(job);
    jobCounter.wait();
    t1 = std::chrono::high_resolution_clock::now();
    std::cout << "Parallel: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << std::endl;
}
