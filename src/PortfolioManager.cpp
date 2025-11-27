#include "PortfolioManager.h"

PortfolioManager::PortfolioManager(double initial_cash)
    : cash_(initial_cash) {
    pnl_history_.reserve(10000000); // Pre-allocate for performance
}

void PortfolioManager::execute_trade(Signal signal, double price_a, double price_b) {
    int quantity = 100;

    switch (signal) {
        case Signal::GO_LONG:
            // Buy A, Sell B
            position_a_ += quantity;
            position_b_ -= quantity;
            cash_ -= price_a * quantity;
            cash_ += price_b * quantity;
            break;

        case Signal::GO_SHORT:
            // Sell A, Buy B
            position_a_ -= quantity;
            position_b_ += quantity;
            cash_ -= price_a * quantity;
            cash_ += price_b * quantity;
            break;

        case Signal::GO_FLAT:
            // Close all positions
            cash_ += position_a_ * price_a;
            cash_ += position_b_ * price_b;
            position_a_ = 0;
            position_b_ = 0;
            break;

        case Signal::HOLD:
            // Do nothing
            break;
    }
}

void PortfolioManager::update_market_value(double price_a, double price_b) {
    double equity = cash_ + position_a_ * price_a + position_b_ * price_b;
    pnl_history_.push_back(equity);
}

const std::vector<double>& PortfolioManager::get_pnl_history() const {
    return pnl_history_;
}
