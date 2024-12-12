cmake_minimum_required(VERSION 3.10)
project(Desafio163)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configuração do OpenSSL
if(WIN32)
    # Caminhos específicos do OpenSSL
    set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")
    set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
    set(OPENSSL_LIBRARIES "${OPENSSL_ROOT_DIR}/lib")
    set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libcrypto.lib")
    set(OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libssl.lib")
    
    # Configurações adicionais
    set(OPENSSL_USE_STATIC_LIBS TRUE)
    set(OPENSSL_MSVC_STATIC_RT TRUE)
    
    # Adiciona os diretórios ao path de busca
    include_directories(${OPENSSL_INCLUDE_DIR})
    link_directories(${OPENSSL_LIBRARIES})
endif()

# Encontrar OpenSSL
find_package(OpenSSL REQUIRED)

if(NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL não encontrado. Por favor, instale o OpenSSL v3.x de https://slproweb.com/products/Win32OpenSSL.html")
endif()

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
