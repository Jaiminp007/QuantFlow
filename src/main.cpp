#include <iostream>
#include <string>
#include <map>
#include "DataReader.h"
#include "SignalGenerator.h"
#include "PortfolioManager.h"
#include "PerformanceCalculator.h"

int main() {
    // 1. Initialization
    const std::string filepath = "data/tick_data.csv";
    const double initial_cash = 100000.0;
    const int lookback = 200;       // Shorter window = more responsive
    const double entry_z = 1.0;     // Lower threshold = more trades
    const double exit_z = 0.2;      // Tighter exit = faster turnaround

    DataReader reader(filepath);
    SignalGenerator signal_gen(lookback, entry_z, exit_z);
    PortfolioManager portfolio(initial_cash);

    TickData current_tick;
    std::map<std::string, double> last_prices;
    last_prices["SYM_A"] = -1.0;
    last_prices["SYM_B"] = -1.0;

    // Trade statistics
    int num_trades = 0;
    int num_longs = 0;
    int num_shorts = 0;
    int num_closes = 0;

    // 2. Event Loop
    while (reader.next_tick(current_tick)) {
        last_prices[current_tick.symbol] = current_tick.price;

        // Ensure we have a price for both symbols before generating signals
        if (last_prices["SYM_A"] > 0 && last_prices["SYM_B"] > 0) {
            // Update portfolio value with the most recent prices
            portfolio.update_market_value(last_prices["SYM_A"], last_prices["SYM_B"]);

            // Generate a signal
            Signal signal = signal_gen.on_price_update(last_prices["SYM_A"], last_prices["SYM_B"]);

            // Execute trade based on signal
            if (signal != Signal::HOLD) {
                portfolio.execute_trade(signal, last_prices["SYM_A"], last_prices["SYM_B"]);
                num_trades++;

                if (signal == Signal::GO_LONG) num_longs++;
                else if (signal == Signal::GO_SHORT) num_shorts++;
                else if (signal == Signal::GO_FLAT) num_closes++;
            }
        }
    }

    // 3. Trade Summary
    std::cout << "\n========== Trade Statistics ==========\n";
    std::cout << "Total Trades:     " << num_trades << "\n";
    std::cout << "Long Entries:     " << num_longs << "\n";
    std::cout << "Short Entries:    " << num_shorts << "\n";
    std::cout << "Position Closes:  " << num_closes << "\n";
    std::cout << "======================================\n";

    // 4. Final Report
    PerformanceCalculator analytics(portfolio.get_pnl_history());
    analytics.print_report();

    std::cout << "Backtest finished." << std::endl;

    return 0;
}
