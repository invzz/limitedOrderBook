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

1. [ZeroMQ](https://github.com/zeromq/libzmq): concurrency framework / networking library
   - **Reason**: Used for efficient message passing and communication between different components of the market server and clients.
2. [spdlog](https://github.com/gabime/spdlog): Fast C++ logging library
   - **Reason**: Provides high-performance logging capabilities to track the application's behavior and debug issues.
3. [nlohmann-json](https://github.com/nlohmann/json): JSON for modern C++
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

## Thread Management

The project uses multithreading to handle concurrent operations efficiently. Here is an overview of how threads are managed:

1. **Order Processing**: A dedicated thread pool is used to handle incoming orders. Each order is processed in a separate thread to ensure quick response times and to prevent blocking the main server thread.
2. **Market Updates**: Another set of threads is responsible for broadcasting market updates to all connected clients. This ensures that clients receive real-time updates without delay.
3. **Synchronization**: Mutexes and condition variables are used to synchronize access to shared resources, such as the order book and market data. This prevents race conditions and ensures data consistency.
4. **Thread Safety**: All critical sections of the code are protected using appropriate locking mechanisms to ensure thread safety. The use of atomic operations is also employed where applicable to reduce locking overhead.

By using these techniques, the project achieves high performance and scalability while maintaining data integrity and consistency.

## Future Developments

To further enhance the Limited Order Book project, the following future developments are planned:

1. **Performance Optimization**: Implement advanced data structures and algorithms to improve the performance of order matching and market data processing.
2. **Scalability Improvements**: Introduce distributed computing techniques to handle a larger volume of orders and market data, ensuring the system can scale horizontally.
3. **Advanced Trading Algorithms**: Develop and integrate more sophisticated trading algorithms and bots to simulate various market conditions and strategies.
4. **Enhanced Security**: Implement additional security measures to protect against potential threats and ensure the integrity and confidentiality of market data.
5. **User Interface**: Create a graphical user interface (GUI) for better visualization of market data and easier interaction with the market server.
6. **API Enhancements**: Expand the API to provide more functionality and flexibility for external clients and developers.
7. **Testing and Validation**: Increase the coverage of unit tests and introduce automated testing frameworks to ensure the reliability and correctness of the system.

These future developments aim to make the Limited Order Book more robust, efficient, and user-friendly, catering to the needs of various stakeholders in the financial market ecosystem.
