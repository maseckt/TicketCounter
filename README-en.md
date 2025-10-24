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

## Building on Windows (MSVC)

1. Install Visual Studio 2019+ with "Desktop development with C++" components.
2. Download and install MSYS2: https://www.msys2.org/
3. In MSYS2: `pacman -S mingw-w64-x86_64-gtk4 mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake`
4. Install nlohmann/json via vcpkg or manually.
5. `mkdir build`
6. `cd build`
7. `cmake -G "MinGW Makefiles" ..`
8. `mingw32-make`
9. Run TicketCounter.exe

Alternative with vcpkg:
- `vcpkg install gtkmm nlohmann-json`
- `cmake -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake ..`
- `cmake --build . --config Release`

## Project Structure

- src/: Source code
- include/: Headers
- tests/: Tests
- data/: JSON data

[На русском](README.md)