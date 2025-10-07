#ifndef SIGNALGENERATOR_H
#define SIGNALGENERATOR_H

#include <vector>
#include <deque>
#include "TickData.h"

enum class Signal { HOLD, GO_LONG, GO_SHORT, GO_FLAT };

class SignalGenerator {
public:
    SignalGenerator(int lookback_window, double entry_threshold, double exit_threshold);
    Signal on_price_update(double price_a, double price_b);

private:
    int lookback_window_;
    double entry_threshold_;
    double exit_threshold_;
    std::deque<double> spread_history_;
    bool is_in_market_ = false;
};

#endif // SIGNALGENERATOR_H
