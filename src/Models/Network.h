#pragma once

#include <cstddef>
struct NetworkInterface {
    const char *name;
    unsigned long rx_bytes;
    unsigned long tx_bytes;
};
