# Limited Order Book

A Limited Order Book implementation in C++.

## Build Instructions

1. Set the environment variable `VCPKG_ROOT` to the path of your vcpkg installation.
2. Run the following command to build the project:

```bash
cmake --build ./build --config Release --target all
```

the output path is set to `./dist`

## Dependencies

1. ZeroMQ: concurrency framework / networking library
2. spdlog: Fast C++ logging library
3. nlohmann-json: JSON for modern C++

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
