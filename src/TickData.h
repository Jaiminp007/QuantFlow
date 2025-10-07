#ifndef TICKDATA_H
#define TICKDATA_H

#include <string>
#include <cstdint>

struct TickData {
    uint64_t timestamp;
    std::string symbol;
    double price;
    uint64_t volume;
};

#endif // TICKDATA_H
