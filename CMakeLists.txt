cmake_minimum_required(VERSION 3.10)
project(Desafio163)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Habilitar OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Force UTF-8 encoding for MSVC
if(MSVC)
    add_compile_options(/utf-8)
    # Adicionar /EHsc para manipulação de exceções
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ob2 /Oi /Ot /GL /arch:AVX2 /fp:fast /Qpar")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -ffast-math")
endif()

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

# Adicionar a biblioteca secp256k1
add_subdirectory(third_party/secp256k1)

# Incluir diretórios
include_directories(third_party/secp256k1/include)

# Vincular a biblioteca secp256k1
set(SECP256K1_LIBRARIES ${CMAKE_BINARY_DIR}/third_party/secp256k1/lib/Release/libsecp256k1.lib)

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
    third_party/secp256k1/include
)

# Link libraries
target_link_libraries(Desafio163 PRIVATE 
    OpenSSL::SSL 
    OpenSSL::Crypto
    $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>
    ${SECP256K1_LIBRARIES}
)
