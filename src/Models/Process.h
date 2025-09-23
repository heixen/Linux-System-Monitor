#pragma once
#include <string>

struct ProcessStat {
    int pid;
    std::string comm;
    char state;
    int ppid;
    int pgrp;
    unsigned long vsize;
    long rss;
    unsigned long utime;
    unsigned long stime;
    unsigned long start_time;
    long threads;

    ProcessStat(int pid, const std::string &comm, char state, int ppid,
                int pgrp, unsigned long vsize, long rss, unsigned long utime,
                unsigned long stime, unsigned long start_time, long threads)
        : pid(pid),
          comm(comm),
          state(state),
          ppid(ppid),
          pgrp(pgrp),
          vsize(vsize),
          rss(rss),
          utime(utime),
          stime(stime),
          start_time(start_time),
          threads(threads) {};
};

struct Process {
    int pid;
    std::string program;
    std::string cmd;
    unsigned int resident_mem;
    unsigned int virtual_mem;
    unsigned int shared_mem;
    int thread;

    // ProcessStat stat;

    Process(int pid, const std::string &program, const std::string &cmd,
            unsigned int resident_mem, unsigned int virtual_mem,
            unsigned int shared_mem, int thread)
        : pid(pid),
          program(std::move(program)),
          cmd(std::move(cmd)),
          resident_mem(resident_mem),
          virtual_mem(virtual_mem),
          shared_mem(shared_mem),
          thread(thread) {}
};
