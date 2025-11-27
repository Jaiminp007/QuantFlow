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
    
    // ADJUSTED PARAMETERS FOR REAL DATA
    // Real market is noisier, so we need looser thresholds
    const int lookback = 200;       
    const double entry_z = 2.0;     // Increased from 1.0 to 2.0 to reduce noise trades
    const double exit_z = 0.5;      // Increased from 0.2 to 0.5

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

    // Variables for synchronization
    uint64_t current_timestamp = 0;
    bool data_initialized = false;

    // 2. Event Loop
    while (reader.next_tick(current_tick)) {
        
        // If this is the first tick, set the timestamp
        if (!data_initialized) {
            current_timestamp = current_tick.timestamp;
            data_initialized = true;
        }

        // === SYNCHRONIZATION LOGIC ===
        // If we moved to a NEW timestamp, process the PREVIOUS timestamp's prices
        if (current_tick.timestamp > current_timestamp) {
            
            // Only trade if we have valid prices for both symbols
            if (last_prices["SYM_A"] > 0 && last_prices["SYM_B"] > 0) {
                
                // 1. Update Portfolio Value (Mark to Market)
                portfolio.update_market_value(last_prices["SYM_A"], last_prices["SYM_B"]);

                // 2. Generate Signal
                Signal signal = signal_gen.on_price_update(last_prices["SYM_A"], last_prices["SYM_B"]);

                // 3. Execute Trade
                if (signal != Signal::HOLD) {
                    portfolio.execute_trade(signal, last_prices["SYM_A"], last_prices["SYM_B"]);
                    num_trades++;

                    if (signal == Signal::GO_LONG) num_longs++;
                    else if (signal == Signal::GO_SHORT) num_shorts++;
                    else if (signal == Signal::GO_FLAT) num_closes++;
                }
            }

            // Update timestamp to the new one
            current_timestamp = current_tick.timestamp;
        }

        // Store the price (buffer it)
        last_prices[current_tick.symbol] = current_tick.price;
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