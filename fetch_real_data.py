import requests
import csv
from datetime import datetime
import time

# ================= CONFIGURATION =================
API_KEY = "ea26437639584f8fac81dd87583664aa"  # <--- PASTE YOUR KEY HERE
SYMBOL_1 = "PEP"   # Asset A (e.g., Pepsi) -> Will become SYM_A
SYMBOL_2 = "PG"    # Asset B (e.g., Coca-Cola) -> Will become SYM_B
INTERVAL = "1min"  # 1min, 5min, 1h, 1day
OUTPUT_SIZE = 5000 # Max 5000 for free tier
OUTPUT_FILE = "data/tick_data.csv"
# =================================================

def fetch_data(symbol):
    print(f"Fetching data for {symbol}...")
    url = f"https://api.twelvedata.com/time_series?symbol={symbol}&interval={INTERVAL}&apikey={API_KEY}&outputsize={OUTPUT_SIZE}"
    response = requests.get(url)
    data = response.json()
    
    if "values" not in data:
        print(f"Error fetching {symbol}: {data}")
        return []
    
    # Twelve Data returns newest first; we need oldest first for backtesting
    return list(reversed(data["values"]))

def convert_to_nanoseconds(date_str):
    # Twelve Data format: "2023-11-20 15:30:00"
    dt = datetime.strptime(date_str, "%Y-%m-%d %H:%M:%S")
    # Convert to seconds timestamp, then to nanoseconds (1e9)
    return int(dt.timestamp() * 1_000_000_000)

def main():
    # 1. Fetch data for both symbols
    data_1 = fetch_data(SYMBOL_1)
    data_2 = fetch_data(SYMBOL_2)

    if not data_1 or not data_2:
        print("Failed to fetch data. Check your API key or internet connection.")
        return

    print(f"Retrieved {len(data_1)} records for {SYMBOL_1}")
    print(f"Retrieved {len(data_2)} records for {SYMBOL_2}")

    # 2. Combine and Sort Data
    # We map SYMBOL_1 -> "SYM_A" and SYMBOL_2 -> "SYM_B" so C++ code works efficiently
    combined_data = []

    for row in data_1:
        ts = convert_to_nanoseconds(row['datetime'])
        # (timestamp, symbol, price, volume)
        combined_data.append((ts, "SYM_A", row['close'], row['volume']))

    for row in data_2:
        ts = convert_to_nanoseconds(row['datetime'])
        # Add a tiny offset (1ns) to B if timestamps are identical, 
        # just to ensure deterministic ordering in sorting, though not strictly necessary
        combined_data.append((ts, "SYM_B", row['close'], row['volume']))

    # Sort by timestamp (index 0)
    combined_data.sort(key=lambda x: x[0])

    # 3. Write to CSV
    print(f"Writing {len(combined_data)} ticks to {OUTPUT_FILE}...")
    with open(OUTPUT_FILE, 'w', newline='') as f:
        writer = csv.writer(f)
        # No header, as expected by DataReader.cpp
        for row in combined_data:
            writer.writerow(row)

    print("Done! You can now run './backtester'")

if __name__ == "__main__":
    main()