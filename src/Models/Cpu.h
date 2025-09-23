#pragma once

struct Core {
    unsigned int user;
    unsigned int nice;
    unsigned int system;
    unsigned int idle;
    unsigned int iowait;
    unsigned int irq;
    unsigned int softirq;
    unsigned int steal;
    unsigned int guest;
    unsigned int guest_nice;

    Core() {}

    Core(unsigned int user, unsigned int nice, unsigned int system,
         unsigned int idle, unsigned int iowait, unsigned int irq,
         unsigned int softirq, unsigned int steal, unsigned int guest,
         unsigned int guest_nice)
        : user(user),
          nice(nice),
          system(system),
          idle(idle),
          iowait(iowait),
          irq(irq),
          softirq(softirq),
          steal(steal),
          guest(guest),
          guest_nice(guest_nice) {}
};
