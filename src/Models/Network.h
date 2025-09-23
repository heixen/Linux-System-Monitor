#pragma once

#include <string>
struct NetworkInterface {
    std::string name;
    unsigned long rx_bytes;
    unsigned long tx_bytes;

    NetworkInterface() : name(""), rx_bytes(0), tx_bytes(0) {}

    NetworkInterface(std::string &name, unsigned long rx, unsigned long tx)
        : name(name), rx_bytes(rx), tx_bytes(tx) {}
};
