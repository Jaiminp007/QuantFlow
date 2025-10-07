#include "DataReader.h"
#include <sstream>
#include <iostream>

DataReader::DataReader(const std::string& filepath) {
    file_stream_.open(filepath);
    if (!file_stream_.is_open()) {
        std::cerr << "Error: Could not open file " << filepath << std::endl;
    }
}

bool DataReader::next_tick(TickData& tick) {
    std::string line;
    if (!std::getline(file_stream_, line)) {
        return false;
    }

    // Parse CSV line efficiently
    size_t pos1 = line.find(',');
    size_t pos2 = line.find(',', pos1 + 1);
    size_t pos3 = line.find(',', pos2 + 1);

    if (pos1 == std::string::npos || pos2 == std::string::npos || pos3 == std::string::npos) {
        return false;
    }

    // Parse timestamp
    tick.timestamp = std::stoull(line.substr(0, pos1));

    // Parse symbol
    tick.symbol = line.substr(pos1 + 1, pos2 - pos1 - 1);

    // Parse price
    tick.price = std::stod(line.substr(pos2 + 1, pos3 - pos2 - 1));

    // Parse volume
    tick.volume = std::stoull(line.substr(pos3 + 1));

    return true;
}
