[🇷🇺 На русском](README.md)

# TicketCounter

Cross-platform application for counting completed tickets with GTK4 GUI. Allows tracking the number of tickets per day, setting daily goals, viewing statistics and charts of completed tasks. Supports configuring the counter reset time, time zones. The application automatically saves data to JSON file and restores it on launch.

## Requirements

- C++17+
- GTK4 (gtkmm4)
- CMake 3.16+
- nlohmann::json

## Building on Linux

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
./TicketCounter
```
