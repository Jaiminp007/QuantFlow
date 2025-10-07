#include "PerformanceCalculator.h"
#include <cmath>
#include <numeric>
#include <iomanip>

PerformanceCalculator::PerformanceCalculator(const std::vector<double>& pnl_history)
    : pnl_history_(pnl_history) {
}

double PerformanceCalculator::calculate_sharpe_ratio() const {
    if (pnl_history_.size() < 2) {
        return 0.0;
    }

    // Calculate returns
    std::vector<double> returns;
    returns.reserve(pnl_history_.size() - 1);

    for (size_t i = 1; i < pnl_history_.size(); ++i) {
        if (pnl_history_[i - 1] != 0.0) {
            returns.push_back((pnl_history_[i] - pnl_history_[i - 1]) / pnl_history_[i - 1]);
        }
    }

    if (returns.empty()) {
        return 0.0;
    }

    // Calculate mean return
    double mean_return = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();

    // Calculate standard deviation of returns
    double variance = 0.0;
    for (const auto& ret : returns) {
        variance += (ret - mean_return) * (ret - mean_return);
    }
    variance /= returns.size();
    double std_dev = std::sqrt(variance);

    if (std_dev < 1e-9) {
        return 0.0;
    }

    // Annualize: assume 252 trading days, returns are per-tick
    // For simplicity, we scale by sqrt of number of observations to annualize
    double sharpe = (mean_return / std_dev) * std::sqrt(252.0);

    return sharpe;
}

double PerformanceCalculator::calculate_max_drawdown() const {
    if (pnl_history_.empty()) {
        return 0.0;
    }

    double max_value = pnl_history_[0];
    double max_drawdown = 0.0;

    for (const auto& value : pnl_history_) {
        if (value > max_value) {
            max_value = value;
        }

        double drawdown = (max_value - value) / max_value * 100.0;
        if (drawdown > max_drawdown) {
            max_drawdown = drawdown;
        }
    }

    return max_drawdown;
}

void PerformanceCalculator::print_report() {
    std::cout << "\n========== Performance Report ==========\n";

    if (!pnl_history_.empty()) {
        double initial_value = pnl_history_.front();
        double final_value = pnl_history_.back();
        double total_return = ((final_value - initial_value) / initial_value) * 100.0;

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Initial Capital:  $" << initial_value << "\n";
        std::cout << "Final Capital:    $" << final_value << "\n";
        std::cout << "Total Return:     " << total_return << "%\n";
        std::cout << "Sharpe Ratio:     " << std::setprecision(4) << calculate_sharpe_ratio() << "\n";
        std::cout << "Max Drawdown:     " << std::setprecision(2) << calculate_max_drawdown() << "%\n";
    } else {
        std::cout << "No data available for performance calculation.\n";
    }

    std::cout << "========================================\n\n";
}
