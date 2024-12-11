cmake_minimum_required(VERSION 3.10)
project(Desafio163)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Add executable
add_executable(Desafio163 
    src/main.cpp
    src/bitcoin_utils.cpp
    src/key_finder.cpp
)

# Include directories
target_include_directories(Desafio163 PRIVATE 
    ${OPENSSL_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(Desafio163 PRIVATE 
    OpenSSL::SSL 
    OpenSSL::Crypto
)
