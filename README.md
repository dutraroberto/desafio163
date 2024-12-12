# Desafio163 - Bitcoin Private Key Recovery Tool

A high-performance Bitcoin private key finder tool written in C++.

## Author
- **Roberto Dutra**
  - GitHub: [@dutraroberto](https://github.com/dutraroberto)

## Overview
This tool is designed to recover Bitcoin private keys using advanced cryptographic techniques and optimized multi-threading.

## Features
- Multi-threaded key search with optimized performance
- Secp256k1 elliptic curve operations
- Bitcoin address generation
- Base58 encoding
- SHA256 and RIPEMD160 hash functions
- Thread-local context caching
- Real-time performance metrics

## Requirements
- Windows 64-bit
- Visual Studio 2022
- CMake 3.10+
- OpenSSL v3.x

## Building
1. Install dependencies:
   - Visual Studio 2022 with C++ development tools
   - CMake 3.10 or higher
   - OpenSSL v3.x (Win64 version)

2. Clone the repository:
   ```bash
   git clone https://github.com/dutraroberto/desafio163.git
   cd desafio163
   ```

3. Build the project:
   ```bash
   build.bat
   ```

## Usage
The executable will be located in `build/Release/Desafio163.exe`

Command line options:
- `--address`: Target Bitcoin address
- `--key`: Partial private key
- `--threads`: Number of threads to use
- `--benchmark`: Run performance test
- `--help`: Show usage information

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Performance Optimizations
- Multi-threaded search algorithm
- Thread-local context caching
- Elliptic curve pre-computation
- Minimized memory allocations
- Optimized cryptographic operations

## Contributing
Contributions are welcome! Please feel free to submit a Pull Request.
