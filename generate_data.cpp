#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <iomanip>

int main() {
    std::ofstream file("data/tick_data.csv");
    if (!file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    std::mt19937 gen(42); // Fixed seed for reproducibility
    std::normal_distribution<double> price_noise(0.0, 0.01);
    std::normal_distribution<double> spread_shock(0.0, 0.08); // Increased volatility
    std::uniform_real_distribution<double> random_shock(0.0, 1.0);

    uint64_t timestamp = 1665158400000000000ULL;
    double price_a = 100.0;
    double price_b = 120.0;
    double spread_mean = price_a - price_b; // -20.0
    double spread = spread_mean;

    // Generate 200,000 ticks (100k per symbol) for realistic testing
    const int num_pairs = 100000;

    std::cout << "Generating realistic pairs trading data..." << std::endl;

    for (int i = 0; i < num_pairs; ++i) {
        // Mean reversion with occasional large shocks
        double mean_reversion = 0.995; // Stronger mean reversion
        spread = spread * mean_reversion + spread_mean * (1.0 - mean_reversion);

        // Add random walk component to spread
        spread += spread_shock(gen);

        // Occasionally add large divergences (10% chance)
        if (random_shock(gen) < 0.1) {
            spread += (random_shock(gen) - 0.5) * 0.5;
        }

        // Update base price B with drift and volatility
        price_b += price_noise(gen);

        // Price A follows B plus the spread
        price_a = price_b + spread + price_noise(gen) * 0.5;

        // Ensure prices stay positive and reasonable
        if (price_a < 50.0) price_a = 50.0;
        if (price_b < 50.0) price_b = 50.0;
        if (price_a > 150.0) price_a = 150.0;
        if (price_b > 150.0) price_b = 150.0;

        // Generate realistic volume
        uint64_t volume_a = 50 + (i * 17 + 13) % 150;
        uint64_t volume_b = 50 + (i * 23 + 7) % 150;

        // Write interleaved ticks with high precision
        file << std::fixed << std::setprecision(4);
        file << timestamp++ << ",SYM_A," << price_a << "," << volume_a << "\n";
        file << timestamp++ << ",SYM_B," << price_b << "," << volume_b << "\n";

        // Progress indicator
        if (i % 10000 == 0) {
            std::cout << "Progress: " << i << " / " << num_pairs
                      << " (" << (i * 100 / num_pairs) << "%)" << std::endl;
        }
    }

    file.close();
    std::cout << "\nGenerated " << (num_pairs * 2) << " ticks in data/tick_data.csv" << std::endl;
    std::cout << "File size: " << (num_pairs * 2 * 50) / 1024 / 1024 << " MB (approx)" << std::endl;

    return 0;
}
