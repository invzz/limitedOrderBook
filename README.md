# Limited Order Book

A Limited Order Book implementation in C++.

## Build Instructions

### Installing vcpkg

#### Windows

1. Open PowerShell and clone the vcpkg repository:

   ```powershell
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   ```

2. Set the `VCPKG_ROOT` environment variable:

   ```powershell
   $env:VCPKG_ROOT = "C:\path\to\vcpkg"
   ```

#### Linux

1. Open a terminal and clone the vcpkg repository:

   ```bash
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   ./bootstrap-vcpkg.sh
   ```

2. Set the `VCPKG_ROOT` environment variable:

   ```bash
   export VCPKG_ROOT=/path/to/vcpkg
   ```

### Build using CMake

#### Windows

1. Open PowerShell and navigate to the project directory:

   ```powershell
   cd /path/to/project
   ```

2. Create a build directory and navigate into it:

   ```powershell
   mkdir build
   cd build
   ```

3. Run CMake to configure the project:

   ```powershell
   cmake -DCMAKE_TOOLCHAIN_FILE=$env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake -DCMAKE_BUILD_TYPE=Release ..
   ```

4. Build the project:

   ```powershell
   cmake --build . --config Release --target all
   ```

#### Linux

1. Open a terminal and navigate to the project directory:

   ```bash
   cd /path/to/project
   ```

2. Create a build directory and navigate into it:

   ```bash
   mkdir build
   cd build
   ```

3. Run CMake to configure the project:

   ```bash
   cmake -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release ..
   ```

4. Build the project:

   ```bash
   cmake --build . --config Release --target all
   ```

The output path is set to `./dist`

## Dependencies

1. ZeroMQ: concurrency framework / networking library
   - **Reason**: Used for efficient message passing and communication between different components of the market server and clients.
2. spdlog: Fast C++ logging library
   - **Reason**: Provides high-performance logging capabilities to track the application's behavior and debug issues.
3. nlohmann-json: JSON for modern C++
   - **Reason**: Used for parsing and serializing JSON data, which is essential for handling configuration files and communication protocols.

## Market design

The market design follows a layered architecture to ensure separation of concerns and modularity. The main components are:

1. **Models**: Represent the core data structures such as orders, trades, and market data.
2. **Repositories**: Provide in-memory storage and retrieval of models using an unordered_map.
3. **Services**: Contain the business logic for processing orders, matching trades, and managing market states.
4. **Commands**: Encapsulate actions or operations that can be performed on the market.
5. **Mediator / Handler**: Manage the communication between different components, ensuring loose coupling.
6. **Controller**: Act as an interface between the market server and the external clients, handling incoming requests and responses.
7. **Market Server**: The main server application that runs the market, processes incoming orders, and broadcasts market updates.
8. **Market Client**: A client application that interacts with the market server, capable of placing orders and receiving market data.
   1. **SimpleBot**: A bot that places orders randomly to simulate market activity.
   2. **SmartBot**: A bot that uses technical market indexes to make informed trading decisions.
