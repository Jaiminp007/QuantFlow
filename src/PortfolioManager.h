#ifndef PORTFOLIOMANAGER_H
#define PORTFOLIOMANAGER_H

#include <vector>
#include "SignalGenerator.h" // For Signal enum

class PortfolioManager {
public:
    explicit PortfolioManager(double initial_cash);
    void execute_trade(Signal signal, double price_a, double price_b);
    void update_market_value(double price_a, double price_b);
    const std::vector<double>& get_pnl_history() const;

private:
    double cash_;
    int position_a_ = 0;
    int position_b_ = 0;
    std::vector<double> pnl_history_;
};

#endif // PORTFOLIOMANAGER_H
