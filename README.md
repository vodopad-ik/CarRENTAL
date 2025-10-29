# CarRental (C++/Qt Desktop App)

A simple desktop application for car rental management built with C++ and Qt (Widgets + SQLite). Current MVP includes a Cars tab with CRUD operations backed by SQLite. Customers and Rentals tabs are scaffolded.

## Prerequisites (Linux)

- CMake >= 3.21
- C++17 compiler (gcc/clang)
- Qt6 dev packages (Widgets, Sql)

On Ubuntu/Debian, you can install:

```bash
sudo apt update
sudo apt install -y build-essential cmake qt6-base-dev qt6-base-dev-tools
```

If Qt6 is unavailable on your distro, install Qt5; CMake falls back automatically:

```bash
sudo apt install -y qtbase5-dev
```

## Build

```bash
mkdir -p build
cd build
cmake ..
cmake --build . -j
```

The binary will be located at `build/bin/CarRental`.

## Run

```bash
./bin/CarRental
```

The SQLite database is stored under `~/.carrental/app.db` and is auto-created with required tables on first launch.

## Project Structure

- `src/` C++ sources
  - `MainWindow.*` main UI logic for Cars tab
  - `db/Database.*` SQLite connection and schema + Cars model
- `ui/` Qt Designer `.ui` files
- `CMakeLists.txt` build configuration

## Next Steps

- Implement Customers and Rentals CRUD
- Add validation and better dialogs
- Add filtering, searching, and basic reports
