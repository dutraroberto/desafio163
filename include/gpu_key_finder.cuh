#pragma once

#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <string>
#include <vector>

class GPUKeyFinder {
public:
    GPUKeyFinder();
    ~GPUKeyFinder();

    // Inicializa recursos da GPU
    bool initialize();

    // Processa um lote de chaves na GPU
    bool processKeyBatch(const std::vector<std::string>& partialKeys, const std::string& targetAddress);

    // Obtém a chave encontrada (se houver)
    std::string getFoundKey() const;

    // Configurações da GPU
    void setBlockSize(int size) { blockSize = size; }
    void setNumBlocks(int num) { numBlocks = num; }

private:
    // Recursos CUDA
    cudaStream_t stream;
    void* d_keys;        // Device memory para chaves
    void* d_address;     // Device memory para endereço alvo
    void* d_found;       // Device memory para flag de chave encontrada
    
    // Configurações
    int blockSize;       // Tamanho do bloco CUDA
    int numBlocks;       // Número de blocos CUDA
    bool initialized;    // Flag de inicialização

    // Tamanhos de memória
    static const size_t KEY_SIZE = 64;        // Tamanho da chave em bytes
    static const size_t ADDR_SIZE = 35;       // Tamanho do endereço em bytes
    static const size_t BATCH_SIZE = 1024;    // Tamanho do lote de chaves

    // Funções auxiliares
    void cleanup();
    bool allocateDeviceMemory();
    bool copyDataToDevice(const std::vector<std::string>& keys, const std::string& address);
};

// Kernel CUDA para processamento de chaves
__global__ void processKeysKernel(
    const unsigned char* keys,
    const unsigned char* targetAddress,
    int numKeys,
    bool* found,
    unsigned char* foundKey
);
