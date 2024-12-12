cmake_minimum_required(VERSION 3.10)
project(Desafio163 LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Habilitar OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Configuração do CUDA
find_package(CUDA REQUIRED)
set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_ARCHITECTURES 75) # RTX 2060 usa arquitetura Turing (SM 75)

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

# Arquivos fonte
set(SOURCES
    src/main.cpp
    src/bitcoin_utils.cpp
    src/key_finder.cpp
    src/gpu_key_finder.cu
)

# Cabeçalhos
set(HEADERS
    include/key_finder.h
    include/gpu_key_finder.cuh
)

# Configuração do executável
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Diretórios de inclusão
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OPENSSL_INCLUDE_DIR}
    ${CUDA_INCLUDE_DIRS}
)

# Bibliotecas
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenSSL::Crypto
    OpenSSL::SSL
    ${CUDA_LIBRARIES}
    $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>
)

# Configurações do compilador
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /MP)
    set_property(TARGET ${PROJECT_NAME} PROPERTY CUDA_SEPARABLE_COMPILATION ON)
endif()

# Copiar DLLs do OpenSSL para o diretório de saída
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${OPENSSL_ROOT_DIR}/bin/libcrypto-3-x64.dll
            ${OPENSSL_ROOT_DIR}/bin/libssl-3-x64.dll
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()
