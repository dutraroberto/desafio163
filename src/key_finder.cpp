#define _CRT_SECURE_NO_WARNINGS
#include "key_finder.h"
#include "bitcoin_utils.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <ctime>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdint>

// Definição da constante de classe
constexpr char KeyFinder::HEX_CHARS[];

std::string KeyFinder::getTimestamp(bool forFilename) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    
    if (forFilename) {
        ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    } else {
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    }
    
    return ss.str();
}

void KeyFinder::initLookupTables() {
    // Pré-computar todas as combinações possíveis de 2 caracteres hex
    for (int i = 0; i < 256; ++i) {
        hexLookup_[i] = std::string(1, HEX_CHARS[(i >> 4) & 0xF]) + HEX_CHARS[i & 0xF];
    }
}

KeyFinder::KeyFinder(const std::string& targetAddress, const std::string& partialKey, size_t threadCount)
    : targetAddress_(targetAddress)
    , partialKey_(partialKey)
    , running_(false)
    , paused_(false)
    , testsCount_(0)
    , lastTestsCount_(0)
    , currentTotal_(0)
    , lastUpdate_(std::chrono::steady_clock::now())
    , startTime_(std::chrono::steady_clock::now())
    , startTimeStr_(getTimestamp(false))  // Armazena o horário de início
    , peakSpeed_(0)
    , checkpointFile_("checkpoint.dat") {
    
    // Inicializar posições dos X's
    for (size_t i = 0; i < partialKey_.length(); i++) {
        if (partialKey_[i] == 'x' || partialKey_[i] == 'X') {
            xPositions_.push_back(i);
        }
    }

    // Inicializar lookup tables
    initLookupTables();
    
    // Inicializar threads
    threads_.resize(threadCount);
}

KeyFinder::~KeyFinder() {
    stop();
}

void KeyFinder::start() {
    if (running_) return;
    
    running_ = true;
    paused_ = false;
    testsCount_.store(0, std::memory_order_relaxed);
    lastTestsCount_ = 0;
    currentTotal_ = 0;
    startTime_ = std::chrono::steady_clock::now();
    startTimeStr_ = getTimestamp(false);  // Armazena o horário de início
    lastUpdate_ = startTime_;
    peakSpeed_ = 0;

    // Iniciar threads
    for (size_t i = 0; i < threads_.size(); ++i) {
        threads_[i] = std::thread(&KeyFinder::workerThread, this, i);
    }
}

void KeyFinder::stop() {
    if (!running_) return;
    running_ = false;
    
    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void KeyFinder::pause() {
    paused_ = true;
}

void KeyFinder::resume() {
    paused_ = false;
}

uint64_t KeyFinder::getTestsPerSecond() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate_).count();
    if (elapsed == 0) return 0;
    
    uint64_t current = testsCount_.load(std::memory_order_relaxed);
    return (current - lastTestsCount_) / elapsed;
}

void KeyFinder::saveCheckpoint(uint64_t currentIndex) {
    std::lock_guard<std::mutex> lock(checkpointMutex_);
    std::ofstream file(checkpointFile_, std::ios::binary);
    if (file) {
        file.write(reinterpret_cast<const char*>(&currentIndex), sizeof(currentIndex));
        file.write(reinterpret_cast<const char*>(&testsCount_), sizeof(testsCount_));
    }
}

uint64_t KeyFinder::loadCheckpoint() {
    std::lock_guard<std::mutex> lock(checkpointMutex_);
    std::ifstream file(checkpointFile_, std::ios::binary);
    uint64_t currentIndex = 0;
    if (file) {
        file.read(reinterpret_cast<char*>(&currentIndex), sizeof(currentIndex));
        uint64_t savedTestsCount;
        file.read(reinterpret_cast<char*>(&savedTestsCount), sizeof(savedTestsCount));
        testsCount_ = savedTestsCount;
    }
    return currentIndex;
}

void KeyFinder::workerThread(size_t threadId) {
    const uint64_t totalCombinations = static_cast<uint64_t>(std::pow(16, xPositions_.size()));
    const uint64_t combinationsPerThread = totalCombinations / threads_.size();
    const uint64_t start = threadId * combinationsPerThread;
    const uint64_t end = (threadId == threads_.size() - 1) ? totalCombinations : (threadId + 1) * combinationsPerThread;

    // Pré-alocação de memória
    std::vector<std::string> batchCandidates;
    batchCandidates.reserve(BATCH_SIZE);
    std::string candidateKey = partialKey_;
    uint64_t localTestsCount = 0;
    bool keyFound = false;
    std::string foundKey;

    // Tempo da última atualização de estatísticas
    auto lastStatsUpdate = std::chrono::steady_clock::now();
    uint64_t lastLocalCount = 0;
    uint64_t currentTotal = 0;

    // Ajuste do loop para OpenMP usando signed int para o índice
    const int64_t numBatches = static_cast<int64_t>((end - start + BATCH_SIZE - 1) / BATCH_SIZE);
    
    #pragma omp parallel for schedule(dynamic) if(numBatches >= 100) shared(keyFound, foundKey)
    for (int64_t batchIndex = 0; batchIndex < numBatches; ++batchIndex) {
        if (keyFound || !running_) continue;
        
        if (paused_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        const uint64_t batchStart = start + batchIndex * BATCH_SIZE;
        const uint64_t batchEnd = std::min(batchStart + BATCH_SIZE, end);
        
        std::vector<std::string> localBatchCandidates;
        localBatchCandidates.reserve(BATCH_SIZE);
        std::string localCandidateKey = partialKey_;

        // Processamento em batch
        for (uint64_t i = batchStart; i < batchEnd; ++i) {
            uint64_t value = i;
            localCandidateKey = partialKey_;
            
            // Otimização usando lookup table para conversão hex
            for (size_t pos = 0; pos < xPositions_.size(); pos += 2) {
                if (pos + 1 < xPositions_.size()) {
                    uint8_t byte = (value & 0xFF);
                    const std::string& hexPair = hexLookup_[byte];
                    localCandidateKey[xPositions_[pos]] = hexPair[0];
                    localCandidateKey[xPositions_[pos + 1]] = hexPair[1];
                    value >>= 8;
                } else {
                    localCandidateKey[xPositions_[pos]] = HEX_CHARS[value & 0xF];
                }
            }
            
            localBatchCandidates.push_back(localCandidateKey);
            localTestsCount++;
        }

        // Validar batch de chaves
        for (const auto& key : localBatchCandidates) {
            if (validateKey(key)) {
                auto now = std::chrono::steady_clock::now();
                auto totalElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime_).count();
                uint64_t currentTotal = testsCount_.load(std::memory_order_relaxed);
                uint64_t averageSpeed = totalElapsed > 0 ? currentTotal / totalElapsed : 0;
                
                // Criar nome do arquivo com timestamp
                std::string filename = "key_found_" + getTimestamp(true) + ".txt";
                
                std::ofstream outFile(filename);
                if (outFile.is_open()) {
                    outFile << "Bitcoin Private Key Found!\n";
                    outFile << "=======================\n\n";
                    outFile << "Search started at: " << startTimeStr_ << "\n";
                    outFile << "Key found at: " << getTimestamp(false) << "\n\n";
                    outFile << "Details:\n";
                    outFile << "- Target Address: " << targetAddress_ << "\n";
                    outFile << "- Private Key: " << key << "\n";
                    outFile << "- Search Pattern: " << partialKey_ << "\n\n";
                    outFile << "Performance Statistics:\n";
                    outFile << "- Total Keys Tested: " << currentTotal << "\n";
                    outFile << "- Average Speed: " << averageSpeed << " keys/s\n";
                    outFile << "- Total Time: " << totalElapsed << " seconds\n";
                    outFile.close();
                    
                    std::cout << "\n\nKey found! Details saved to: " << filename << std::endl;
                }
                
                running_ = false;
                break;
            }
        }
    }

    // Se encontrou a chave, processar o resultado
    if (keyFound) {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime_).count();

        std::cout << "\n==========================================\n";
        std::cout << "CHAVE ENCONTRADA!\n";
        
        // Converter para WIF e salvar resultados
        auto bytes = BitcoinUtils::hexStringToBytes(foundKey);
        std::string wif = BitcoinUtils::privateKeyToWIF(bytes);
        std::string address = BitcoinUtils::privateKeyToAddress(bytes);
        
        // Criar diretório results se não existir
        std::filesystem::create_directories("results");
        
        // Gerar nome do arquivo com timestamp
        std::string filename = "results/chave_encontrada_" + getTimestamp(true) + ".txt";
        
        // Salvar em arquivo
        std::ofstream outFile(filename);
        outFile << "=== Chave Bitcoin Encontrada ===\n\n";
        outFile << "Data/Hora: " << getTimestamp() << "\n\n";
        outFile << "Chave privada (HEX): " << foundKey << "\n";
        outFile << "Chave privada (WIF): " << wif << "\n";
        outFile << "Endereco Bitcoin: " << address << "\n\n";
        outFile << "Tempo total: " << duration << " segundos\n";
        outFile << "Total de chaves testadas: " << currentTotal << "\n";
        outFile << "Velocidade máxima: " << peakSpeed_ << " chaves/s\n";
        outFile << "Detalhes da busca:\n";
        outFile << "- Chave parcial usada: " << partialKey_ << "\n";
        outFile << "- Endereco alvo: " << targetAddress_ << "\n";
        outFile << "- Numero de threads: " << threads_.size() << "\n\n";
        outFile << "==========================================\n";
        outFile.close();

        std::cout << "Chave privada (HEX): " << foundKey << "\n";
        std::cout << "Chave privada (WIF): " << wif << "\n";
        std::cout << "Endereco Bitcoin: " << address << "\n";
        std::cout << "Tempo total: " << duration << " segundos\n";
        std::cout << "Total de chaves testadas: " << currentTotal << "\n";
        std::cout << "Velocidade máxima: " << peakSpeed_ << " chaves/s\n";
        std::cout << "==========================================\n";
        
        std::cout << "\nChave salva em: " << filename << "\n";
        std::cout << "\nPressione 'q' para sair...\n";
        
        char ch;
        while ((ch = std::cin.get()) != 'q' && ch != 'Q') {
            // Espera até que 'q' ou 'Q' seja pressionado
        }
        
        stop();
    }
    
    std::cout << std::string(100, ' ') << "\r" << std::flush;
    std::cout << "Thread " << threadId << " finalizada.\n";
}

bool KeyFinder::validateKey(const std::string& candidateKey) {
    try {
        // Converter a chave para bytes
        auto bytes = BitcoinUtils::hexStringToBytes(candidateKey);
        
        // Gerar endereço Bitcoin
        std::string address = BitcoinUtils::privateKeyToAddress(bytes);
        
        // Verificar se o endereço corresponde ao alvo
        if (address == targetAddress_) {
            return true;
        }
        
        return false;
    }
    catch (const std::exception& e) {
        // Em caso de erro na conversão ou geração do endereço
        std::cerr << "Erro ao validar chave: " << e.what() << std::endl;
        return false;
    }
}
