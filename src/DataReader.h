#ifndef DATAREADER_H
#define DATAREADER_H

#include <string>
#include <fstream>
#include "TickData.h"

class DataReader {
public:
    explicit DataReader(const std::string& filepath);
    bool next_tick(TickData& tick); // Returns false if EOF

private:
    std::ifstream file_stream_;
};

#endif // DATAREADER_H
