#include "SignalGenerator.h"
#include <cmath>
#include <numeric>

SignalGenerator::SignalGenerator(int lookback_window, double entry_threshold, double exit_threshold)
    : lookback_window_(lookback_window),
      entry_threshold_(entry_threshold),
      exit_threshold_(exit_threshold) {
}

Signal SignalGenerator::on_price_update(double price_a, double price_b) {
    // Calculate current spread
    double current_spread = price_a - price_b;

    // Add to history
    spread_history_.push_back(current_spread);

    // Maintain window size
    if (static_cast<int>(spread_history_.size()) > lookback_window_) {
        spread_history_.pop_front();
    }

    // Need full window for statistics
    if (static_cast<int>(spread_history_.size()) < lookback_window_) {
        return Signal::HOLD;
    }

    // Calculate mean
    double mean = std::accumulate(spread_history_.begin(), spread_history_.end(), 0.0) / lookback_window_;

    // Calculate standard deviation
    double variance = 0.0;
    for (const auto& spread : spread_history_) {
        variance += (spread - mean) * (spread - mean);
    }
    variance /= lookback_window_;
    double std_dev = std::sqrt(variance);

    // Avoid division by zero
    if (std_dev < 1e-9) {
        return Signal::HOLD;
    }

    // Calculate Z-score
    double z_score = (current_spread - mean) / std_dev;

    // Trading logic
    if (!is_in_market_) {
        if (z_score > entry_threshold_) {
            is_in_market_ = true;
            return Signal::GO_SHORT;
        } else if (z_score < -entry_threshold_) {
            is_in_market_ = true;
            return Signal::GO_LONG;
        }
    } else {
        if (std::abs(z_score) < exit_threshold_) {
            is_in_market_ = false;
            return Signal::GO_FLAT;
        }
    }

    return Signal::HOLD;
}
