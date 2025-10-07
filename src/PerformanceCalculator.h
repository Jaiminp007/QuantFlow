#ifndef PERFORMANCECALCULATOR_H
#define PERFORMANCECALCULATOR_H

#include <vector>
#include <iostream>

class PerformanceCalculator {
public:
    explicit PerformanceCalculator(const std::vector<double>& pnl_history);
    void print_report();

private:
    double calculate_sharpe_ratio() const;
    double calculate_max_drawdown() const;
    const std::vector<double>& pnl_history_;
};

#endif // PERFORMANCECALCULATOR_H
